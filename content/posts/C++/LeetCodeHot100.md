---
title: "LeetCode热题100"
date: 2025-03-12
tags: [算法]
description: "LeetCode热题100整理"
showDate: true
math: true
chordsheet: true
---



# LeetCodeHot100

## 双指针

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



## 滑动窗口

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



## 子串

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



## 矩阵

### 73. 矩阵置零

**步骤1：检查首行和首列是否存在 0**

- 遍历第0列，如果有 `0`，则 `colFlag = true`（表示首列需要全部置零）
- 遍历第0行，如果有 `0`，则 `rowFlag = true`（表示首行需要全部置零）

**步骤2：利用第0行和第0列作为标志位**

- 遍历矩阵的非首行非首列部分
- 如果 matrix\[i][j] == 0，则设置：
  - `matrix[i][0] = 0`（标记第 `i` 行需要置零）
  - `matrix[0][j] = 0`（标记第 `j` 列需要置零）

**步骤3：根据标志位置零**

- 遍历非首行非首列部分：
  - 如果 `matrix[i][0] == 0`，将该行全部置零
  - 如果 `matrix[0][j] == 0`，将该列全部置零

**步骤4：根据 `rowFlag` 和 `colFlag` 处理首行和首列**

- 如果 `rowFlag == true`，将首行全部置零
- 如果 `colFlag == true`，将首列全部置零

```c++
void setZeroes(vector<vector<int>>& matrix) {
    int m = matrix.size();
    int n = matrix[0].size();
    bool rowflag = false, colflag = false;
    for(int i = 0; i < m; i ++) {
        if(matrix[i][0] == 0) colflag = true;
    }
    for(int j = 0; j < n; j ++) {
        if(matrix[0][j] == 0) rowflag = true;
    }
    for(int i = 1; i < m; i ++) {
        for(int j = 1; j < n; j ++) {
            if(matrix[i][j] == 0) {
                matrix[i][0] = 0;
                matrix[0][j] = 0;
            }
        }
    }
    for(int i = 1; i < m; i ++) {
        if(matrix[i][0] == 0) {
            for(int j = 1; j < n; j ++) {
                matrix[i][j] = 0;
            }
        }
    }
    for(int j = 1; j < n; j ++) {
        if(matrix[0][j] == 0) {
            for(int i = 1; i < m; i ++) {
                matrix[i][j] = 0;
            }
        }
    }
    if(rowflag) {
        for(int j = 0; j < n; j ++) matrix[0][j] = 0;
    }
    if(colflag) {
        for(int i = 0; i < m; i ++) matrix[i][0] = 0;
    }
}
```



### 48. 旋转图像（矩阵）

*用翻转代替旋转*：

1. 先水平翻转

2. 再主对角线翻转

```c++
void rotate(vector<vector<int>>& matrix) {
    int n = matrix.size();
    for(int i = 0; i < n / 2; i ++) {
        swap(matrix[i], matrix[n - i - 1]);
    }
    for(int i = 0; i < n; i ++) {
        for(int j = i + 1; j < n; j ++) {
            swap(matrix[i][j], matrix[j][i]);
        }
    }
}
```



### 240. 搜索二维矩阵Ⅱ

Z字形搜索，想象要查找的元素在左下角，如果当前元素比target要大，就向左移动，如果比target小，就向下移动。

```c++
bool searchMatrix(vector<vector<int>>& matrix, int target) {
    int m = matrix.size(), n = matrix[0].size();
    int x = 0, y = n - 1;
    while (x < m && y >= 0) {
        if(matrix[x][y] == target) {
            return true;
        }
        if(matrix[x][y] > target) {
            y --;
        }
        else {
            x ++;
        }
    }
    return false;
}
```



## 链表

### 2. 两数相加

初始化一个新的头节点res，用carry记录进位，在下次遍历中增加到sum中，不断地new一个新节点连接到res上。

```c++
ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
    ListNode* res = new ListNode(0);
    ListNode* cur = res;
    int carry = 0;
    while(l1 || l2 || carry) {
        int sum = carry;
        if(l1) {
            sum += l1->val;
            l1 = l1->next;
        }
        if(l2) {
            sum += l2->val;
            l2 = l2->next;
        }
        carry = sum / 10;
        cur->next = new ListNode(sum % 10);
        cur = cur->next;
    }
    return res->next;
}
```



### 19. 删除链表的倒数第N个结点

和倒数第k个元素的链表那道题很像，这道题只需找到第k个元素的前一个就好。

当p1多走一步时，p2就会指向第N个节点的前一个结点。

```c++
ListNode* removeNthFromEnd(ListNode* head, int n) {
    ListNode* res = new ListNode(0);
    res->next = head;
    ListNode* p1 = res, *p2 = res;
    while(n -- >= 0) {
        if(p1) p1 = p1->next;
    }
    while(p1) {
        p1 = p1->next;
        p2 = p2->next;
    }
    if(p2->next) {
        p2->next = p2->next->next;
    }
    return res->next;
}
```



