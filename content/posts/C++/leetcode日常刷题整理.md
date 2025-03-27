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