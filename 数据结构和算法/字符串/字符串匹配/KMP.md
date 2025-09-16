> 本笔记中假定算法讲解中字符串下标从 $1$ 开始，代码中下标从 $0$ 开始。

# border，周期和循环节

## border

如果字符串 $S$ 的一个字串**既是它的前缀，又是它的后缀**，则该字串成为 $S$ 的一个 **border**。

* 特殊地，字符串本身也可是是他的 border，具体根据语境判断。
* 特殊地，border 也可以指这个字串的长度，具体根据语境判断。

**例：**

>$S$：
>
>```
>bbabbab
>```
>
>$S$ 的所有 border：
>
>```
>b
>bbab
>bbabbab
>```

**🧩 border 的传递性：**

$S$ 的 border 的 border 也是 $S$ 的 border。

求 $S$ 的所有 border 等价于求所有前缀的最大 border。

## 周期

对于字符串 $S$ 和正整数 $p$，如果对于任意 $i$，都有 $p \lt i \leq \lvert S \rvert$，则称 $p$ 为字符串 $S$ 的一个**周期**。

- 特殊地，$p = \lvert S \rvert$ 一定是 $S$ 的周期。

**例：**

> $S$：
>
> ```
> bbabbab
> ```
>
> $S$ 的所有周期：
>
> ```
> bba
> bbabba
> bbabbab
> ```

🧩 **border 与周期的关系：**$p$ 是 $S$ 的 border $\iff$ $\lvert S \rvert - p$ 是 $S$ 的周期。

🧩 **周期定理：**若 $p, q$ 均为 $S$ 的周期，则 $\gcd(p, q)$ 也为 $S$ 的周期。

🧩 一个串的 Boredr 数量为 $n$，他们组成了 $\log n$ 个等差数列。

## 循环节

若字符串 $S$ 的周期 $p$ 满足 $p \mid \lvert S \rvert$，则称 $p$ 为 $S$ 的一个**循环节**。

* 特殊地，$p = \lvert S \rvert$ 一定是 $S$ 的循环节。

**例：**

> $S$：
>
> ```
> bbabbabba
> ```
>
> $S$ 的所有循环节：
>
> ```
> bba
> bbabbabba
> ```

---

# next 数组和 border 树

