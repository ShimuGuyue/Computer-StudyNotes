# 多项式 Hash

将字符串看作是某个进制（base）下的数字串。

$$
H(S) = \sum_{i = 1}^{n = \lvert S \rvert} (S[i] \cdot base^{i})
\\
=(S[1] \cdot base ^ 1 + S[2] \cdot base ^ 2 + \cdots + S[n] \cdot base ^ n) \% Mod
$$

* 优点：无哈希冲突。
* 缺点：数字范围过大，难以存储。

---

# 多项式取模 Hash（模哈）

模哈是为了解决多项式 Hash 的缺点，在效率和冲突率之间进行的折衷：将多项式 Hash 的值对一个较大的质数取模。
$$
H'(S) = H(S) \% Mod
$$
根据**生日悖论**，当检验次数超过 $\sqrt{Mod}$ 时，会有较大概率发生哈希冲突。因此，在模哈中使用的 $Mod$ 最好超过 Hash 检验次数的平方。

---

# 字串 Hash

> 笔记中字符串下标均从 $1$ 开始，代码中字符串下标从 $0$ 开始。

$$
H(S[l, r]) = \sum_{i = l} ^ r (S[i] \cdot base ^ {i - l + 1}) \% Mod
\\
= (S[l] \cdot base ^ 1 + S[l + 1] \cdot base ^ {2} + \cdots + S[r] \cdot base ^ {r - l + 1}) \% Mod
$$

令 $F(i) = H(S[1, i])$
$$
F(l - 1) = (S[1] \cdot base ^ 1 + S[2] \cdot base ^ 2 + \cdots + S[l - 1] \cdot base ^ {l - 1}) \% Mod
\\
F(r) = (S[1] \cdot base ^ 1 + S[2] \cdot base ^ 2 + \cdots + S[r] \cdot base ^ {r}) \% Mod
$$

因此，

$$
F(r) - F(l - 1) = (S[l] \cdot base ^ l + S[l + 1] \cdot base ^ {l + 1} + \cdots + S[r] \cdot base ^ r) \% Mod
\\
H(S[l, r]) = \frac{F(r) - F(l - 1)}{base ^ {l - 1}} \% Mod
$$
所以只需要求出每个前缀的 Hash 值 $F(i)$，就可以快速求出字串的 Hash 值。

```c++
vector<int64_t> bulidPreHashs(string &s, int64_t base, int64_t mod)
{
	int n = s.length();
	vector<int64_t> pre_hashs(n);
	pre_hashs[0] = static_cast<int64_t>(s[0])
	for (int i = 1; i < n; ++i)
	{
		pre_hashs[i] = (pre_hashs[i - 1] + s[i] * pow(base, i) % mod) % mod;
	}
	return pre_hashs;
}
```

```c++
int64_t getHash(vector<int64_t> &pre_hashs, int l, int r, int64_t base, int64_t mod)
{
	return (pre_hashs[r] - pre_hashs[l - 1] + mod) % mod * inv(pow(base, l - 1), mod) % mod;
}
```

> 若仅判断两子串是否相等，可以不用求出两串的具体 Hash 值，只需将位置靠前的子串的前缀哈希值乘上相应的倍数，检测与靠后的子串前缀 Hash 值是否相等即可（见模板），以避免繁琐的求逆元操作。

---

# 模板

```c++
class StringHash
{
private:
	int64_t mod{};
	int64_t base{};
	int64_t inv_base{};
	vector<int64_t> pows{};
	vector<int64_t> invs{};
	vector<int64_t> pre_hashs{};

private:
	int64_t quickPow(int64_t base, int64_t n)
	{
		int64_t ans = 1;
		while (n)
		{
			if (n & INT64_C(1))
				ans = ans * base % mod;
			base = base * base % mod;
			n >>= 1;
		}
		return ans;
	}

public:
	StringHash()
	{}
	StringHash(string &s, int64_t mod = INT64_C(1000000007), int64_t base = 131)
	{
		buildPreHashs(s, mod, base);
	}

	void buildPreHashs(string &s, int64_t mod, int64_t base)
	{
		this->mod = mod;
		this->base = base;
		this->inv_base = quickPow(base, mod - 2);

		const int n = s.length();
		pows.assign(n, 0);
		invs.assign(n, 0);
		pre_hashs.assign(n, 0);
		pows[0] = 1;
		invs[0] = 1;
		pre_hashs[0] = int(s[0]) * pows[0] % mod;

		for (int i = 1; i < n; ++i)
		{
			pows[i] = pows[i - 1] * base % mod;
			invs[i] = invs[i - 1] * inv_base % mod;
			pre_hashs[i] = (pre_hashs[i - 1] + int(s[i]) * pows[i] % mod) % mod;
		}
	}

	int64_t getHash(int l, int r)
	{
		if (l == 0)
			return pre_hashs[r];
		return (pre_hashs[r] - pre_hashs[l - 1] + mod) % mod * invs[l] % mod;
	}

	bool equals(int l1, int r1, int l2, int r2)
	{
		if (r1 - l1 != r2 - l2)
			return false;
		if (l1 > l2)
		{
			swap(l1, l2);
			swap(r1, r2);
		}

		int64_t h1 = pre_hashs[r1];
		int64_t h2 = pre_hashs[r2];
		if (l1 > 0)
			h1 = (h1 - pre_hashs[l1 - 1] + mod) % mod;
		if (l2 > 0)
			h2 = (h2 - pre_hashs[l2 - 1] + mod) % mod;

		int64_t dif = pows[l2 - l1];
		return h1 * dif % mod == h2;
	}
};
```

