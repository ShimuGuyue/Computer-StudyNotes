> 本笔记中假定字符集均为小写字母。

# AC 自动机

AC 自动机是基于 **Trie**，将 **KMP** 的 **border** 概念推广到多模式串上，用于解决**单个文本串**和**多个模式串**之间的匹配问题。

ACAM 是一种**离线数据结构**，不支持添加新的字符串。如果字典串被修改，需要重新构建数据结构。

## 构建字典树

详见 [Trie树](./Trie树.md)。

## 构建失配链

类似于单字典串 KMP 算法使用的 `next` 数组辅助匹配失败时的跳转，根据 Trie 上的节点连接情况构建失配链，辅助多字典串匹配失败时的跳转。

当 ACAM 匹配时，如果某个节点 `node` 没有字符 `c` 的子节点，则匹配的节点要沿着 fail 链回退，直到找到一个能走的 `c` 节点或者回到根节点。2

> **border 推广：**
>
> 对于**两个串 $S$ 和 $T$**，相等的 $p$ 长度 $S$ 的后缀和 $T$ 的前缀称为一个 border。
>
> 对于**一个串 $S$ 和一个字典 $D$**，相等的 $p$ 长度的 $S$ 的后缀和任意一个字典串 $T$ 的前缀称为一个 border。 
>
> **失配指针：**
>
> 对于 Trie 中的每一个节点（即某个字典串的前缀），它与 Trie 中所有串的最大 border 对应的节点即为该节点失配指针指向的位置。

类似于 KMP 求 border，任意节点的 border 长度减一一定是父节点的 border，因此可以通过遍历父节点的失配指针链来求解当前节点的失配指针。

由于构建过程需要遍历父节点的失配指针链，而 Trie 上的失配指针链会交错在多个字典串中，因此 fail 链必须逐层构建，保证父节点失配指针链上的 `fail` 指针已经确定。

```c++
void buildFail()
{
	queue<Node*> q;
	// 由于根节点及其直接子节点的 border 均为 0，他们的 fail 指针均指向根节点
	root->fail = root;
	for (int index = 0; index < 26; ++index)
	{
		const Node* next = root->nexts[index];
		if (next == nullptr)
			continue;
		// 由于 Trie 树未记录父节点，因此在父节点处处理子节点的 fail 指针
		next->fail = root;
		q.push(next);
	}
	// 逐层建立各节点的 fail 指针
	while (!q.empty())
	{
		Node* node = q.front();
		q.pop();
		for (int index = 0; index < 26; ++index)
		{
			Node* next = node->nexts[index];
			if (next == nullptr)
				continue;
			// 不断回退确定 fail 指针指向
			Node* n = node->fail;
			while (n->nexts[index] == nullptr && n != root)
			{
				n = n->fail;
			}
			nexts->fail = n->nexts[index] == nullptr ? n : n->nexts[index];
			// 当一个串匹配成功，其子串也一定匹配成功
			nexts->count_end += nexts->fail->count_end;
			q.push(nexts);
		}
	}
};
```

## 多模式匹配

进行多模式匹配时，每当匹配失败，则当前节点跳转到对应的 `fail` 指针处继续匹配，如果某个位置是字符串的结束位置，则累加匹配成功的字典串个数。

```c++
int match(string& text)
{
	int ans = 0;
	Node* node = root;
	for (char c : text)
	{
		int index = c - 'a';
		// 匹配失败时延 fail 链跳转指针
		while (node->nexts[index] == nullptr && node != root)
		{
			node = node->fail;
		}
		if (node->nexts[index] != nullptr)
			node = node->nexts[index];
		// 累加匹配成功字符串个数
		ans += node->count_end;
	}
	return ans;
}
```

若需要统计每个字符串各自的出现次数，则需要记录下每个节点处结束的字典串 id。每次匹配到一个节点，就沿着 fail 链不断统计所有匹配成功的字符串。

```c++
vector<int> match(const string &text)
{
	vector<int> ans(dictionary.size());
	Node* node = root;
	for (char c : text)
	{
		int index = c - 'a';
		while (node->nexts[index] == nullptr && node != root)
		{
			node = node->fail;
		}
		if (node->nexts[index] != nullptr)
			node = node->nexts[index];
		// 延 fail 链回退，标记所有匹配成功的模式串
		for (Node* n = node; n != root; n = n->fail)
		{
			for (int id : n->end_ids)
			{
				++ans[id];
			}
		}
	}
	return ans;
}
```

## Trie 图

由于匹配失败时指针需要延 fail 链不断回退，会有比较大的时间消耗。为了提升时间效率，可以预处理每个节点匹配失败时 `fail` 指针的指向，以便跳转时**一步到位**。

由于朴素 ACAM 在匹配时要判断每个节点的 `nexts[index]` 是否为空，不为空时直接跳转到该节点的 `nexts[index]` 处，为空时则跳转到该节点的 `fail->nexts[index]` 处。因此我们可以将所有为空的边直接连接到该点的 `fail->nexts[index]` 处（逐层构建保证构建时一步到位），后续跳转时无需判断直接跳转即可。