### 24. 两两交换链表中的节点

*迭代法*：设置一个虚拟头节点，tmp从头节点开始，node1是tmp的下一个节点，node2是node1的下一个节点。

```c++
ListNode* swapPairs(ListNode* head) {
    ListNode* res = new ListNode(0);
    res->next = head;
    ListNode* tmp = res;
    while(tmp->next != nullptr && tmp->next->next != nullptr) {
        ListNode* node1 = tmp->next, *node2 = tmp->next->next;
        tmp->next = node2;
        node1->next = node2->next;
        node2->next = node1;
        tmp = node1;
    }
    return res->next;
}
```



### 25. K个一组反转链表



```c++
ListNode* reverseKGroup(ListNode* head, int k) {
    if(!head || k <= 1) return head;
    ListNode* tmp = head;
    int len = 0;
    while(tmp) {
        tmp = tmp->next;
        len ++;
    }
    ListNode* res = new ListNode(0);
    res->next = head;
    ListNode* pre = res;
    ListNode* cur, *next;

    while(len >= k) {
        cur = pre->next;
        next = cur->next;
        for(int i = 1; i < k; i ++) {
            cur->next = next->next;
            next->next = pre->next;
            pre->next = next;
            next = cur->next;
        }
        pre = cur;
        len -= k;
    }
    return res->next;
}
```



### 23. 合并K个升序链表

*归并排序*：和排序链表一样的道理，只不过K个需要两两合并，**分治法**。

举例子：

```
lists[0] = merge(lists[0], lists[3])
lists[1] = merge(lists[1], lists[4])
```

**`lists[2]` 没有被合并！**

