#include <string>
using std::string;
#include <vector>
using std::vector;

namespace next
{
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
}

namespace kmp_first
{
using next::buildNexts;

int kmp(string &s, string &t)
{
	vector<int> nexts = buildNexts(t);
	int n = s.length();
	int m = t.length();
	int i = 0, j = 0;
	while (i < n)
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
	return -1;
}
}

namespace kmp_all
{
using next::buildNexts;

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
				j = nexts[j-1];
			}
			else
			{
				++j;
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
	return indexs;
}
}
