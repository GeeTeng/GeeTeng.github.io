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

1️⃣ **先检查 `sum-k`** 是否存在，统计结果
2️⃣ **再更新 `mp[sum]`**，防止影响后续计算

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

> 给你一个字符串 `s` 、一个字符串 `t` 。返回 `s` 中涵盖 `t` 所有字符的最小子串。如果 `s` 中不存在涵盖 `t` 所有字符的子串，则返回空字符串 `""` 。

*滑动窗口双指针*：

`need` 记录 `t` 中字符的 **需求数量**。`window` 记录窗口内各字符的 **当前数量**。

`left, right` 维护滑动窗口的左右边界。`start, minLen` 用于存储 **最小窗口的起始位置** 和 **最小长度**。

`valid` 记录窗口中满足 `t` 需求的字符数（当 `valid == need.size()` 时，说明窗口已经包含了 `t` 中所有字符）。



右指针遍历s字符

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
            // 如果找到了包含t串的子串 则尝试收缩
            while(valid == need.size()) {
                // 更新最小覆盖子串的位置和长度
                if(right - left < minLen) {
                    start = left;
                    minLen = right - left;
                }
                // 缩小窗口
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

### 53. 最大子数组和

```c++
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        int n = nums.size();
        vector<int> dp(n);
        int res = nums[0];
        dp[0] = nums[0];
        for(int i = 1; i < n; i ++) {
            dp[i] = max(dp[i - 1] + nums[i], nums[i]);
            res = max(res, dp[i]);
        }
        return res;
    }
};
```



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



### 238. 除自身以外数组的乘积

```c++
class Solution {
public:
    vector<int> productExceptSelf(vector<int>& nums) {
        int n = nums.size();
        vector<int> front(n), back(n);
        vector<int> res(n);
        int mul = 1;
        for(int i = 0; i < n; i ++) {
            front[i] = mul;
            mul *= nums[i];
        }
        mul = 1;
        for(int i = n - 1; i >= 0; i --) {
            back[i] = mul;
            mul *= nums[i];
        }
        
        for(int i = 0; i < n; i ++) {
            res[i] = front[i] * back[i];
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



## 回溯

### 46. 全排列

*回溯法*

遍历u到n数字，交换u和i的位置，再复位。

```c++
#include<iostream>
#include<vector>
using namespace std;

void backtrack(vector<vector<int>>& res, vector<int>& output, int u, int n) {
	if (u == n) {
		res.push_back(output);
		return;
	}
	for (int i = u; i < n; i++) {
		swap(output[i], output[u]);
		backtrack(res, output, u + 1, n);
		swap(output[i], output[u]);
	}
}

vector<vector<int>> permute(vector<int>& nums) {
	int n = nums.size();
	vector<vector<int>> res;
	backtrack(res, nums, 0, n);
	return res;
}


int main() {
	int n;
	cin >> n;
	vector<int> nums(n);
	for (int i = 0; i < n; i++) {
		int x;
		cin >> nums[i];
	}
	vector<vector<int>> res = permute(nums);
	for (auto r : res) {
		for (auto i : r) {
			cout << i << ' ';
		}
		cout << endl;
	}
	return 0;
}
```



### 78. 子集

*回溯法*

加入这个数字的情况

弹出这个数字的情况

```c++
#include<iostream>
#include<vector>
using namespace std;

vector<int> output;
vector<vector<int>> res;

void backtrack(vector<int> nums, int cur) {
	if (nums.size() == cur) {
		res.push_back(output);
		return;
	}
	output.push_back(nums[cur]);
	backtrack(nums, cur + 1);
	output.pop_back();
	backtrack(nums, cur + 1);
}
vector<vector<int>> subsets(vector<int> nums) {
	backtrack(nums, 0);
	return res;
}

int main() {
	int n;
	cin >> n;
	vector<int> input(n);
	for (int i = 0; i < n; i++) {
		cin >> input[i];
	}
	subsets(input);
	for (auto& r : res) {
		for (auto i : r) {
			cout << i << ' ';
		}
		cout << endl;
	}
	return 0;
}
```



### 17. 电话号码的字母组合

*回溯法*

`vector<string> phone` 用来存放电话号码对应的字母，比如说2对应的是abc，那么phone[2] = "abc"。

遍历phone[2]对应的这个字符串，依次加入a、b、c的情况去递归，再回溯。

```c++
#include<iostream>
#include<vector>
using namespace std;

vector<string> phone = { "",    "",    "abc",  "def", "ghi", "jkl", "mno", "pqrs", "tuv", "wxyz" };
vector<string> res;

void backtrack(const string digit, int index, string& path, vector<string>& res, const vector<string>& phone) {
	if (index == digit.size()) {
		res.push_back(path);
		return;
	}
	int num = digit[index] - '0';
	for (auto c : phone[num]) {
		path.push_back(c);
		dfs(digit, index + 1, path, res, phone);
		path.pop_back();
	}
}

vector<string> letterCombinations(string digits) {
	string path;
	backtrack(digits, 0, path, res, phone);
	return res;
}

int main() {
	string digits;
	cin >> digits;
	letterCombinations(digits);
	for (auto s : res) {
		cout << s << ' ';
	}
	return 0;
}
```



### 39. 组合总和

*回溯法*

`index`记录的是`candidates`中第几位数字，`combine`记录当前正在尝试的组合。如果`target = 0`就可以存入res了。

- 当不选择该数字的时候，index 要 + 1。
- 当选择该数字时，前提条件是选择了之后target是否 >0，否则选择没意义。选择该数字的时候index不变，因为可以重复选择。

```c++
// candidates候选数字数组 target目标值 res存放所有满足条件 combine正在尝试的组合 index当前搜索的candidates索引
void backtrack(vector<int>& candidates, int target, vector<vector<int>>& res, vector<int>& combine, int index) {
    if(index == candidates.size()) {
        return;
    }
    if(target == 0) {
        res.push_back(combine);
        return;
    }
    // 不选当前数字 尝试后面元素
    backtrack(candidates, target, res, combine, index + 1);
    // 只有当 target - candidates[index] >= 0，才考虑选 candidates[index] 不然就是负数了
    if(target - candidates[index] >= 0) {
        combine.push_back(candidates[index]);
        // 允许重复选择 所以下一层依旧是index
        backtrack(candidates, target - candidates[index], res, combine, index);
        combine.pop_back();
    }
}

vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
    vector<vector<int>> res;
    vector<int> combine;
    backtrack(candidates, target, res, combine, 0);
    return res;
}
```



### 22. 括号生成

*回溯法*

**终止条件**：当left和right都等于n时，这个时候所有括号都用完了。

**加入左括号的条件**：当left < n时，还有剩余左括号

**加入右括号的条件**：当right < left时，代表还有多余右括号可以匹配。 

```c++
class Solution {
public:
    void backtrack(int n, vector<string>& res, string& current, int left, int right) {
        if (left == n && right == n) {
            res.push_back(current);
            return;
        }
        if (left < n) {
            current.push_back('(');
            backtrack(n, res, current, left + 1, right);
            current.pop_back();
        }
        if (right < left) {
            current.push_back(')');
            backtrack(n, res, current, left, right + 1);
            current.pop_back();
        }
    }
    vector<string> generateParenthesis(int n) {
        vector<string> res;
        string current;
        backtrack(n, res, current, 0, 0);
        return res;
    }
};
```



### 79. 单词搜索

*回溯法*

像是图论问题，如果超出边界或者该字符不等于`word[k]`则返回false，遍历四个方向后再将该点的字符改回原值。

```c++
class Solution {
public:
    int dx[4] = {1, -1, 0, 0};
    int dy[4] = {0, 0, 1, -1};
    bool backtrack(vector<vector<char>>& board, string& word, int i, int j, int k) {
        if(k == word.size()) return true;
        if(i < 0 || i >= board.size() || j < 0 || j >= board[0].size() || word[k] != board[i][j]) return false;
        char tmp = board[i][j];
        board[i][j] = '#';
        for(int d = 0; d < 4; d ++) {
            int x = i + dx[d], y = j + dy[d];
            if(backtrack(board, word, x, y, k + 1)) return true;
        }
        board[i][j] = tmp;
        return false;
         
    }
    bool exist(vector<vector<char>>& board, string word) {
        int n = board.size(), m = board[0].size();
        for(int i = 0; i < n; i ++) {
            for(int j = 0; j < m; j ++) {
                if(backtrack(board, word, i, j, 0)) return true;
            }
        }
        return false;
    }
};
```



### 131. 分割回文串

*回溯法 + 动态规划*

动态规划：先预处理`isPalindrome`数组，用 `dp[i][j]` 表示 `s[i:j]` 是否为回文。如果`s[i] == s[j]`则长度为1和2的字符串一定是回文，如果长度超过2，则回文判断方法为`dp[i][j] = dp[i+1][j-1])`。

回溯搜索：维护一个 `path` 记录当前切分方案，遍历 `s[i:j]`，如果是回文，则递归搜索 `s[j+1:]`。当 `start == s.size()` 时，说明找到了一组有效方案。

```c++
class Solution {
public:
    vector<vector<string>> res;
    vector<string> path;
    vector<vector<bool>> isPalindrome;
    void backtrack(string& s, int start) {
        if(start == s.size()) {
            res.push_back(path);
            return;
        }
        for(int end = start;  end < s.size(); end ++) {
            if(isPalindrome[start][end]) {
                path.push_back(s.substr(start, end - start + 1));
                backtrack(s, end + 1);
                path.pop_back();
            }
        }
    }
    vector<vector<string>> partition(string s) {
        int n = s.size();
        isPalindrome.assign(n, vector<bool>(n, false));
        for(int len = 1; len <= n; len ++) {
            for(int i = 0; i + len - 1 < n; i ++) {
                int j = i + len - 1;
                if(s[i] == s[j]) {
                    if(len == 1 || len == 2) isPalindrome[i][j] = true;
                    else isPalindrome[i][j] = isPalindrome[i + 1][j - 1];
                }
            }
        }
        backtrack(s, 0);
        return res;
    }
};
```



### 51. N皇后

*回溯法*

主对角线和副对角线的公式是`y = x + b`和`y = -x + b`，截距相同时两个元素在同一对角线上，所以`b = x + y`和`b = y - x`。

`row` 实际上是 `u`（当前遍历到的行），`col` 是 `i`（当前尝试放置皇后的列）。所以`dg[u + i]`代表主对角线，`udg[n - u + i]`代表副对角线。为什么有一个n，是因为防止`-u+i<0`（由于 `(row - col)` 可能为负数，为了不出现负索引，我们 **加上 `n` 作为偏移量**）。

```c++
class Solution {
public:
    vector<vector<string>> res;
    vector<string> path;
    vector<bool> col, dg, udg;
    void dfs(int n, int u) {
        if(u == n) {
            res.push_back(path);
            return;
        }
        for(int i = 0; i < n; i ++) {
            if(!col[i] && !dg[u + i] && !udg[n - u + i]) {
                path[u][i] = 'Q';
                col[i] = dg[u + i] = udg[n - u + i] = true;
                dfs(n, u + 1);
                col[i] = dg[u + i] = udg[n - u + i] = false;
                path[u][i] = '.';
            }
        }
    }
    vector<vector<string>> solveNQueens(int n) {
        res.clear();
        path.assign(n, string(n, '.'));
        col.assign(n, false);
        dg.assign(2 * n, false);
        udg.assign(2*n, false);
        dfs(n, 0);
        return res;
    }
};
```



## 二分查找

*二分查找分为两种情况*：

**精确查找**和**查找边界**，如果是精确查找的话`while(i <= j)`，如果是查找边界那么循环是`while(i < j)`。

精确查找的循环停止条件是空区间`i > j`，而查找边界的循环停止条件是 `i == j`。

| 问题类型         | i更新       | j更新       | mid计算方式           |
| ---------------- | ----------- | ----------- | --------------------- |
| 精确查找         | i = mid + 1 | j = mid - 1 | mid = (i + j) / 2     |
| 找边界（左边界） | i + mid + 1 | j = mid     | mid = (i + j) / 2     |
| 找边界（右边界） | i = mid     | j = mid - 1 | mid = (i + j + 1) / 2 |

总结规律就是：找边界问题，找哪个边界就哪个多移动；查找单个元素就两边都移动。并且移动的那方一定是`<`或`>`target而非`<=`或`>=`。

### 74. 搜索二维矩阵

使用index来记录target可能在的行。

```c++
class Solution {
public:
    bool searchMatrix(vector<vector<int>>& matrix, int target) {
        int n = matrix.size(), m = matrix[0].size();
        int i = 0, j = n - 1;
        int index = 0;
        while(i <= j) {
            int mid = i + j >> 1;
            if(matrix[mid][0] > target) j = mid - 1;
            else {
                i = mid + 1;
                index = mid;
            }
        }
        i = 0, j = m - 1;
        while(i <= j) {
            int mid = i + j >> 1;
            if(matrix[index][mid] == target) return true;
            else if(matrix[index][mid] > target) j = mid - 1;
            else i = mid + 1;
        }
        return false;
    }
};
```



### 34. 在排序数组中查找元素的第一个和最后一个位置

```c++
class Solution {
public:
    vector<int> searchRange(vector<int>& nums, int target) {
        int n = nums.size();
        if(n == 0) return {-1, -1};
        int i = 0, j = n - 1;
        while(i < j) {
            int mid = (i + j) >> 1;
            if(nums[mid] < target) i = mid + 1;
            else j = mid;
        }
        if(nums[i] != target) return{-1, -1};
        int left = i;
        j = n - 1;
        while(i < j) {
            int mid = (i + j + 1) >> 1;
            if(nums[mid] > target) j = mid - 1;
            else i = mid;
        }
        return {left, j};
    }
};
```



### 33. 搜索旋转数组

直接二分查找即可，二分查找后分为两个区域，`[l, mid]`和`(mid,r]`，判断哪一区域是有序的。

举例左半部分有序：

- 如果taget在这个区间内（即target >= nums[l] && target < nums[mid]），则正常二分查找j = mid - 1;
- 如果target不在该区间内，则就在另外呢个无序区间内，l = mid + 1；

```c++
class Solution {
public:
    int search(vector<int>& nums, int target) {
        int n = nums.size();
        int i = 0, j = n - 1;
        while(i <= j) {
            int mid = (i + j) >> 1;
            if(target == nums[mid]) return mid;
            if(nums[i] <= nums[mid]) {
                if(nums[i] <= target && nums[mid] > target) j = mid - 1;
                else i = mid + 1;
            }
            else {
                if(nums[mid] < target && nums[j] >= target) i = mid + 1;
                else j = mid - 1;
            }
        }
        return -1;
    }
};
```



### 153. 寻找旋转排序数组中的最小值

如果`nums[mid] > nums[r]`，意味着`mid`位置绝不可能是最小值，且右侧数据整体小于左侧数据，最小值一定在右侧，所以`l = mid + 1`。

否则，就是nums[mid] <= nums[r]，这意味着右侧数据是有序的，最小值不可能在右侧区间内部（因为递增），但有可能是`mid`也有可能是左侧数据，所以r = mid。

> 不是说精确查找是`while(l <= r)`吗，为什么该题找最小值也是精确查找但是`while(l < r)`？

因为精确查找固定值的时候，有可能值不存在，所以要遍历到区间为空为止。但是最小值一定会存在，所以遍历到只剩下一个元素就可以得到了。

```c++
class Solution {
public:
    int findMin(vector<int>& nums) {
        int n = nums.size();
        int l = 0, r = n - 1;
        while(l < r) {
            int mid = (l + r) >> 1;
            if(nums[mid] > nums[r]) l = mid + 1;
            else {
                r = mid;
            }
        }
        return nums[l];
    }
};
```



### 4. 寻找两个正序数组的中位数

将寻找中位数问题转换成寻找第k小的数，因为要求时间复杂度，所以没办法把数组真的合并。

index用来记录当前索引，k用来找中位数的位置（不断减少，直到1的时候返回）。

```c++
class Solution {
public:
    int getKthElement(const vector<int>& nums1, const vector<int>& nums2, int k) {
        int m = nums1.size();
        int n = nums2.size();
        int index1 = 0, index2 = 0;
        while(true) {
            if(index1 == m) {
                return nums2[index2 + k - 1];
            }
            if(index2 == n) {
                return nums1[index1 + k - 1];
            }
            if(k == 1) {
                return min(nums1[index1], nums2[index2]);
            }
            int newIndex1 = min(index1 + k / 2 - 1, m - 1);
            int newIndex2 = min(index2 + k / 2 - 1, n - 1);
            int pivot1 = nums1[newIndex1];
            int pivot2 = nums2[newIndex2];
            if(pivot1 <= pivot2) {
                k -= newIndex1 - index1 + 1;
                index1 = newIndex1 + 1;
            } else {
                k -= newIndex2 - index2 + 1;
                index2 = newIndex2 + 1;
            }
        }
    }

