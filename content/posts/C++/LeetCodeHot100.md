---
title: "LeetCode热题100"
date: 2025-03-12
tags: [C++]
description: "LeetCode热题100整理"
showDate: true
math: true
chordsheet: true
---



# LeetCodeHot100

### 42. 接雨水

*方法一*：

**前后缀分解**

计算前缀最大值数组、后缀最大值数组。再遍历一遍，取该处的前缀最大值和后缀最大值的最小值，再减去该处高度，就是接到的容量。

```c++
int trap(vector<int>& height) {
    int n = height.size();
    vector<int> pre(n), back(n);
    int maxh = 0;
    for(int i = 0; i < n; i ++) {
        maxh = max(maxh, height[i]);
        pre[i] = maxh;
    }
    maxh = 0;
    for(int i = n - 1; i >= 0; i --) {
        maxh = max(maxh, height[i]);
        back[i] = maxh; 

    } 
    int res = 0;
    for(int i = 0; i < n; i ++) {
        res += min(pre[i], back[i]) - height[i];
    }
    return res;
}
```

*方法二*：

空间优化O(1) **双指针**

定义左右指针和左最大值和右最大值，当左指针小于右指针的时候，不断更新左右最大值。

- 如果左最大值 < 右最大值，则左指针移动，并且答案加上左最大值 - 当前高度。
- 如果右最大值 < 左最大值，则右指针移动，并且答案加上右最大值 - 当前高度。

```c++
int trap(vector<int>& height) {
    int n = height.size();
    int l = 0, r = n - 1;
    int premax = 0, backmax = 0;
    int res = 0;
    while(l < r) {
        premax = max(premax, height[l]);
        backmax = max(backmax, height[r]);
        if(premax < backmax) {
            res += premax - height[l];
            l ++;
        }
        else {
            res += backmax - height[r];
            r --;
        }
    }
    return res;
}
```



### 438. 找到字符串中所有字母异位词

**滑动窗口**

用vector代替哈希表，因为vector可以直接比较两个vector是否相等。

初始化2个vector分别存储当前遍历到的字符串 和 目标字符串。

如果超出长度，则 l++；

```c++
vector<int> findAnagrams(string s, string p) {
    vector<int> mp(26, 0), smp(26, 0);
    vector<int> res;
    for(int i = 0; i < p.length(); i ++) mp[p[i] - 'a'] ++;
    for(int l = 0, r = 0; r < s.length(); r ++) {
        smp[s[r] - 'a'] ++;
        if(r - l + 1 > p.length()) {
            smp[s[l] - 'a'] --;
            l ++;
        }
        if(smp == mp) res.push_back(l);
    }
    return res;
}
```



### 560. 和为k的子数组

**前缀和、哈希表**

记录在哈希表中记录前缀和，每次查找sum - k存不存在，如果存在则说明有这样一组连续的数和为k。

```c++
int subarraySum(vector<int>& nums, int k) {
    unordered_map<int, int> prefixCount;
    prefixCount[0] = 1;
    int sum = 0, res = 0;
    for(int num : nums) {
        sum += num;
        if(prefixCount.find(sum - k) != prefixCount.end()) {
            res += prefixCount[sum - k];
        }
        prefixCount[sum] ++;
    }
    return res;
}
```



### 239. 滑动窗口最大值

**单调队列**

举例子2,1,4,2,3,2这一系列数，窗口大小为3。

**两种情况：**

1. 当遇到4时，前面的2 1将永远不会成为最大值，因为4比他们更大。—— 第一种情况，遇到更大的数，队尾不断弹出。

   ```c++
   while(!q.empty() && nums[i] > nums[q.back()]) {
       q.pop_back();
   }
   ```

2. 继续走，4的后面遇到了2、3、2。发现4已经超出窗口了，所以4不能是当前窗口的最大值，也被淘汰了。——第二种情况，队头滑出窗口要弹出。

   ```c++
   if(!q.empty() && q.front() < i - k + 1) q.pop_front();
   ```

```c++
vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    deque<int> q;
    int n = nums.size();
    vector<int> res;
    for(int i = 0; i < n; i ++) {
        while(!q.empty() && nums[i] > nums[q.back()]) {
            q.pop_back();
        }
        q.push_back(i);
        if(!q.empty() && q.front() < i - k + 1) q.pop_front();
        // 当 i >= k - 1时才记录答案，就是在没达到窗口大小之前不记录。
        if(i >= k - 1) res.push_back(nums[q.front()]);
    }
    return res;
}
```



### LRU缓存

**双向链表和哈希表**

双向链表存储的是{key, value}，哈希表存储的是双向链表的迭代器。

