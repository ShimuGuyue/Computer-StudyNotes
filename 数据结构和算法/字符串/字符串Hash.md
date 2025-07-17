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

> 笔记和代码中字符串下标均从 $1$ 开始。

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
vector<uint64_t> bulidPreHashs(string &s, uint64_t base, uint64_t mod)
{
	const size_t n = s.length();
	vector<uint64_t> pre_hashs(n);
	for (size_t i = 1; i < n; ++i)
	{
		pre_hashs[i] = (pre_hashs[i - 1] + s[i] * pow(base, i) % mod) % mod;
	}
	return pre_hashs;
}
```

```c++
uint64_t getHash(string &s, size_t l, size_t r, uint64_t base, uint64_t mod, vector<uint64_t> &pre_hashs)
{
	return (pre_hashs[r] - pre_hashs[l - 1] + mod) % mod * inv(pow(base, l - 1), mod) % mod;
}
```

