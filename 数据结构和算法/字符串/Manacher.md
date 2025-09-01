# 回文串

**回文串**：对于一个字符串 $S = S[1]S[2]...S[n]$，其**反串** $R(S) = S[n]S[n-1]...S[1]$，若满足 $S = R(S)$，则 $S$ 为回文串。

**回文中心**：奇长度回文串回文中心为 $S[\frac{n}{2}]$，偶长度回文串回文中心为 $S[\frac{n}{2}]$ 与 $ S[\frac{n}{2} + 1]$ 中间。

**回文半径**：从回文中心开始到两端点之间的字符个数，等于 $S$ 长度的一半（上取整）。

常用二元组 $<回文中心, 回文半径>$ 表示一个回文串。

🧩**回文半径的二分性：**回文半径 $-1$ 等价于同时删掉回文串的首位字母，依然是回文串。

🧩**回文串和 border：**对于一个回文串，其回文前（后）缀等价于 border。

# Manacher

**Manacher** 算法通过遍历字符串的每个位置，利用已求得的回文串信息求得新回文中心的初始半径然后向外拓展，在线性时间内求出字符串的所有回文子串。

## 预处理

由于偶长度回文串的回文中心不在字符位置上，为了将奇偶回文串的处理方式统一起来，可以将 $S$ 的每两个字符之间以及开头结尾处插入一个特殊字符 `#`，得到奇数长度的 $S^{\#}$。

经过预处理之后，所有的回文串都变成奇数长度，且首尾一定是 `#`。

```c++
void turn(string &s)
{
	string temp = "#";
	for (const char c : s)
	{
		temp += c;
		temp += '#';
	}
	s = temp;
}
```

## 最右回文串的维护

从左到右遍历每个位置，记录各位置的**最大回文半径**，同时维护已求得的**最右回文串** $S[l, r]$ 及其回文中心 $p$。

对于每个位置 $i$，当 $i > r$ 时，从当前位置开始向两侧暴力地进行拓展，求得回文半径 $len_i$，由于此时以 $i$ 为中心的回文串右端点一定在 $r$ 的右侧，将最右回文串更新为 $p = i, l = i - len_i + 1, r = i + len_i - 1$。

当 $i$ 在 $P$ 的内部时，根据 $P$ 的回文性，可以求出 $i$ 关于 $p$ 对称的位置 $j = 2p - i$。由于 $len_j$ 已求得，根据 $P$ 的回文性，$len_i$ 可以**继承** $len_j$ 在 $P$ 范围内部分的长度，根据 $len_j$ 是否超出了 $P$ 的范围，再分两种情况讨论。

当 $l_j > l$ 时，由于 $P$ 的回文性，可以确定 $len_i$ 等于 $len_j$，且不能再继续拓展。由 $l_j > l$ 可得，$r_i < r$，此时最右回文串没有发生变化。

当 $l_j <= l$ 时，$len_i$ 首先根据 $P$ 的回文性继承 $len_j$ 在 $P$ 内部的长度，然后继续向两侧暴力拓展，求得回文半径 $len_i$。

```c++
vector<int> manacher(const string &s)
{
	const int n = s.size();
	int l = -1, r = -1, p = -1;
	vector<int> lens(n);
	for (int i = 0; i < n; ++i)
	{
		if (i > r)
		{
			for (int len = 0; i >= len && i + len < n; ++len)
			{// 枚举的 len 为不包括中心点的拓展长度
				if (s[i - len] != s[i + len])
					break;
				lens[i] = len + 1;
			}
			l = i - lens[i] + 1;
			r = i + lens[i] - 1;
			p = i;
		}
		else
		{
			int j = 2 * p - i;
			if (j - (lens[j] - 1) > l)
			{// 完全在范围内直接对称
				lens[i] = lens[j];
			}
			else
			{// 不完全在范围内从已知安全长度开始拓展
				lens[i] = r - i + 1;
				for (int len = lens[i]; i >= len && i + len < n; ++len)
				{
					if (s[i - len] != s[i + len])
						break;
					lens[i] = len + 1;
				}
				if (i + lens[i] - 1 > r)
				{
					l = i - lens[i] + 1;
					r = i + lens[i] - 1;
					p = i;
				}
			}
		}
	}
	return lens;
}
```

---

# 例题

[P3805 【模板】manacher - 洛谷](https://www.luogu.com.cn/problem/P3805)（模板）

[P4555 [国家集训队] 最长双回文串 - 洛谷](https://www.luogu.com.cn/problem/P4555)（不饱和回文串）