由于该操作将树上节点连接子树的边连接到了层次更浅的节点，此时 Trie 树有一颗树变成了一个有向图，即 **Trie 图**。

```c++
void buildFail()
{
	queue<Node*> q;
	root->fail = root;
	for (int index = 0; index < 26; ++index)
	{
		Node* next = root->nexts[index];
		if (next != nullptr)
		{
			next->fail = root;
			q.push(next);
		}
		else
		{	// 构建 Trie 图
			root->nexts[index] = root;
		}
	}
	while (!q.empty())
	{
		Node* node = q.front();
		q.pop();
		for (int index = 0; index < 26; ++index)
		{
			Node* next = node->nexts[index];
			if (next != nullptr)
			{
				next->fail = node->fail->nexts[index];
				next->count_end += next->fail->count_end;
				q.push(next);
			}
			else
			{	// 构建 Trie 图
				node->nexts[index] = node->fail->nexts[index];
			}
		}
	}
};
```

```c++
int match(string &text)
{
	int ans = 0;
	Node* node = root;
	for (char c : text)
	{
		int index = c - 'a';
		// 根据 Trie 图直接跳转，无需分类讨论
		node = node->nexts[index];
		ans += node->count_end;
	}
	return ans;
}
```

## 拓扑优化

匹配每个字符串串各自的出现次数的情形下，每走到一个节点都要延 fail 链回退，对所有的字符串结尾节点进行标记，需要大量的时间开销。

而已知当一个串出现时，它的 border 也一定出现，因此可以用 Trie 树上所有的 fail 链建一个有向图，有每个节点指向其 fail 指针对应的节点。然后在匹配时只记录当前节点被经过的次数，等到匹配结束后，再根据建好的图进行**拓扑排序**，将子节点的出现次数累加到其 `fail` 指针指向的节点即可。

```c++
void buildFail()
{
	/* 以上省略 fail 链构建代码 */
	
	/* 根据 fail 链建图 */
	// 预先用一个数组保存所有节点 vector<Node*> tree;
	// 使用每个节点的 fail 指针表示拓扑图上的孩子节点即可
	count_topofathers.resize(tree.size());	// 使用一个全局数组记录每个节点在拓扑图上的父节点数量
	for (Node* &node : tree)
	{
		++count_topofathers[node->fail->id];
	}
}
```

```c++
vector<int> matchEach(string &text)
{
	vector<int> count_passs(tree.size());	// 统计各节点匹配次数

	Node* node = root;
	for (char c : text)
	{
		int index = c - 'a';
		node = node->nexts[index];
		++count_passs[node->id];	// Node 额外记录每个节点的树上编号
	}
	
	vector<int> counts(dictionary.size());
	vector<int> count_fathers = count_topofathers;	// 操作副本，原图可反复使用
	/* 拓扑排序累计匹配次数 */
	queue<Node*> q;
	for (Node* &node : tree)
	{
		if (count_fathers[node->id] == 0)
			q.push(node);
	}
	while (!q.empty())
	{
		Node* n = q.front();
		q.pop();
		for (int id : n->end_ids)
		{	// 记录各字符串匹配次数
			counts[id] = count_passs[n->id];
		}
		count_passs[n->fail->id] += count_passs[n->id];
		if (--count_topofathers[n->fail->id] == 0)
			q.push(n->fail);
	}
	return counts;	// 返回所有字符串对应的出现次数
}
```



---

# 模板

## 指针版

