> 本笔记中假定字符集均为小写字母。

# Trie 树

**Trie** 树是一种树形数据结构，通过提取字符串集的公共前缀，用一棵树结构当作**字典**来共享存储字符串集合。

从根节点到任意一个节点路径上的字符序列代表一个前缀，每个节点通过若干条边表示存在的后续字符序列，另有一个 $count\_end$ 变量记录有多少个字符串在该节点处结束，以及一个 $count\_pass$ 变量记录有多少个字符串经过该节点。

## 结构实现

每个节点的成员信息

```c++
struct Node
{
	array<Node*, 26> nexts{};	// 指向子节点的所有边，默认指向空
	int count_end{0};		// 在当前节点处结束的字符串个数
	int count_pass{0};		// 经过当前节点处的字符串个数
};

Node* root{nullptr};	// 根节点
```

初始化

```c++
Trie()
{
	root = new Node;	// 初始化根节点;
}
```

## 插入字符串

插入字符串时需要遍历字符串的所有字符，从 Trie 树的根节点开始，逐层次拓展树上的节点并为每个节点的经过标记加一，最后在字符串结尾时，在对应的树上节点处加上一个结束标记。

```c++
void insert(const string &s)
{
	Node* node = root;
	++root->count_pass;
	for (const char c : s)
	{
		int index = c - 'a';
		if (node->nexts[index] == nullptr)	// 没有边时建立新边
			node->nexts[index] = new Node;
		node = node->nexts[index];	// 逐层遍历树上节点
		++node->count_pass;	// 为每个节点的经过标记 +1
	}
	++node->count_end;	// 添加结束标记
}
```

## 检索字符串

检索某个字符串在 Trie 树上的出现次数，同样便利字符串的所有字符，从树的根节点开始，逐层次确认是否存在对应的边，如果某条对应的边不存在，则说明不存在该字符串。最后返回字符串结尾处对应的树上节点处的结束标记个数，即为该字符串的出现次数。

```c++
int count(const string &s)
{
	Node* node = root;
	for (const char c : s)
	{
		int index = c - 'a';
		if (node->count_pass == 0 || node->nexts[index] == nullptr)	// 不存在该字符串
			return 0;
		node = node->nexts[index];
	}
	return node->count_end;
}
```

## 检索前缀串

检查以某个字符串为前缀的字符串总数，只需找到该字符串结尾对应的树上节点，返回该节点的 $count\_pass$ 变量即可。若要找出具体的所有字符串，则从该节点开始向下 $dfs$，每到达一个结束标记记录一个字符串即可。

```c++
int countPre(const string &s)
{
	Node* node = root;
	for (const char c : s)

	{
		int index = c - 'a';
		if (node->count_pass == 0 || node->nexts[index] == nullptr)
			return 0;
		node = node->nexts[index];
	}
	return node->count_pass;
	//	以下为统计具体字符串，替换上一行的 return 语句，函数返回值更改为 vector<string>
	// vector<string> ans;
	// string str = s;
	// auto dfs = [&ans, &str](auto &&dfs, Node* n) -> void
	// {
	//	 if (n->count_end > 0)
	//		 ans.push_back(str);
	//	 for (int index = 0; index < 26; ++index)
	//	 {
	//		 if (n->nexts[index] == nullptr || n->nexts[index]->count_pass == 0)
	//			 continue;
	//		 str += static_cast<char>('a' + index);
	//		 dfs(dfs, n->nexts[index]);
	//		 str.pop_back();
	//	 }
	// };
	// dfs(dfs, node);
	// return ans;
}
```

## 删除字符串

从 Trie 树上删除字符串时，先找到对应字符串结尾处对应的书上节点（不存在直接结束），将该节点处的结束标记减去 $1$，然后从该节点一路向上删除节点，知道遇到某个节点的终止标记数量不为 $0$ 或者到达根节点。

```c++
void erase(const string &s)
{
	if (count(s) == 0)	// 首先保证字符串存在
		return;
	Node* node = root;
	--root->count_pass;
	for (const char c : s)
	{
		int index = c - 'a';
		node = node->nexts[index];
		--node->count_pass;
	}
	--node->count_end;
	// 此处仅删除标记而未实际删除节点，便于节点的重复利用，避免重复 new 和 delete 的时间开销
}
```

---

# 模板

## 指针版