```c++
#include<iostream>
#include<list>
#include<unordered_map>
using namespace std;

class LRUCache {
private:
	int capacity;
	list<pair<int, int>> cache;
	unordered_map<int, list<pair<int, int>>::iterator> hashTable;

public:
	LRUCache(int cap) : capacity(cap) { }
	int get(int key) {
		if (hashTable.find(key) == hashTable.end()) return -1;
		cache.splice(cache.begin(), cache, hashTable[key]); // 移动到头部
		return hashTable[key]->second;
	}

	void put(int key, int value) {
		if (hashTable.find(key) != hashTable.end()) {
			hashTable[key]->second = value;
			cache.splice(cache.begin(), cache, hashTable[key]); //如果在缓存中，则移动到头部
		}
		else { // 如果不在缓存中
			if (cache.size() >= capacity) { // 并且容量满了
				auto last = cache.back();
				hashTable.erase(last.first); // 从哈希表中删除
				cache.pop_back(); // 把最不常用的队尾的pop
			}
			cache.push_front({ key, value });
			hashTable[key] = cache.begin();
		}
	}
};

int main() {
	LRUCache lru(2);
	lru.put(1, 10);
	lru.put(2, 20);
	cout << lru.get(1) << endl;
	lru.put(3, 30);
	cout << lru.get(2) << endl;
	return 0;
}
```

### 76. 最小覆盖字串

```c++
class Solution {
public:
    string minWindow(string s, string t) {
        if(s.empty() || t.empty()) return "";
        unordered_map<char, int> need, window;
        for(char c : t) need[c] ++;
        int left = 0, right = 0;
        int valid = 0;
        int start = 0, minLen = INT_MAX;
        while(right < s.size()) {
            char c = s[right];
            right ++;
            if(need.count(c)) {
                window[c] ++;
                if(window[c] == need[c]) valid ++;
            }
            while(valid == need.size()) {
                if(right - left < minLen) {
                    start = left;
                    minLen = right - left;
                }
                char d = s[left];
                left ++;
                if(need.count(d)) {
                    if(window[d] == need[d]) {
                        valid --;
                    }
                    window[d] --;
                }
            }
        }
        return minLen == INT_MAX ? "" : s.substr(start, minLen);
    }
};
```



## 二叉树

### 101. 对称二叉树

*递归*：将root的左右子树传递给check函数。check函数递归左右子树的左右子树。

```c++
class Solution {
public:
    bool check(TreeNode* p, TreeNode* q) {
        if(!p && !q) return true;
        if(!p || !q) return false;
        return p->val == q->val && check(p->left, q->right) && check(p->right, q->left); 
    }
    bool isSymmetric(TreeNode* root) {
        return check(root->left, root->right);
    }
};
```

### 543. 二叉树的直径

找左子树中最大的深度和右子树中最大的深度，加在一起 -1就是最长的路径。

```c++
class Solution {
public:
    int ans = 0;
    int depth(TreeNode* root) {
        if(root == nullptr) return 0;
        int left = depth(root->left);
        int right = depth(root->right);
        ans = max(left + right + 1, ans);
        return max(left, right) + 1;
    }
    int diameterOfBinaryTree(TreeNode* root) {
        depth(root);
        return ans - 1;
    }
};
```

### 108. 将有序数组转换为二叉搜索树

*二分法*

```c++
class Solution {
public:
    TreeNode* helper(vector<int>& nums, int left, int right) {
        if(left > right) return nullptr;
        int mid = left + right >> 1;
        TreeNode* root = new TreeNode(nums[mid]);
        root->left = helper(nums, left, mid - 1);
        root->right = helper(nums, mid + 1, right);
        return root;
    }
    TreeNode* sortedArrayToBST(vector<int>& nums) {
        return helper(nums, 0, nums.size() - 1);
    }
};
```



## 普通数组

### 56. 合并区间

```c++
class Solution {
public:
    vector<vector<int>> merge(vector<vector<int>>& intervals) {
        if(intervals.empty()) return {};
        sort(intervals.begin(), intervals.end());
        vector<vector<int>> res;
        res.push_back(intervals[0]);
        for(int i = 1; i < intervals.size(); i ++) {
            vector<int>& last = res.back();
            if(intervals[i][0] <= last[1]) {
                last[1] = max(intervals[i][1], last[1]);
            } 
            else {
                res.push_back(intervals[i]);
            }
        }
        return res;
    }
};
```



### 41. 缺失的第一个整数

*原地哈希*：将数字想象成哈希表，把数字放在它该放在的位置，例如：3放到nums[2], 2放到nums[1]。跳过负数，和大于n的数，剩下遍历0 - n的数组，如果位置不对，则该位置应该在的数就是缺失的第一个整数。

```c++
class Solution {
public:
    int firstMissingPositive(vector<int>& nums) {
        int n = nums.size();
        for(int i = 0; i < n; i ++) {
            while(nums[i] > 0 && nums[i] <= n && nums[nums[i] - 1] != nums[i]) {
                swap(nums[i], nums[nums[i] - 1]);
            }
        }
        for(int i = 0; i < n; i ++) {
            if(nums[i] != i + 1) return i + 1;
        }
        return n + 1;
    }
};
```