```c++
class ACAM
{
private:
	struct Node
	{
		int treeid{0};			// 节点对应的树上编号
		vector<int> endids{};	// 节点处结束的字符串编号
		Node* fail{nullptr};		// 节点的 fail 指针指向
		array<Node*, 26> nexts{};	// 节点在 Trie 上的子节点
	};

	Node* root{nullptr};						// Trie 根节点
	vector<Node*> tree{};						// 节点集合
	vector<int> topo_countfathers{};		// 拓扑图各节点父节点个数
	vector<pair<string, int>> dictionary{};	// 所有字典串及其编号集合

public:
	ACAM() : root{new Node}, tree{root}
	{}

	~ACAM()
	{
		for (Node* &node : tree)
		{
			delete node;
		}
	}

public:
	void insert(string &s)
	{
		Node* node = root;
		for (char c : s)
		{
			int index = c - 'a';
			if (node->nexts[index] == nullptr)
			{
				node->nexts[index] = new Node;
				node->nexts[index]->treeid = tree.size();
				tree.push_back(node->nexts[index]);
			}
			node = node->nexts[index];
		}
		int stringid = dictionary.size();
		node->endids.push_back(stringid);
		dictionary.push_back(pair<string, int>{s, stringid});
	}

	void build()
	{
		/* fail */
		root->fail = root;
		queue<Node*> q;
		for (int index = 0; index < 26; ++index)
		{
			Node* next = root->nexts[index];
			if (next != nullptr)
			{
				next->fail = root;
				q.push(next);
			}
			else
			{
				root->nexts[index] = root;
			}
		}
		while (!q.empty())
		{
			Node* node = q.front();
			q.pop();
			for (int index = 0; index < 26; ++index)
			{
				Node* next = node->nexts[index];
				if (next != nullptr)
				{
					next->fail = node->fail->nexts[index];
					q.push(next);
				}
				else
				{
					node->nexts[index] = node->fail->nexts[index];
				}
			}
		}

		/* topo */
		topo_countfathers.assign(tree.size(), 0);
		for (Node* &node : tree)
		{
			++topo_countfathers[node->fail->treeid];
		}
	}

	vector<int> matchEach(string &text)
	{
		vector<int> count_matchs(tree.size());	// 树上节点匹配次数
		Node* node = root;
		for (char c : text)
		{
			int index = c - 'a';
			node = node->nexts[index];
			++count_matchs[node->treeid];
		}

		vector<int> counts(dictionary.size());	// 各字符串匹配次数
		vector<int> count_fathers = topo_countfathers;
		queue<Node*> q;
		for (Node* &n : tree)
		{
			if (count_fathers[n->treeid] == 0)
				q.push(n);
		}
		while (!q.empty())
		{
			Node* n = q.front();
			q.pop();
			for (int id : n->endids)
			{
				counts[id] = count_matchs[n->treeid];
			}
			count_matchs[n->fail->treeid] += count_matchs[n->treeid];
			if (--count_fathers[n->fail->treeid] == 0)
				q.push(n->fail);
		}
		return counts;
	}

	int matchAll(string &text)
	{
		vector<int> counts = matchEach(text);
		return accumulate(counts.begin(), counts.end(), 0);
	}
};
```

## 数组版

```c++
class ACAM
{
private:
	struct Node
	{
		vector<int> endids{};	// 节点处结束的字符串编号
		int fail{0};		// 节点的 fail 指针指向
		array<int, 26> nexts{};	// 节点在 Trie 上的子节点
	};

	vector<Node> tree{};						// 节点集合
	vector<int> topo_countfathers{};		// 拓扑图各节点父节点个数
	vector<pair<string, int>> dictionary{};	// 所有字典串及其编号集合

public:
	ACAM() : tree{1}
	{}

public:
	void insert(string &s)
	{
		int node = 0;
		for (char c : s)
		{
			int index = c - 'a';
			if (tree[node].nexts[index] == 0)
			{
				tree[node].nexts[index] = tree.size();
				tree.emplace_back();
			}
			node = tree[node].nexts[index];
		}
		int stringid = dictionary.size();
		tree[node].endids.push_back(stringid);
		dictionary.push_back(pair<string, int>{s, stringid});
	}

	void build()
	{
		/* fail */
		queue<int> q;
		for (int index = 0; index < 26; ++index)
		{
			int next = tree[0].nexts[index];
			if (next != 0)
				q.push(next);
		}
		while (!q.empty())
		{
			int node = q.front();
			q.pop();
			for (int index = 0; index < 26; ++index)
			{
				int next = tree[node].nexts[index];
				if (next != 0)
				{
					tree[next].fail = tree[tree[node].fail].nexts[index];
					q.push(next);
				}
				else
				{
					tree[node].nexts[index] = tree[tree[node].fail].nexts[index];
				}
			}
		}

		/* topo */
		topo_countfathers.assign(tree.size(), 0);
		for (Node &node : tree)
		{
			++topo_countfathers[node.fail];
		}
	}

	vector<int> matchEach(string &text)
	{
		vector<int> count_matchs(tree.size());	// 树上节点匹配次数
		int node = 0;
		for (char c : text)
		{
			int index = c - 'a';
			node = tree[node].nexts[index];
			++count_matchs[node];
		}

		vector<int> counts(dictionary.size());	// 各字符串匹配次数
		vector<int> count_fathers = topo_countfathers;
		queue<int> q;
		for (int id = 0; id < tree.size(); ++id)
		{
			if (count_fathers[id] == 0)
				q.push(id);
		}
		while (!q.empty())
		{
			int n = q.front();
			q.pop();
			for (int id : tree[n].endids)
			{
				counts[id] = count_matchs[n];
			}
			count_matchs[tree[n].fail] += count_matchs[n];
			if (--count_fathers[tree[n].fail] == 0)
				q.push(tree[n].fail);
		}
		return counts;
	}

	int matchAll(string &text)
	{
		vector<int> counts = matchEach(text);
		return accumulate(counts.begin(), counts.end(), 0);
	}
};
```

---

# 例题

[P3808 AC 自动机（简单版） - 洛谷](https://www.luogu.com.cn/problem/P3808)（模板）

[P3796 AC 自动机（简单版 II） - 洛谷](https://www.luogu.com.cn/problem/P3796)（模板 - Trie 图）

[P5357 【模板】AC 自动机 - 洛谷](https://www.luogu.com.cn/problem/P5357)（模板 - 拓扑优化）