```c++
class Trie
{
private:
	struct Node
	{
		array<Node*, 26> nexts{};	// 指向子节点的所有边，默认指向空
		int count_end{0};		// 在当前节点处结束的字符串个数
		int count_pass{0};		// 经过当前节点处的字符串个数
	};

	Node* root{nullptr};	// 根节点

public:
	Trie()
	{
		root = new Node;
	}
	~Trie()
	{
		auto clear = [](auto &&clear, const Node* node) -> void
		{
			for (const Node* next : node->nexts)
			{
				if (next != nullptr)
					clear(clear, next);
			}
			delete node;
		};
		clear(clear, root);
	}

public:
	void insert(const string &s)
	{
		Node* node = root;
		++root->count_pass;
		for (const char c : s)
		{
			int index = c - 'a';
			if (node->nexts[index] == nullptr)
				node->nexts[index] = new Node;
			node = node->nexts[index];
			++node->count_pass;
		}
		++node->count_end;
	}

	int count(const string &s)
	{
		Node* node = root;
		for (const char c : s)
		{
			int index = c - 'a';
			if (node->nexts[index] == nullptr || node->nexts[index]->count_pass == 0)
				return 0;
			node = node->nexts[index];
		}
		return node->count_end;
	}

	int countPre(const string &s)
	{
		Node* node = root;
		for (const char c : s)
		{
			int index = c - 'a';
			if (node->nexts[index] == nullptr || node->nexts[index]->count_pass == 0)
				return 0;
			node = node->nexts[index];
		}
		return node->count_pass;
		//	以下为统计具体字符串，替换上一行的 return 语句，函数返回值更改为 vector<string>，另需将查找失败的 return 0; 改为 return {};
		// vector<string> ans;
		// string str = s;
		// auto dfs = [&ans, &str](auto &&dfs, Node* n) -> void
		// {
		//	 if (n->count_end > 0)
		//		 ans.push_back(str);
		//	 for (int index = 0; index < 26; ++index)
		//	 {
		//		 if (n->nexts[index] == nullptr || n->nexts[index]->count_pass == 0)
		//			 continue;
		//		 str += static_cast<char>('a' + index);
		//		 dfs(dfs, n->nexts[index]);
		//		 str.pop_back();
		//	 }
		// };
		// dfs(dfs, node);
		// return ans;
	}

	void erase(const string &s)
	{
		if (count(s) == 0)
			return;
		Node* node = root;
		--root->count_pass;
		// 此代码仅删除标记而未实际删除节点，便于节点的重复利用，避免重复 new 和 delete 的时间开销
		for (const char c : s)
		{
			int index = c - 'a';
			node = node->nexts[index];
			--node->count_pass;
		}
		--node->count_end;
		// 此代码实际删除节点，减少无用节点的内存占用
		// vector<Node*> nodes{root};
		// vector<int> indexs{};
		// for (const char c : s)
		// {
		//	 int index = c - 'a';
		//	 node = node->nexts[index];
		//	 --node->count_pass;
		//	 nodes.push_back(node);
		//	 indexs.push_back(index);
		// }
		// --node->count_end;
		// for (int i = nodes.size() - 1; i > 0; --i)
		// {
		//	 Node* up = nodes[i-1];
		//	 Node* now = nodes[i];
		//	 if (now->count_pass != 0 || now->count_end != 0)
		//		 break;
		//	 up->nexts[indexs[i-1]] = nullptr;
		//	 delete now;
		// }
	}
};
```

## 数组版

```c++
class Trie
{
private:
	struct Node
	{
		array<int, 26> nexts{};
		int count_end{0};
		int count_pass{0};
	};
	vector<Node> tree;	// 用动态数组模拟指针节点

public:
	Trie() : tree(1)	// 0 号节点作为根节点
	{}

public:
	void insert(const string &s)
	{
		int node = 0;
		++tree[0].count_pass;
		for (const char c : s)
		{
			int index = c - 'a';
			if (tree[node].nexts[index] == 0)
			{
				tree[node].nexts[index] = tree.size();	// 在数组末尾添加新节点
				tree.emplace_back();
			}
			node = tree[node].nexts[index];
			++tree[node].count_pass;
		}
		++tree[node].count_end;
	}

	int count(const string &s)
	{
		int node = 0;
		for (const char c : s)
		{
			int index = c - 'a';
			if (tree[node].nexts[index] == 0 || tree[tree[node].nexts[index]].count_pass == 0)
				return 0;
			node = tree[node].nexts[index];
		}
		return tree[node].count_end;
	}

	int countPre(const string &s)
	{
		int node = 0;
		for (const char c : s)
		{
			int index = c - 'a';
			if (tree[node].nexts[index] == 0 || tree[tree[node].nexts[index]].count_pass == 0)
				return 0;
			node = tree[node].nexts[index];
		}
		return tree[node].count_pass;
		// 以下为统计具体字符串，替换上一行的 return 语句，函数返回值更改为 vector<string>，另需将查找失败的 return 0; 改为 return {};
		// vector<string> ans;
		// string str = s;
		// auto dfs = [&ans, &str](auto &&dfs, int n) -> void
		// {
		//	 if (tree[n].count_end > 0)
		//		 ans.push_back(str);
		//	 for (int index = 0; index < 26; ++index)
		//	 {
		//		 if (tree[n].nexts[index] == 0 || tree[tree[n].nexts[index]].count_pass == 0)
		//			 continue;
		//		 str += static_cast<char>('a' + index);
		//		 dfs(dfs, tree[n].nexts[index]);
		//		 str.pop_back();
		//	 }
		// };
		// dfs(dfs, node);
		// return ans;
	}

	void erase(const string &s)
	{
		if (count(s) == 0)
			return;
		int node = 0;
		--tree[0].count_pass;
		// 数组版实现难以实际删除节点，仅删除字符串标记
		for (const char c : s)
		{
			int index = c - 'a';
			node = tree[node].nexts[index];
			--tree[node].count_pass;
		}
		--tree[node].count_end;
	}
};
```

---

# 例题

[洛谷T565718 Trie字符串统计](https://www.luogu.com.cn/problem/T565718)

