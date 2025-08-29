# ST 表

**ST 表** 是用于解决**可重复贡献**（最值、最大公因数等，这里以区间最大值为例）问题的**区间总贡献**询问的**离线**数据结构。

ST 表基于倍增思想，在序列上建立处若干个长度为 $2^k$ 的区间，通过 [$2$ 个区间](# 查询)贡献的合并，快速求出所询问区间的总贡献。

## 建表

ST 表是一张二维表，其中 $st_{i, j}$ 表示从第 $i$ 个位置开始，长度为 $2 ^ j$ 的区间内的总贡献。

ST 表的第一维长度 $n$ 即为序列的长度，第二维的长度 $m$ 根据序列的长度，从 $0$ 开始的最长一个区间长度不超过序列长度，即 $2 ^ {m - 1} <= n$，$m = \lfloor \log_2 n \rfloor + 1$。

初始时将所有 $st_{i, 0}$ 赋值为序列中第 $i$ 个位置的贡献，表示从 $i$ 到 $i$ 长度为 $1$ 的区间。

```c++
for (int i = 0; i < n; ++i)
{
	st[i][0] = as[i];
}
```

接下来对于 ST 表后边的每一列 $j$，其区间长度为 $2 ^ j$，均由上一列两个长度为 $2 ^ {j - 1}$ 的区间合并而来，起点分别为 $i$ 和 $i + 2 ^ {j - 1}$。其中要保证两个区间内表示的都是有效值。其中第 $0$ 列被填满，之后的每一列比前一列少填充 $2 ^ {j - 1}$ 个位置，因此每列填充的位置数量为 $len_j = n - \sum_{k = 0}^{j - 1} 2^k = n - (2 ^ j - 1)$。

```c++
for (int j = 1; j < m; ++j)
{
	int len = 1 << (j - 1);
	for (int i = 0; i + (1 << j) - 1 < n; ++i)
	{
		st[i][j] = st[i][j - 1] + st[i + len][j - 1];
	}
}
```

## 查询

每次查询区间 $[l, r]$ 内的总贡献时，首先找出以 $l$ 为起点在区间内最长的一个区间，长度为 $len = 2 ^ {\log_2(r - l + 1)}$，然后再选取一个区间覆盖后边未被覆盖的部分。

由于数据可重复贡献的性质，第二个区间长度的要求仅仅是大于未被覆盖部分区间的长度，而由于 $2 ^ {\log_2(r - l + 1)} > len / 2$ 成立，可以直接另第二个区间的长度与第一个区间长度相同，两区间分别表示 $[l, l + len - 1]$ 和 $[r - len + 1, r]$。

两区间贡献值合并得到区间总贡献。

```c++
int query(const int l, const int r)
{
	int power = log2(r - l + 1);
	int len = 1 << power;
	return max(st[l, power], st[r - len + 1][power]);
}
```

---

# 模板

```c++
template <typename T>
class ST
{
private:
	vector<vector<T>> st{};

public:
	ST()
	{}
	
	ST(const vector<T> &v)
	{
		build(v);
	}
	
private:
	// 此函数规定贡献方式
	T merge(const T a, const T b)
	{
		return std::max(a, b);
	}

public:
	void build(const vector<T> &v)
	{
		const int n = v.size();
		const int m = log2(n) + 1;
		st.assign(n, vector<T>(m));

		for (int i = 0; i < n; ++i)
		{
			st[i][0] = v[i];
		}
		for (int j = 1; j < m; ++j)
		{
			int len = 1 << (j - 1);
			for (int i = 0; i < n - (1 << j) + 1; ++i)
			{
				st[i][j] = merge(st[i][j - 1], st[i + len][j - 1]);
			}
		}
	}
	
	T query(const int l, const int r)
	{
		int power = log2(r - l + 1);
		int len = 1 << power;
		return merge(st[l][power], st[r - len + 1][power]);
	}
};
```

---

# 例题

[P3865 【模板】ST 表 & RMQ 问题 - 洛谷](https://www.luogu.com.cn/problem/P3865)（模板）



