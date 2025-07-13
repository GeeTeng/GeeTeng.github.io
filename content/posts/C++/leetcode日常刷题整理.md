---
title: "LeetCode日常刷题整理"
date: 2025-03-27
tags: [算法]
description: "LeetCode日常刷题整理笔记"
showDate: true
math: true
chordsheet: true
---



### 841. 钥匙和房间

[841. 钥匙和房间 - 力扣（LeetCode）](https://leetcode.cn/problems/keys-and-rooms/description/)

> 有 `n` 个房间，房间按从 `0` 到 `n - 1` 编号。最初，除 `0` 号房间外的其余所有房间都被锁住。你的目标是进入所有的房间。然而，你不能在没有获得钥匙的时候进入锁住的房间。
>
> 当你进入一个房间，你可能会在里面找到一套 **不同的钥匙**，每把钥匙上都有对应的房间号，即表示钥匙可以打开的房间。你可以拿上所有钥匙去解锁其他房间。
>
> 给你一个数组 `rooms` 其中 `rooms[i]` 是你进入 `i` 号房间可以获得的钥匙集合。如果能进入 **所有** 房间返回 `true`，否则返回 `false`。
>
> **示例 1：**
>
> ```
> 输入：rooms = [[1],[2],[3],[]]
> 输出：true
> 解释：
> 我们从 0 号房间开始，拿到钥匙 1。
> 之后我们去 1 号房间，拿到钥匙 2。
> 然后我们去 2 号房间，拿到钥匙 3。
> 最后我们去了 3 号房间。
> 由于我们能够进入每个房间，我们返回 true。
> ```

我用的*bfs*方法，用一个indexused数组来标记是否该房间被访问过，将访问过的房间的钥匙不断加入到**队列**中。

最后遍历indexused数组，看是否所有房间都被访问过。

```c++
class Solution {
public:
    bool canVisitAllRooms(vector<vector<int>>& rooms) {
        int n = rooms.size();
        queue<int> q;
        vector<bool> indexused(n, false);
        q.push(0);
        indexused[0] = true;
        while(!q.empty()){
            auto index = q.front();
            q.pop();
            for(int i = 0; i < rooms[index].size(); i ++) {
                int next = rooms[index][i];
                if(!indexused[next]) {
                    indexused[next] = true;
                    q.push(next);
                }
            }
        }
        for(bool visited : indexused) {
            if(!visited) return false;
        }
        return true;
    }
};
```

注意点：

不能在房间号出队时标记为true，而是应该在房间号push之前，因为有可能会发生：

处理 0，访问 1和 2，`q = [1, 2]`

处理 1，发现 2 还没有被 indexused 标记，所以又把 `2` 重复入队，`q = [2, 2]`



### 6. Z字形变换

[6. Z 字形变换 - 力扣（LeetCode）](https://leetcode.cn/problems/zigzag-conversion/description/?envType=study-plan-v2&envId=top-interview-150)

> 将一个给定字符串 `s` 根据给定的行数 `numRows` ，以从上往下、从左到右进行 Z 字形排列。
>
> 比如输入字符串为 `"PAYPALISHIRING"` 行数为 `3` 时，排列如下：
>
> ```
> P   A   H   N
> A P L S I I G
> Y   I   R
> ```
>
> 之后，你的输出需要从左往右逐行读取，产生出一个新的字符串，比如：`"PAHNAPLSIIGYIR"`。
>
> 请你实现这个将字符串进行指定行数变换的函数：
>
> ```
> string convert(string s, int numRows);
> ```
>
> **示例 1：**
>
> ```
> 输入：s = "PAYPALISHIRING", numRows = 3
> 输出："PAHNAPLSIIGYIR"
> ```

row表示当前字符要放在哪一行，最终答案是res字符串数组一行一行拼接起来的。

down表示向上走还是向下走，如果向下走row++，如果向上走row--。

如果到最顶部或者最底部要改变方向，其实相当于将二维矩阵压缩成一行。

```c++
class Solution {
public:
    string convert(string s, int numRows) {
        if(numRows == 1) return s;
        bool down = true;
        int row = 0;
        vector<string> res(numRows);
        for(int i = 0; i < s.length(); i ++) {
            res[row] += s[i];
            if(down) row ++;
            if(!down) row --;
            if(row == numRows - 1 || row == 0) down = !down;
        }
        string ans;
        for(auto r : res){
            ans += r;
        }
        return ans;
    }
};
```



无需开会的工作日

[3169. 无需开会的工作日 - 力扣（LeetCode）](https://leetcode.cn/problems/count-days-without-meetings/description/)

> 给你一个正整数 `days`，表示员工可工作的总天数（从第 1 天开始）。另给你一个二维数组 `meetings`，长度为 `n`，其中 `meetings[i] = [start_i, end_i]` 表示第 `i` 次会议的开始和结束天数（包含首尾）。
>
> 返回员工可工作且没有安排会议的天数。
>
> **注意：**会议时间可能会有重叠。
>
> **示例 1：**
>
> **输入：**days = 10, meetings = [[5,7],[1,3],[9,10]]
>
> **输出：**2
>
> **解释：**
>
> 第 4 天和第 8 天没有安排会议。

```c++
class Solution {
public:
    int countDays(int days, vector<vector<int>>& meetings) {
        sort(meetings.begin(), meetings.end());
        int start = meetings[0][0], end = meetings[0][1];
        int duration = start - 1;
        for(vector<int> meet : meetings) {
            start = meet[0];
            if(start > end) {
                duration += start - end - 1;
            }
            end = max(end, meet[1]); // ！！！
        }
        duration += days - end;
        return duration;
    }
};
```

