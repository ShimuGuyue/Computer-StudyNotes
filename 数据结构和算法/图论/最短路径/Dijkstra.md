**Dijkstra** 算法用于求解**无负权图**的**单源最短路**。

Dijkstra 算法基于**贪心策略**和**松弛操作**。

* 松弛：

  对于**已确定**最短路径的所有点 $u$，遍历其相邻的所有点 $v$，尝试更新其最短路径。

* 贪心：

  对于**未确定**最短路径的点所有中，选择当前距离起点最近的点 $v$，确定他的最短路径。

> 本笔记中假定图中节点编号从 $0$ 开始。

---

## 一、朴素版 Dijkstra

在朴素版 Dijkstra 中，每次**遍历所有点**，找到当前最短路径最短的点。

图的存储方式使用**邻接矩阵**，该方法更适用于**稠密图**。

时间复杂度为 $O(n ^ 2)$。

```c++
vector<uint32_t> dijkstra(const vector<vector<uint32_t>> &grid, const size_t start)
{
	size_t n = grid.size();
	vector<uint32_t> min_lens(n, INT_MAX);
	vector<bool> visited(n, false);
	min_lens[start] = 0;
	visited[start] = true;

	// 由于起点最短路径已经确定，实际上外层循环可以少跑一次
	for (size_t i = 0; i < n; ++i)
	{
		uint32_t min = INT_MAX;
		size_t node = -1;
		for (size_t v = 0; v < n; ++v)
		{
			// 已经确定最短路径的节点不会再做更新
			if (visited[v])
				continue;
			if (min_lens[v] < min)
			{
				min = min_lens[v];
				node = v;
			}
		}
		// 如果所有可达点的最短路已全部找到，提前退出循环
		if (node == -1)
			break;
		// 将找到的节点标记为已找到最短路径
		visited[node] = true;
		// 根据新找到节点的最短路径更新其相邻点的最短路径
		for (size_t v = 0; v < n; ++v)
		{
			if (visited[v] || grid[node][v] == INT_MAX)
				continue;
			min_lens[v] = std::min(min_lens[v], min_lens[node] + grid[node][v]);
		}
	}
	return min_lens;
}
```

---

## 二、堆优化版 Dijkstra

在堆优化版的 Dijkstra 中，用一个小根堆维护所有未确定最短路径的点的当前最短路径，直接取堆顶找到所求的点。

图的存储方式使用**邻接表**，该方法更适用于**稀疏图**。

时间复杂度为 $O((n + m)\log m)$

```c++
vector<uint32_t> dijkstra(const vector<vector<pair<uint32_t, size_t>>> &grid, const size_t start)
{
	size_t n = grid.size();
	vector<uint32_t> min_lens(n, INT_MAX);
	vector<bool> visited(n, false);
	min_lens[start] = 0;

	// 堆中 pair 的 first 和 second 分别存储 len 和 node，在小根堆中按 len 排序
	priority_queue<pair<uint32_t, size_t>, vector<pair<uint32_t, size_t>>, greater<pair<uint32_t, size_t>>> h;
	h.push({0, start});
	while (!h.empty())
	{
		auto [len, u] = h.top();
		h.pop();
		// 如果当前点已经被确定了最短路径，跳过
		if (visited[u])
			continue;
		visited[u] = true;
		// grid 中每个 pair 的 first 和 second 分别存储 node 和 len
		for (auto [v, w] : grid[u])
		{
			if (visited[v])
				continue;
			if (len + w < min_lens[v])
			{
				min_lens[v] = len + w;
				h.push({min_lens[v], v});
			}
		}
	}
	return min_lens;
}
```