```c++
class Solution {
public:
    ListNode* merge(ListNode* l1, ListNode* l2) {
        ListNode* res = new ListNode(0);
        ListNode* cur = res;
        while(l1 && l2) {
            if(l1->val < l2->val) {
                cur->next = l1;
                l1 = l1->next;
            } else {
                cur->next = l2;
                l2 = l2->next;
            }
            cur = cur->next;
        }
        if(l1) cur->next = l1;
        if(l2) cur->next = l2;
        return res->next;
    }
    ListNode* mergeKLists(vector<ListNode*>& lists) {
        if(lists.empty()) return nullptr;
        int n = lists.size();
        while(n > 1) {
            int newSize = (n + 1) / 2; // 奇数情况比如5个数 实际上是3组
            for(int i = 0; i < n / 2; i ++) {
                lists[i] = merge(lists[i], lists[i + (n + 1) / 2]); // 看上面
            }
            n = newSize;
        }
        return lists[0];

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



### 102. 二叉树的层序遍历

层序遍历用队列

```c++
vector<vector<int>> levelOrder(TreeNode* root) {
    vector<vector<int>> res;
    if (root == nullptr)
        return res;
    queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        int size = q.size();
        vector<int> level;
        for (int i = 0; i < size; i++) {
            TreeNode* tmp = q.front();
            q.pop();
            level.push_back(tmp->val);
            if (tmp->left)
                q.push(tmp->left);
            if (tmp->right)
                q.push(tmp->right);
        }
        res.push_back(level);
    }
    return res;
}
```



### 98. 验证二叉搜索树

*递归*： 对于一个二叉搜索树，左边结点一定在 [-∞， 根节点的值]这个区间，而右边结点一定在 [根结点的值，正无穷]这个区间内。

所以只要判断每个节点是否在它应该在的区间内，并不断更新区间，就可以判断它是否是一颗二叉搜索树。

```c++
class Solution {
public:
    bool helper(TreeNode* root, long long lower, long long upper) {
        if(!root) return true;
        if(root->val <= lower || root->val >= upper) return false;
        return helper(root->left, lower, root->val) && helper(root->right, root->val, upper);
    }
    bool isValidBST(TreeNode* root) {
        return helper(root, LONG_MIN, LONG_MAX);
    }
};
```



### 230. 二叉搜索树中第k小的元素

*中序遍历*：一路遍历root结点的左节点，存入栈中，栈顶元素就是最小元素。跳出循环之后，k --，寻找第k小的元素。如果k == 0跳出循环返回值，如果还没找到第k个结点，呢就继续下一次循环增加根节点的右节点进到栈。

```c++
int kthSmallest(TreeNode* root, int k) {
    stack<TreeNode*> st;
    while(root != nullptr || st.size() > 0) {
        while(root != nullptr) {
            st.push(root);
            root = root->left;
        }
        root = st.top();
        st.pop();
        -- k;
        if(k == 0) break;
        root = root->right;
    }
    return root->val;
}
```



### 199. 二叉树的右视图

先递归右子树，再递归左子树，当某个深度首次到达时，对应的节点就在右视图中。

```c++
vector<int> rightSideView(TreeNode* root) {
    vector<int> res;
    dfs(root, 0, res);
    return res;
}
void dfs(TreeNode* root, int depth, vector<int>& res) {
    if(root == nullptr) return;
    if(depth == res.size()) {
        res.push_back(root->val);
    }
    dfs(root->right, depth + 1, res);
    dfs(root->left, depth + 1, res);
}
```



### 114. 二叉树展开为链表

*先序遍历*：先把先序遍历的结点存入到vector中，然后将vector中的树节点左指针指向nullptr，右指针指向后一个元素。

最后一个n - 1元素左右指针都指向nullptr。

```c++
void flatten(TreeNode* root) {
    if(root == nullptr) return;
    vector<TreeNode*> res;
    preOrder(root, res);
    int n = res.size();
    for(int i = 0; i < n - 1; ++ i) {
        res[i]->left = nullptr;
        res[i]->right = res[i + 1];
    }
    res[n - 1]->left = res[n - 1]-> right = nullptr;
}
void preOrder(TreeNode* root, vector<TreeNode*>& res) {
    if(root == nullptr) return;
    res.push_back(root);
    preOrder(root->left, res);
    preOrder(root->right, res);
}
```



### 236. 二叉树的最近公共祖先LCA

*递归：*非二叉搜索树，递归左子树找 p 或 q ，如果左右子树都非空，代表root为最近公共祖先，如果其中一个没有找到p或q，代表 p 和 q 在root的同一侧，则返回非空的left或right。

```c++
TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    if(root == nullptr || root == p || root == q) return root;
    TreeNode* left = lowestCommonAncestor(root->left, p, q);
    TreeNode* right = lowestCommonAncestor(root->right, p, q);
    if(left && right) return root;
    return left ? left : right;
}
```



### 124. 二叉树中的最大路径和

*递归*：计算从当前 `root` 出发的**最大贡献值**（即从 `root` 到某个叶子节点的最大路径和）

在递归过程中计算最大路径和 `maxSum`。

`maxGain(root)` 递归返回的是**从当前 `root` 开始的最大贡献值**，而**不是**路径和！

```c++
int maxSum = INT_MIN;
int maxGain(TreeNode* root) {
    if(root == nullptr) return 0;
    int leftGain = max(maxGain(root->left), 0);
    int rightGain = max(maxGain(root->right), 0);
    int pathPrice = leftGain + rightGain + root->val; // 用于记录最大路径和maxSum
    maxSum = max(pathPrice, maxSum); 
    return max(leftGain,rightGain) + root->val; // 用于递归的返回值 返回单条路径right或者left
}
int maxPathSum(TreeNode* root) {
    maxGain(root);
    return maxSum;
}
```



### 146. LRU缓存

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

### 

## 图论

### 200. 岛屿数量

*dfs*

```c++
#include<iostream>
#include<vector>
using namespace std;

const int dx[4] = { 0, 1, 0, -1 };
const int dy[4] = { 1, 0, -1, 0 };

void dfs(vector<vector<char>>& grid, int x, int y, int m, int n) {
	if (x < 0 || x >= m || y < 0 || y >= n || grid[x][y] == '0') return;
	grid[x][y] = '0';
	for (int i = 0; i < 4; i++) {
		dfs(grid, x + dx[i], y + dy[i], m, n);
	}
}

int numIslands(vector<vector<char>>& grid) {
	int m = grid.size(), n = grid[0].size();
	int count = 0;
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			if (grid[i][j] == '1') {
				count++;
				dfs(grid, i, j, m, n);
			}
		}
	}
	return count;
}
int main() {
	int m, n;
	cin >> m >> n;
	vector<vector<char>> grid(m, vector<char>(n));
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			cin >> grid[i][j];
		}
	}
	cout << numIslands(grid) << endl;
	return 0;
}
```



### 994. 腐烂的橘子

*bfs*：因为需要一圈一圈的扩散，所以用bfs，而且最主要的原因是，需要计算每次扩散的时间。dfs没办法做到这一点。

```c++
#include<iostream>
#include<queue>
using namespace std;

const int dx[4] = { 0, 0, 1, -1 };
const int dy[4] = { 1, -1, 0, 0 };

int orangeRotting(vector<vector<int>>& grid) {
	int time = 0, freshOrange = 0;
	queue<pair<int, int>> q;
	int m = grid.size(), n = grid[0].size();
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			if (grid[i][j] == 1) freshOrange++;
			else if (grid[i][j] == 2) q.push({ i, j });
		}
	}
	while (!q.empty() && freshOrange) {
		int size = q.size();
		for (int i = 0; i < size; i++) {
			pair<int, int> front = q.front();
			int x = front.first, y = front.second;
			q.pop();
			for (int d = 0; d < 4; d++) {
				int nx = x + dx[d], ny = y + dy[d];
				if (nx >= 0 && nx < m && ny >= 0 && ny < n && grid[nx][ny] == 1) {
					grid[nx][ny] = 2;
					q.push({ nx, ny });
					freshOrange--;
				}
			}
		}
		time++;
	}
	return freshOrange ? -1 : time;
}