$next[i]$ 表示 $preffix[i]$ 的非平凡最大 border。至于为什么叫做 next 数组，详见[KMP 算法](# KMP 算法)原理。

求解 $next[i]$ 时，遍历 $preffix[i]$ 的所有 border，即 $next[i-1], next[next[i-1]], ..., 0$。检查后一个字符是否等于 $S[i]$，如果等于，则 $next[i]$ 为当前所检查的 border 加 $1$。

```c++
vector<int> buildNexts(string &s)
{
	int n = s.length();
	vector<int> nexts(n);
	for (int i = 1; i < n; ++i)
	{
		nexts[i] = nexts[i-1];
		while (nexts[i] > 0 && s[i] != s[nexts[i]])
		{
			nexts[i] = nexts[nexts[i] - 1];
		}
		if (s[i] == s[nexts[i]])
			++nexts[i];
	}
	return nexts;
}
```

next 数组通过每个节点的最大 border 形成若干条 border 链，这些链组合在一起形成了一颗树形结构，称为 **border 树**。

---

# KMP 算法

KMP 算法用于快速在主串中查找模式串出现的位置，相较于暴力匹配，KMP 使用 border 的性质和模式串的 next 数组进行快速跳转，避免许多不必要的状态搜索。

KMP 算法中，使用两个指针 $i, j$ 分别表示主串和模式串当前遍历到的位置，其中主串指针不断前进，仅有模式串的指针在字符不匹配时进行回溯。

相较于暴力匹配每次字符不匹配时直接回到起点，KMP 算法将指针 $j$ 的位置回溯至 $next[j - 1] + 1$ 的位置进行下一轮匹配，而不需要进行前边字符的匹配。这是因为 $S[1, j - 1]$ 的最大 border 为 $next[j - 1]$，而已知主串已匹配部分的后缀与模式串已匹配部分的后缀相同，利用 next 数组中存储的最大 border，可证明主串以匹配部分后缀长度为 border 的部分与模式串前缀长度为 border 的部分相匹配，因此这段区间无需再次尝试匹配，将模式串指针移动到 $next[j - 1] + 1$ 尝试进行下个位置的匹配即可。

>这也是 `next` 数组名称的由来——它指明了模式串匹配失败时，指针应跳转的“下一个”位置。

**例：**

>主串：
>
>```
>a a b a a b a a f
>```
>
>模式串：
>
>```
>a a b a a f
>```
>
>模式串的 next 数组：
>
>```
>0 1 0 1 2 0
>```
>
>当 $i = 6, j = 6$ 的位置匹配失败时，由于串 `aabaa` 的最大 border 为 `aa`，已知主串的 $i-1$ 位置之前的 `aa` 和模式串 $S[1, j-1]$ 的后缀 `aa` 成功匹配，则这段主串和模式串的前缀 `aa` 也可成功匹配，而当前 $i$ 指针位于 `aa` 的后一个位置，因此将 $j$ 指针移动到模式串的前缀 `aa` 的下一个位置即 $nexts[j-1] + 1$ 进行下一轮匹配，跳过前边已知一定可以匹配成功的部分，尝试在下个位置继续匹配。

```c++
int kmp(string &s, string &t)
{
	// 建立模式串的 next 数组
	vector<int> nexts = buildNexts(t);

	int n = s.length();
	int m = t.length();
	int i = 0, j = 0;
	while (i < n)
	{
		if (s[i] == t[j])
		{
			// 当前位置匹配成功，尝试匹配下个位置
			++i;
			++j;
			// 模式串匹配完成，返回主串中匹配到的起始位置
			if (j == m)
				return i - m;
		}
		else
		{
			// 匹配不成功则跳转 j 指针
			if (j != 0)
				j = nexts[j-1];
			// 如果第一个字符都不匹配，只需将 i 指针移动到下一处即可
			else
				++i;
		}
	}
	// 匹配失败返回 -1
	return -1;
}
```

如果需要找出所有匹配的字串位置，只需每次找到完全匹配的字串之后，类比匹配失败的情况，将 $j$ 指针转移到 $nexts[j] + 1$ （不是转移到 $nexts[j-1] + 1$ 因为当前位置是匹配成功的）的位置继续下一轮匹配即可。

```c++
vector<int> kmp(string &s, string &t)
{
	vector<int> indexs;
	vector<int> nexts = buildNexts(t);

	int n = s.length();
	int m = t.length();
	int i = 0, j = 0;
	while (i < n)
	{
		if (s[i] == t[j])
		{
			++i;
			++j;
			// 模式串完全匹配则记录子串起始位置，跳转 j 指针
			if (j == m)
			{
				indexs.push_back(i - m);
				// 由于前边 ++j 操作，需将 j 指针回调
				j = nexts[j-1];
			}
		}
		else
		{
			if (j != 0)
				j = nexts[j-1];
			else
				++i;
		}
	}
	// 返回所有字串起始下标
	return indexs;
}
```

# KMP 自动机

**KMP 自动机**可类比[AC 自动机](./AC自动机.md)的 Trie 图优化。~~但是 AC 自动机的优化似乎更容易理解。~~

由于 KMP 算法在匹配失败时需要沿 border 链多次跳转，在极端数据情况下可能会退化成暴力枚举的时间复杂度，因此可以根据 border 树上的节点进行预处理，找出当已经匹配成功任意数量个字符时，再匹配下一个字符时，对于可能出现的任意字符，模式串指针应该跳转到什么位置，以便在匹配时一步到位，无需多次跳转。将时间复杂度优化为严格的 $O(\vert pattern \vert)$。

预处理时，需要一个一维数组 $fail$ 来存储每个节点的最大 border（即之前的 next 数组）和一个二维数组 $next$ 来表示每个位置 $i$ 匹配到每个字符 $cj$ 时应跳转的位置 $next[i][ch]$。

逐字符对 next 数组进行预处理时，当匹配到第 $i$ 个字符时，若匹配新字符 $ch$ 成功时，设置 $next[i][ch] = i + 1$，成功匹配的字符数计数加一，模式串匹配的指针后移；否则，$i$ 随 $i - 1$ 处的 fail 链跳转。由于 $fail[i]$ 处的信息已被处理过且包含所有更小 border 处的信息，所以 fail 链只需一次跳转即可得到 $next[i][ch]$ 指向的位置，即 $next[i][ch] = next[fail[i - 1]][ch]$。

对于模式串的匹配指针 $i$，正在匹配第 $i$ 个位置同样也可以表示已经匹配了 $i$ 个字符。而对于一个长度为 $n$ 的模式串，统计计数共计有 $n + 1$ 种不同情况。当需要匹配所有出现的字符串时，应将 $next$ 数组的第一维长度设为 $n + 1$，$nexts[n][ch]$ 表示已经找到一个模式串之后再找下一个模式串应该跳转到哪个位置。~~或者也可以不额外开一个位置，匹配完成时根据 fail 链回退亦可。~~

```c++
vector<array<int, 26>> buildNext(string &pattern, vector<int> &fails)
{
	int m = pattern.length();

	vector<array<int, 26>> nexts(m + 1);
	for (int i = 0; i <= m; ++i)
	{
		for (int index = 0; index < 26; ++index)
		{
			if (i < m && index + 'a' == pattern[i])	// 字符串下标不能越界
			{
				nexts[i][index] = i + 1;
			}
			else
			{
				if (i == 0)
					nexts[i][index] = 0;
				else
					nexts[i][index] = nexts[fails[i - 1]][index];
			}
		}
	}
	return nexts;
}

```

```c++
vector<int> kmp(string &text, string &pattern)
{
	int n = text.length();
	int m = pattern.length();

	vector<int> indexs;

	int index = 0;
	for (int i = 0; i < n; ++i)
	{
		index = nexts[index][text[i] - 'a'];
		if (index == m)	// 若只找第一个出现位置则直接返回
			 indexs.push_back(i - m + 1);
	}
	return indexs;
}
```

---

# 模板

## 朴素 KMP

```c++
class KMP
{
private:
	string pattern{};
	vector<int> nexts{};

public:
	KMP()
	{}
	KMP(string &s)
	{
		build(s);
	}

public:
	void build(string &s)
	{
		pattern = s;
		int n = s.length();
		nexts.assign(n, 0);
		for (int i = 1; i < n; ++i)
		{
			nexts[i] = nexts[i - 1];
			while (nexts[i] > 0 && s[nexts[i]] != s[i])
			{
				nexts[i] = nexts[nexts[i] - 1];
			}
			if (s[nexts[i]] == s[i])
				++nexts[i];
		}
	}

	vector<int> match(string &text)
	{
		vector<int> indexs;

		int n = text.length();
		int m = pattern.length();
		int i = 0, j = 0;
		while (i < n)
		{
			if (text[i] == pattern[j])
			{
				++i;
				++j;
				if (j == m)
				{
					indexs.push_back(i - m);
					j = nexts[j - 1];
				}
			}
			else
			{
				if (j != 0)
					j = nexts[j - 1];
				else
					++i;
			}
		}
		return indexs;
	}
};
```

## KMP 自动机

```c++
class KMPAM
{
public:
	struct Data
	{
		int fail{0};
		array<int, 26> nexts{};
	};

	string pattern{};
	vector<Data> automaton{};

public:
	KMPAM()
	{}
	KMPAM(string &s)
	{
		build(s);
	}
	
public:
	void build(string &s)
	{
		pattern = s;
		int n = s.length();
		automaton.assign(n + 1, {});
		
		for (int i = 1; i < n; ++i)
		{
			int fail = automaton[i - 1].fail;
			while (fail > 0 && s[fail] != s[i])
			{
				fail = automaton[fail - 1].fail;
			}
			if (s[fail] == s[i])
				++fail;
			automaton[i].fail = fail;
		}

		for (int i = 0; i <= n; ++i)
		{
			for (int index = 0; index < 26; ++index)
			{
				if (i < n && index + 'a' == s[i])
				{
					automaton[i].nexts[index] = i + 1;
				}
				else
				{
					if (i == 0)
						automaton[i].nexts[index] = 0;
					else
						automaton[i].nexts[index] = automaton[automaton[i - 1].fail].nexts[index];
				}
			}
		}
	}

	vector<int> match(string &text)
	{
		int n = text.length();
		int m = pattern.length();
		
		vector<int> indexs;
		
		int index = 0;
		for (int i = 0; i < n; ++i)
		{
			index = automaton[index].nexts[text[i] - 'a'];
			if (index == m)
				indexs.push_back(i - m + 1);
		}
		return indexs;
	}
};
```

---

# 例题

[P3375 【模板】KMP - 洛谷](https://www.luogu.com.cn/problem/P3375)（模板）

