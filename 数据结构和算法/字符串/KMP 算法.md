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

# next 数组

`next[i]` 表示 `preffix[i]` 的非平凡最大 border。

求解 `next[i]` 时，遍历 `preffix[i]` 的所有 border，即 `next[i-1], next[next[i-1]], ..., 0`，检查后一个字符是否等于 `S[i]`，如果等于，则 `next[i]` 为当前所检查的 border 加 $1$。

```c++
vector<int> buildNexts(const string &s)
{
    const int n = s.length();
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

---

# KMP 算法

KMP 算法用于快速在主串中查找模式串出现的位置，相较于暴力匹配，KMP 使用 border 的性质和模式串的 next 数组进行快速跳转，避免许多不必要的状态搜索。

KMP 算法中，使用两个指针 $i, j$ 分别表示主串和模式串当前遍历到的位置，其中主串指针不断前进，仅有模式串的指针在字符不匹配时进行回溯。

相较于暴力匹配每次字符不匹配时直接回到起点，KMP 算法将指针 $j$ 的位置回溯至 `nexts[j-1] + 1` 的位置进行下一轮匹配，而不需要进行前边字符的匹配。这是因为 $S[1, \dots , j-1]$ 的最大 border 为 `nexts[j-1]`，而已知主串已匹配部分的后缀与模式串已匹配部分的后缀相同，利用 next 数组中存储的最大 border，可证明主串以匹配部分后缀长度为 border 的部分与模式串前缀长度为 border 的部分相匹配，因此这段区间无需再次尝试匹配，将模式串指针移动到 `nexts[j-1] + 1` 尝试进行下个位置的匹配即可。

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
>当 $i = 6, j = 6$ 的位置匹配失败时，由于串 `aabaa` 的最大 border 为 `aa`，已知主串的 $i-1$ 位置之前的 `aa` 和模式串 $S[1, \dots , j-1]$ 的后缀 `aa` 成功匹配，则这段主串和模式串的前缀 `aa` 也可成功匹配，而当前 $i$ 指针位于 `aa` 的后一个位置，因此将 $j$ 指针移动到模式串的前缀 `aa` 的下一个位置即 `nexts[j-1] + 1` 进行下一轮匹配，跳过前边已知一定可以匹配成功的部分，尝试在下个位置继续匹配。

```c++
int kmp(string &s, string &t)
{
	// 建立模式串的 next 数组
	vector<int> nexts = buildNexts(t);

	int n = s.length();
	int m = t.length();
	int i = 0, j = 0;
	while (i < n && j < m)
	{
		// 当前位置匹配成功，尝试匹配下个位置
		if (s[i] == t[j])
		{
			++i;
			++j;
			// 模式串匹配完成，返回主串中匹配到的起始位置
			if (j == m)
				return i - m;
		}
		// 匹配不成功则跳转 j 指针
		else
		{
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