int main() {
	int m, n;
	cin >> m >> n;
	vector<vector<int>> grid(m, vector<int>(n));
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			cin >> grid[i][j];
		}
	}
	cout << orangeRotting(grid) << endl;
	return 0;
}
```



### 207. 课程表

拓扑排序*bfs：*构建邻接表

`vector<int> inDegree(numCourses, 0);  // 记录每门课的前置课数量（入度）`
`vector<vector<int>> graph(numCourses); // 邻接表，graph[i] 存放课程 i 的后续课程`

入度为0的入队，取出队列的课程，可以立即学习。

遍历该课程的后继课程，将其入度-1，如果入度变为0则入队。

判断是否能学完 numCourses门课（无环）。

邻接表`graph`的结构

> 0: [1, 2]  // 课程 0 是 1 和 2 的前置课程
> 1: [3]     // 课程 1 是 3 的前置课程
> 2: [3]     // 课程 2 是 3 的前置课程
> 3: []      // 课程 3 没有后续课程

```c++
#include<iostream>
#include<vector>
#include<queue>
using namespace std;

bool canFinish(int numCourses, vector<vector<int>> prerequisites) {
	vector<int> inDegree(numCourses, 0);
	vector<vector<int>> graph(numCourses);
	for (auto p : prerequisites) {
		int next = p[0], pre = p[1];
		graph[pre].push_back(next);
		inDegree[next]++;
	}
	queue<int> q;
	for (int i = 0; i < numCourses; i ++) {
		if (inDegree[i] == 0) q.push(i);
	}
	int count = 0;
	while (!q.empty()) {
		auto front = q.front();
		q.pop();
		count++;
		for (int next : graph[front]) {
			inDegree[next]--;
			if (inDegree[next] == 0) q.push(next);
		}
	}
	return numCourses == count ? true : false;
}

int main() {
	int numCourse = 4;
	vector<vector<int>> prerequisites = { {1, 0}, {2, 0}, {3, 1}, {3, 2} };
	cout << (canFinish(numCourse, prerequisites) ? "true" : "false") << endl;
	return 0;
}
```



### 208. 实现Trie（前缀树）

`children`指针数组记录英文字母，`isEnd`用来标记是否字符串的结尾。

- 插入字符串

  - 如果子节点存在，则移动到子节点处理下一个字符。
  - 如果子节点不存在，则创建一个新的子节点，然后处理下一个字符。

- 查找字符串

  不断搜索下一个字符是否在字典树中，遍历完后返回`isEnd`，只有到达单词结尾，才是完整单词。

- 查找前缀

  同上，但不用判断isEnd。

```c++
#include<iostream>
#include<vector>
using namespace std;

class Trie {
private:
	struct TrieNode
	{
		TrieNode* children[26] = { nullptr };
		bool isEnd = false; // 记录是否为单词结尾
	};
	TrieNode* root;

public:
	Trie() {
		root = new TrieNode();
	}
	~Trie() { deleteTrie(root); }
	void insert(string word) {
		TrieNode* node = root;
		for (char ch : word) {
			int index = ch - 'a';
			if (!node->children[index]) {
				node->children[index] = new TrieNode();
			}
			node = node->children[index];
		}
		node->isEnd = true;
	}
	bool search(string word) {
		TrieNode* node = root;
		for (char ch : word) {
			int index = ch - 'a';
			if (!node->children[index]) return false;
			node = node->children[index];
		}
		return node->isEnd;
	}
	bool startsWith(string prefix) {
		TrieNode* node = root;
		for (char ch : prefix) {
			int index = ch - 'a';
			if (!node->children[index]) return false;
			node = node->children[index];
		}
		return true;
	}
private:
    // 递归删除掉trie树所有节点
	void deleteTrie(TrieNode* node) {
		if (!node) return;
		for (int i = 0; i < 26; i++) {
			if (node->children[i]) {
				deleteTrie(node->children[i]);
			}
		}
		delete node;
	}
};

int main() {
	Trie trie;
	int n;
	cin >> n;
	while (n--) {
		string op, word;
		cin >> op >> word;
		if (op == "insert") {
			trie.insert(word);
		}
		else if (op == "search") {
			cout << (trie.search(word) ? "true" : "false") << endl;
		}
		else {
			cout << (trie.startsWith(word) ? "true" : "false") << endl;
		}
	}
	return 0;
}
```

