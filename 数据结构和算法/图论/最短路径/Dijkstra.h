#include <vector>
using std::vector;
#include <queue>
using std::priority_queue;
#include <utility>
using std::pair;
#include <functional>
using std::greater;
#include <climits>

/**
 * 朴素版 Dijkstra
 */
vector<int> Dijkstra1(const vector<vector<int>> &grid, const int start)
{
    int n = grid.size();
    vector<int> min_lens(n, INT_MAX);
    vector<bool> visited(n, false);
    min_lens[start] = 0;
    visited[start] = true;

    // 由于起点最短路径已经确定，实际上外层循环可以少跑一次
    for (int i = 0; i < n; ++i)
    {
        int min = INT_MAX;
        int node = -1;
        for (int v = 0; v < n; ++v)
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
        for (int v = 0; v < n; ++v)
        {
            if (visited[v] || grid[node][v] == INT_MAX)
                continue;
            min_lens[v] = std::min(min_lens[v], min_lens[node] + grid[node][v]);
        }
    }
    return min_lens;
}

/**
 * 堆优化版 Dijkstra
 */
vector<int> Dijkstra2(const vector<vector<pair<int, int>>> &grid, const int start)
{
    int n = grid.size();
    vector<int> min_lens(n, INT_MAX);
    vector<bool> visited(n, false);
    min_lens[start] = 0;

    // 堆中 pair<int, int> 的 first 和 second 分别存储 len 和 node，在小根堆中按 len 排序
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> h;
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