    double findMedianSortedArrays(vector<int>& nums1, vector<int>& nums2) {
        int totalLength = nums1.size() + nums2.size();
        if(totalLength % 2 == 1) {
            return getKthElement(nums1, nums2, (totalLength + 1) / 2);
        } else {
            return (getKthElement(nums1, nums2, totalLength / 2) + getKthElement(nums1, nums2, (totalLength / 2) + 1)) / 2.0;
        }
    }
};
```



## 栈

### 20. 有效的括号

栈用来存储左括号，如果遇到右括号那么就弹出左括号匹配是否相等。

```c++
class Solution {
public:
    bool isValid(string s) {
        int n = s.length();
        if(n % 2 == 1) {
            return false;
        }
        unordered_map<char, char> pairs = {
            {']', '['},
            {')', '('},
            {'}', '{'}
        };
        stack<char> stk;
        for(char ch : s) {
            // 如果是右括号则取出左括号弹出
            if(pairs.count(ch)) {
                if(stk.empty() || stk.top() != pairs[ch]) {
                    return false;
                }
                stk.pop();
            } // 如果是左括号 就入栈
            else {
                stk.push(ch);
            }
        }
        return stk.empty();
    }
};
```



### 394. 字符串解码

两个栈分别存放数字和字符串

当遇到数字时，可能是多位数字，所以`num = num * 10 + (ch - '0');` 

当遇到`[`时，要将栈中放入currentStr和num，并且置空这两个变量。

当遇到`]`时，取出两个栈顶的数字和字符串，然后去重复字符串。

当遇到字符时，就直接加入到currentStr中。

```c++
class Solution {
public:
    string decodeString(string s) {
        stack<int> countStack;
        stack<string> strStack;
        string currentStr;
        int num = 0;
        for(char ch : s) {
            if(isdigit(ch)) {
                num = num * 10 + (ch - '0');
            }
            else if(ch == '[') {
                countStack.push(num);
                strStack.push(currentStr);
                num = 0;
                currentStr = "";
            }
            else if(ch == ']') {
                int repeat = countStack.top();
                countStack.pop();
                string prevStr = strStack.top();
                strStack.pop();
                string temp;
                for(int i = 0; i < repeat; i ++) {
                    temp += currentStr;
                }
                currentStr = prevStr + temp; 
            }
            else {
                currentStr += ch;
            }
        }
        return currentStr;
    }
};
```



### 739. 每日温度

单调栈，当遇到比栈顶温度大的天数，就得出了最近温度升高的那天，也就是`i - stk.top()`，并弹出栈顶元素。

```c++
class Solution {
public:
    vector<int> dailyTemperatures(vector<int>& temperatures) {
        int n = temperatures.size();
        vector<int> res(n, 0);
        stack<int> stk;
        for(int i = 0; i < n; i ++) {
            while (!stk.empty() && temperatures[stk.top()] < temperatures[i]) {
                int preDay = stk.top();
                res[preDay] = i - preDay;
                stk.pop();
            }
            stk.push(i);
        }
        return res;
    }
};
```



### 84.柱状图中最大的矩形

单调栈递增，在heights数组末尾追加0，为了停止循环确保所有柱子都被弹出。

右边界为i，左边界为l。宽度`width = i - l - 1`;

```c++
class Solution {
public:
    int largestRectangleArea(vector<int>& heights) {
        stack<int> st;
        int ans = 0;
        heights.push_back(0);
        for(int i = 0; i < heights.size(); i ++) {
            while(!st.empty() && heights[st.top()] >= heights[i]) {
                int tmp = st.top();
                st.pop();
                int l = st.empty() ? -1 : st.top();
                ans = max(ans, (i - l - 1) * heights[tmp]);
            }
            st.push(i);
        }
        return ans;
    }
};
```

