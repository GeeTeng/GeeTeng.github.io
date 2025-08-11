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

## 哈希

### 49. 字母异位词分组

```c++
class Solution {
public:
    vector<vector<string>> groupAnagrams(vector<string>& strs) {
        int n = strs.size();
        vector<vector<string>> res;
        unordered_map<string, vector<string>> mp;
        for(int i = 0; i < n; i ++) {
            string tmp = strs[i];
            sort(tmp.begin(), tmp.end());
            mp[tmp].emplace_back(strs[i]);
        }
        for(auto it = mp.begin(); it != mp.end(); it ++) {
            res.push_back(it->second);
        }
        return res;
    }
};
```



### 128. 最长连续序列

如果num - 1是不存在的，则开始往后查找num + 1，每次更新num为num + 1，继续找num + 1，直到找不到位置，去更新maxlen。

注意：要遍历unordered_set，而不是原数组，否则会TLE。

```c++
class Solution {
public:
    int longestConsecutive(vector<int>& nums) {
        unordered_set<int> map(nums.begin(), nums.end());
        int res = 0, cnt = 1;
        for (int num : map) {
            cnt = 1;
            if (map.find(num - 1) == map.end()) {
                while (map.find(num + 1) != map.end()) {
                    cnt++;
                    num++;
                }
                res = max(res, cnt);
            }
        }
        return res;
    }
};
```



## 双指针

### 283. 移动零

l和r都从0开始遍历，r不断地++遍历整个数组，只有当r所指数字不为0时，l才会移动。

为了保证数字的顺序，所以要交换的条件是当r不等于0的时候去交换。

```c++
class Solution {
public:
    void moveZeroes(vector<int>& nums) {
        int l = 0, r = 0;
        int n = nums.size();
        while (r < n) {
            if (nums[r]) {
                swap(nums[l], nums[r]);
                l++;
            }
            r++;
        }
    }
};
```



### 15. 三数之和

将找三个数转换成找两个数的问题，`target = -k`，将数组排序，每个不重复的数字都会成为target。

i要从大于k的地方开始寻找，j从数组结尾开始寻找。遇到相同数字时，要跳过，这些是为了防止找到重复的元素。

如果nums[k]大于0的时候，target就是负数，而大于k的数字都是正数，所以不可能会出现答案，于是跳过。

```c++
if (nums[k] > 0) break;
```

跳过重复的解

```c++
if (k > 0 && nums[k] == nums[k - 1]) continue;
```

为了跳过重复的答案，比如{-1， -1， 2}，{-1， -1， 2}

```c++
while (i < j && nums[i] == nums[i + 1]) i++;
while (i < j && nums[j] == nums[j - 1]) j--;
```

`nums[k]` 是**跳过相同起点的三元组**；

`nums[i]` 和 `nums[j]` 是**跳过双指针阶段的重复解**；

这两个步骤缺一不可，否则**同样的三元组会被加入多次**。

```c++
vector<vector<int>> threeSum(vector<int>& nums) {
	sort(nums.begin(), nums.end());
	vector<vector<int>> res;
	for (int k = 0; k < nums.size(); k++) {
		if (nums[k] > 0) break;
		if (k > 0 && nums[k] == nums[k - 1]) continue;
		int target = -nums[k];
		int i = k + 1, j = nums.size() - 1;
		while (i < j) {
			if (nums[j] + nums[i] > target) j--;
			else if (nums[j] + nums[i] < target) i++;
			else {
				res.push_back({ nums[i], nums[j], nums[k] });
				while (i < j && nums[i] == nums[i + 1]) i++;
				while (i < j && nums[j] == nums[j - 1]) j--;
				i++;
				j--;
			}
		}
	}
	return res;
}
```



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

注意：一定要初始化总和0为1，并且res累加的是哈希表中的值而不是res ++，因为同一前缀和可能有多个连续数组组成。

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



### *76. 最小覆盖字串

> 给你一个字符串 `s` 、一个字符串 `t` 。返回 `s` 中涵盖 `t` 所有字符的最小子串。如果 `s` 中不存在涵盖 `t` 所有字符的子串，则返回空字符串 `""` 。

*滑动窗口双指针*：

`need` 记录 `t` 中字符的 **需求数量**。`window` 记录窗口内各字符的 **当前数量**。

`left, right` 维护滑动窗口的左右边界。`start, minLen` 用于存储 **最小窗口的起始位置** 和 **最小长度**。

`valid` 记录窗口中满足 `t` 需求的字符数（当 `valid == need.size()` 时，说明窗口已经包含了 `t` 中所有字符）。

注意：尽管移动r和移动l的两部分代码很相似，但是区别在于增加字符的时候，要先增加再判断，而减少字符的时候，要先判断再减少。

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

*动态规划：*

res和dp的初始化要是nums[0]，目的是防止数组只有负数。  

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

### 234. 回文链表

可以存到vector中用双指针比较，但是缺点是需要花费空间。

所以使用快慢指针寻找回文的中点，代码中`l1`就是回文右半部分开始的地方。

将右半部分反转，形成两个链表，分别是`head为头结点的left`和`prev为头结点的right`，之后再去比较值是否相等

。

```c++
class Solution {
public:
    bool isPalindrome(ListNode* head) {
        if(head == nullptr || head->next == nullptr) return true;
        ListNode* l1 = head, *l2 = head;
        while(l2 && l2->next) {
            l1 = l1->next;
            l2 = l2->next->next;
        }
        ListNode* prev = nullptr;
        while(l1) {
            ListNode* tmp = l1->next;
            l1->next = prev;
            prev = l1;
            l1 = tmp;
        }
        ListNode* left = head, *right = prev;
        while(right) {
            if(right->val != left->val) return false;
            left = left->next;
            right = right->next;
        }
        return true;
    }
};
```



### 141. 环形链表

```c++
#include<iostream>
#include<vector>
using namespace std;

struct ListNode
{
	ListNode* next;
	int val;
	ListNode(int val):val(val), next(nullptr) {}
};

bool hasCircle(ListNode* head) {
	ListNode* slow = head, * fast = head;
	do {
		if (slow) slow = slow->next;
		else return false;
		if (fast && fast->next) fast = fast->next->next;
		else return false;
	} while (slow != fast);
	return true;
}

int main() {
	int n, pos;
	cin >> n >> pos;
	vector<ListNode*> nodes;
	for (int i = 0; i < n; i++) {
		int val;
		cin >> val;
		ListNode* node = new ListNode(val);
		nodes.push_back(node);
		if (i > 0) nodes[i - 1]->next = nodes[i];
	}
	if (pos != -1 && pos < n) {
		nodes[n - 1]->next = nodes[pos];
	}
	ListNode* head = nodes[0];
	bool res = hasCircle(head);
	cout << res << endl;
	return 0;
}
```



### 142. 环形链表Ⅱ

```c++
#include<iostream>
#include<vector>
using namespace std;

struct ListNode
{
	ListNode* next;
	int val;
	ListNode(int val):val(val), next(nullptr) {}
};

ListNode* insert(ListNode* node, int val) {
	if (node == nullptr) return new ListNode(val);
	ListNode* newNode = new ListNode(val);
	newNode->next = node->next;
	node->next = newNode;
	return node;
}

bool hasCircle(ListNode* head) {
	ListNode* slow = head, * fast = head;
	do {
		if (slow) slow = slow->next;
		else return false;
		if (fast && fast->next) fast = fast->next->next;
		else return false;
	} while (slow != fast);
	return true;
}

ListNode* hasCircle2(ListNode* head) {
	ListNode* slow = head, * fast = head;
	do {
		if (slow) slow = slow->next;
		else return nullptr;
		if (fast && fast->next) fast = fast->next->next;
		else return nullptr;
	} while (slow != fast);
	fast = head;
	while (slow != fast) {
		fast = fast->next;
		slow = slow->next;
	}
	return slow;
}

int main() {
	int n, pos;
	cin >> n >> pos;
	vector<ListNode*> nodes;
	for (int i = 0; i < n; i++) {
		int val;
		cin >> val;
		ListNode* node = new ListNode(val);
		nodes.push_back(node);
		if (i > 0) nodes[i - 1]->next = nodes[i];
	}
	if (pos != -1 && pos < n) {
		nodes[n - 1]->next = nodes[pos];
	}
	ListNode* head = nodes[0];
	ListNode* res = hasCircle2(head);
	cout << res->val << endl;
	return 0;
}
```



### 21. 合并两个有序链表

```c++
#include<iostream>
#include<vector>

using namespace std;

struct ListNode {
	int val;
	ListNode* next;
	ListNode(int val) :val(val), next(nullptr) {};
};

ListNode* merge(ListNode* head1, ListNode* head2)
{
	ListNode* res = new ListNode(0);
	ListNode* cur1 = head1, * cur2 = head2, *cur = res;
	while(cur1 && cur2) {
		if (cur1->val > cur2->val) {
			cur->next = cur2;
			cur2 = cur2->next;
		}
		else {
			cur->next = cur1;
			cur1 = cur1->next;
		}
		cur = cur->next;
		
	}
	if (cur1) cur->next = cur1;
	if (cur2) cur->next = cur2;
	return res->next;
}
int main() {
	int n1, n2;
	cin >> n1 >> n2;
	vector<ListNode*> nodes1, nodes2;
	for (int i = 0; i < n1; i++) {
		int val;
		cin >> val;
		ListNode* node = new ListNode(val);
		nodes1.push_back(node);
		if (i > 0) nodes1[i - 1]->next = nodes1[i];
	}
	for (int i = 0; i < n2; i++) {
		int val;
		cin >> val;
		ListNode* node = new ListNode(val);
		nodes2.push_back(node);
		if (i > 0) nodes2[i - 1]->next = nodes2[i];
	}
	ListNode* head1 = nodes1[0], * head2 = nodes2[0];
	ListNode* res = merge(head1, head2);
	ListNode* cur = res;
	while (cur) {
		cout << cur->val << ' ';
		cur = cur->next;
	}
	return 0;
}
```



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

但是要注意当只有一个元素的时候是无法找到p2，即第N个结点的前一个结点，所以要加一个头结点0。

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



### *25. K个一组反转链表

增加一个头结点，pre指向头结点，cur为head，还有一个next指针永远指向cur->next。

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



### *138. 随机链表的复制

将原链表的结点对应的拷贝结点连在其后，链表从A - B - C变成A - A' - B - B' - C - C'。

然后再去连接random指针；

最后将原链表和复制的链表拆分，各自指向各自的下一节点。

```c++
class Solution {
public:
    Node* copyRandomList(Node* head) {
        if(head == nullptr) return nullptr;
        Node* cur = head;
        while(cur) {
            Node* tmp = new Node(cur->val);
            tmp->next = cur->next;
            cur->next = tmp;
            cur = tmp->next;
        }
        Node* old = head, *clone = head->next, *res = head->next;
        while(old) {
            clone->random = old->random == nullptr ? nullptr : old->random->next;
            if(old->next) old = old->next->next;
            if(clone->next) clone = clone->next->next;
        }
        old = head, clone = head->next;
        while(old) {
            if(old->next) old->next = old->next->next;
            if(clone->next) clone->next = clone->next->next;
            old = old->next;
            clone = clone->next;
        }
        return res;
    }
};
```



### *148. 排序链表

*归并排序*

由于merge只能合并两个有序的数列，所以要`ListNode* left = sortList(head), *right = sortList(slow);`递归的将链表不断分割。

要找到中间节点吗，使用快慢指针。

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

    ListNode* sortList(ListNode* head) {
        if(head == nullptr || head->next == nullptr) return head; 
        ListNode* slow = head, *fast = head, *prev = nullptr;
        while(fast && fast->next) {
            fast = fast->next->next;
            prev = slow;
            slow = slow->next;
        }
        prev->next = nullptr;
        ListNode* left = sortList(head), *right = sortList(slow);
        return merge(left,  right);
    }
};
```



### *23. 合并K个升序链表

*归并排序*：和排序链表一样的道理，只不过K个需要两两合并，**分治法**。

使用 `i += 2` 遍历，每次处理两个链表。

- 如果 `i + 1 < n`，合并 `lists[i]` 和 `lists[i + 1]`。
- 如果 `i + 1 >= n`（即奇数情况），直接保留 `lists[i]`。

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
            int newsize = 0;
            for(int i = 0; i < n; i += 2) {
                if(i + 1 < n) {
                    lists[newsize ++] = merge(lists[i], lists[i + 1]); 
                }
                else lists[newsize ++] = lists[i];
            }
            n = newsize;
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

注意：最长路径并不代表一定经过根节点，

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



### *230. 二叉搜索树中第k小的元素

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

*原地展开：*

```c++
    void flatten(TreeNode* root) {
        TreeNode* cur = root;
        while(cur != nullptr) {
            if(cur->left != nullptr) {
                auto next = cur->left;
                auto predecessor = next;
                // 找next的最右边结点，也就是cur的右结点的上一个结点。
                while(predecessor->right != nullptr) {
                    predecessor = predecessor->right;
                }
                // 连接cur右节点和next的最右结点
                predecessor->right = cur->right;
                // 把cur左指针（指向next）置空 为了右指针指向next
                cur->left = nullptr;
                cur->right = next;
            }
            cur = cur->right;
        }
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
struct LRUCache
{
	int capcity;
	list<pair<int, int>> cache;
	unordered_map<int, list<pair<int, int>>::iterator> hashTable;
	LRUCache(int cap) :capcity(cap) { };
	void put(int key, int val) {
		if (hashTable.find(key) != hashTable.end()) {
			hashTable[key]->second = val;
			cache.splice(cache.begin(), cache, hashTable[key]);
		}
		else {
			if (capcity <= cache.size()) {
				auto last = cache.back();
				hashTable.erase(last.first);
				cache.pop_back();
			}
			// 必须是这个顺序先pushback然后再哈希表指向list的迭代器
			// 因为一旦list为空，就指向了一个无效的迭代器，呢么会导致崩溃
			cache.push_back({ key, val });
			hashTable[key] = cache.begin();
		}
	}
	int get(int key) {
		if (hashTable.find(key) != hashTable.end()) {
			// splice 第一个参数是pos目标位置 把元素插入到该位置
			// 第二个参数是数据来源的list
			// 第三个参数是要被移动的元素迭代器
			cache.splice(cache.begin(), cache, hashTable[key]);
			return hashTable[key]->second;
		}
		else return -1;
	}
};

int main() {
	LRUCache res(2);
	res.put(1, 1);
	res.put(2, 2);
	cout << res.get(1) << endl;
	res.put(3, 3);
	cout << res.get(2) << endl;

	return 0;
}
```



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



### *994. 腐烂的橘子

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

*拓扑排序bfs*：构建邻接表

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

Trie树是由一个个TrieNode构成，每个TrieNode结点都包含着26个子节点指针，可以说当存入一个apple字符串，实际上是存储了5 * 26 * 8（指针大小）的大小。

构造函数用于创建root根节点TrieNode，析构函数用于删除以root为根结点的所有TrieNode结点。

另外3个功能函数：

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
		// 每个节点有26个子节点指针（对应26个小写字母）
		TrieNode* children[26] = { nullptr };
		bool isEnd = false; // 记录是否为单词结尾
	};
	TrieNode* root;

public:
	Trie() {
        // 注意要用类的root成员初始化
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

重点：将字符改变成‘#’，递归，再改回来回溯。不需要记录选择了什么字符，只需要记录x y和index。

像是图论问题，如果超出边界或者该字符不等于`word[k]`则返回false，遍历四个方向后再将该点的字符改回原值。

```c++
class Solution {
public:
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};
    bool exist(vector<vector<char>>& board, string word) {
        for(int i = 0; i < board.size(); i ++) {
            for(int j = 0; j < board[0].size(); j ++) {
                if(board[i][j] == word[0]) if(dfs(board, word, 0, i, j)) return true;
            }
        }
        return false;
    }
    bool dfs(vector<vector<char>>& board, string word, int index, int x, int y) {
        if(index == word.size() - 1) return true;
        char tmp = board[x][y];
        board[x][y] = '#';
        for(int i = 0; i < 4; i ++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if(nx < board.size() && nx >= 0 && ny < board[0].size() && ny >= 0 && board[nx][ny] == word[index + 1]) {
                if(dfs(board, word, index + 1, nx, ny)) return true;
            }
        }
        board[x][y] = tmp;
        return false;
    }
};
```



### 131. 分割回文串

回溯搜索：维护一个 `path` 记录当前切分方案，遍历 `s[i:j]`，如果是回文，则递归搜索 `s[j+1:]`。当 `start == s.size()` 时，说明找到了一组有效方案。

```c++
class Solution {
public:
    vector<vector<string>> partition(string s) {
        vector<vector<string>> res;
        vector<string> cur;
        backtrack(s, 0, cur, res);
        return res;
    } // 不断地切分字符串 比如aab，start = 0，a是回文则继续递归start = 1，看之后的字符。
    void backtrack(string s, int start, vector<string>& cur, vector<vector<string>>& res) {
        if(start == s.size()) {
            res.push_back(cur);
            return;
        }
        for(int i = start; i < s.size(); i ++) {
            string tmp = s.substr(start, i - start + 1);
            if(isHuiWen(tmp)) {
                cur.push_back(tmp);
                backtrack(s, i + 1, cur, res);
                cur.pop_back();
            }
        }

    } // 判断是否回文串
    bool isHuiWen(string tmp) {
        int i = 0, j = tmp.size() - 1;
        while(i < j) {
            if(tmp[i] != tmp[j]) return false;
            i ++, j --;
        }
        return true;
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



### 35. 搜索插入位置

```c++
int searchInsert(vector<int>& nums, int target) {
    int n = nums.size();
    int l = 0, r = n - 1;
    int mid = (l + r) / 2;
    while(l <= r) {
        int mid = (l + r) / 2;
        if(nums[mid] < target) l = mid + 1;
        else if(nums[mid] > target) r = mid - 1;
        else return mid;
    }
    return l;
}
```





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
        } // 判断是否还有多余字符 有的话则无效
        return stk.empty();
    }
};
```



### 394. 字符串解码

两个栈分别存放数字和字符串

当遇到数字时，可能是多位数字，所以`num = num * 10 + (ch - '0');` 

当遇到`[`时，要将栈中放入currentStr和num，并且置空这两个变量。

当遇到`]`时，取出两个栈顶的数字和字符串，然后去重复字符串。

当遇到字符时，就直接加入到currentStr中，currentStr中是累计的长字符，当遇到[时候要清空。

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

注意是while循环，而不是if，因为如果只处理了一个栈顶元素，如果当前温度比多个栈顶元素都大，就会漏处理。

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



## 堆

小根堆 根节点是左右两边最小值
一维数组可以存堆 左孩子2x 右孩子2x+1

| 插入一个数         | heap[++size] = x; up(size);                      |
| ------------------ | ------------------------------------------------ |
| 求集合当中的最小值 | heap[1];                                         |
| 删除最小值         | heap[1] = heap[size]; size --; down(1);          |
| 删除任意一个元素   | heap[k] = heap[size]; size --; down(k); / up(k); |
| 修改任意一个元素   | heap[k] = x; down(k); / up(k);                   |

```c++
const int N = 100010;
int h[N], n, m, siz;
void down(int u) {
	int t = u;
	if (u * 2 < siz && h[u * 2] < h[t]) t = u * 2;
	if (u * 2 + 1 < siz && h[u * 2 + 1] < h[t]) t = u * 2 + 1;
	if (u != t) {
		swap(h[u], h[t]);
		down(t);
	}
}
void up(int u) {
	while (u / 2 && h[u / 2] > h[u]) {
		swap(h[u / 2], h[u]);
		u /= 2;
	}
}
```



### 347. 数组中第k个最大元素

```c++
class Solution {
public:
    int siz, h[100010]; 
    void down(int u) {
        int t = u;
        if(u * 2 <= siz && h[u * 2] > h[t]) t = u * 2;
        if(u * 2 + 1 <= siz && h[u * 2 + 1] > h[t]) t = u * 2 + 1;
        if(t != u) {
            swap(h[u], h[t]);
            down(t);
        }
    }
    int findKthLargest(vector<int>& nums, int k) {
        siz = nums.size();
        for(int i = 0; i < siz; i ++) {
            h[i + 1] = nums[i];
        }
        for(int i = siz / 2; i; i --) {
            down(i);
        }
        while(-- k) {
            cout << h[1];
            h[1] = h[siz];
            siz --;
            down(1);
            
        }
        return h[1];
    }
};
```



### 347. 前k个高频元素

```c++
class Solution {
public:
    vector<int> topKFrequent(vector<int>& nums, int k) {
        unordered_map<int, int> mp;
        priority_queue<pair<int, int>> pq;
        vector<int> res;
        for(int num: nums) {
            mp[num] ++;
        }
        for(auto it = mp.begin(); it != mp.end(); it ++) {
            pq.push(pair<int, int>(it->second, it->first));
        }
        while(k --) {
            res.push_back(pq.top().second);
            pq.pop();
        }
        return res;
    }
};
```



### 295. 数据流的中位数

```c++
class MedianFinder {
public:
    // 默认是大根堆 min是大根堆（存储较小元素） max是小根堆（存储较大元素）
    priority_queue<int> min;
    priority_queue<int, vector<int>, greater<int>> max;
    MedianFinder() {}    
    void addNum(int num) {
        min.push(num);
        max.push(min.top());
        min.pop();
        if(max.size() > min.size()) {
            min.push(max.top());
            max.pop();
        }
    }
    double findMedian() {
        if(max.size() < min.size()) return min.top();
        return (min.top() + max.top()) / 2.0;
    }
};
```



## 贪心

### 121. 买卖股票的最佳时期

在最低点买入，然后在接下来的最高点抛出。

```c++
class Solution {
public:
    int maxProfit(vector<int>& prices) {
        int profit = 0, minVal = prices[0];
        for(int i = 0; i < prices.size(); i ++){
            if(minVal > prices[i]) minVal = prices[i];
            else if(profit < prices[i] - minVal) profit = prices[i] - minVal;
        }
        return profit;
    }
};
```



### 55. 跳跃游戏

```c++
class Solution {
public:
    bool canJump(vector<int>& nums) {
        int n = nums.size();
        int rightmax = 0;
        for(int i = 0; i < n; i ++) {
            // 检查当前位置i是否可以被到达
            if(rightmax >= i) {
                rightmax = max(rightmax, i + nums[i]);
                if(rightmax >= n - 1) return true;
            }
        }
        return false;
    }
};
```



### 45. 跳跃游戏Ⅱ

```c++
class Solution {
public:
    int jump(vector<int>& nums) {
        int n = nums.size(), end = 0, maxpos = 0, cnt = 0;
        for(int i = 0; i < n - 1; i ++)
        {
            if(maxpos >= i)
            {
                maxpos = max(maxpos, nums[i] + i);
                if(i == end)
                { // 到达边界 增加跳跃次数并且再次更新边界
                    cnt ++;
                    end = maxpos;
                }
            }
        }
        return cnt;
    }
};
```



### 763. 划分字母区间

记录每个字母最后一次出现的下标，当遍历到end的时候就划分出一个字符串。

```c++
class Solution {
public:
    vector<int> partitionLabels(string s) {
        int last[26];
        int length = s.size();
        for(int i = 0; i < length; i ++) {
            last[s[i] - 'a'] = i;
        }
        vector<int> partition;
        int start = 0, end = 0;
        for(int i = 0; i < length; i ++) {
            end = max(last[s[i] - 'a'], end);
            if(i == end) {
                partition.push_back(end - start + 1);
                start = end + 1;
            }
        }
        return partition;
    }
};
```



## 动态规划

### 118. 杨辉三角

`ret[i].resize(i + 1);`将每行的大小设为1 2 3 4 5...

初始化`ret[i][0] = ret[i][i] = 1;`三角形的两个边为1。

```c++
vector<vector<int>> generate(int numRows) {
    vector<vector<int>> ret(numRows);
    for(int i = 0; i < numRows; i ++) {
        ret[i].resize(i + 1);
        ret[i][0] = ret[i][i] = 1;
        for(int j = 1; j < i; j ++) {
            ret[i][j] = ret[i - 1][j] + ret[i - 1][j - 1];
        }
    }
    return ret;
}
```



### 198. 打家劫舍

`dp[1] = max(nums[0], nums[1]);`为什么要取max，是因为在只有两个房屋的时候，不能偷相邻的两个，所以只能偷价值最大的屋子。

无非是偷第1家，第3家，第5家还是2，4，6。

对应的状态转移方程就是dp[i - 1] 和 dp[i - 2] + nums[i]取谁价值大。 

```c++
class Solution {
public:
    int rob(vector<int>& nums) {
        int n = nums.size();
        if(n == 0) return 0;
        if(n == 1) return nums[0];
        vector<int> dp(n);
        dp[0] = nums[0], dp[1] = max(nums[0], nums[1]);
        for(int i = 2; i < n; i ++) {
            dp[i] = max(dp[i - 2] + nums[i], dp[i - 1]);
        }
        return dp[n - 1];
    }
};
```



### 279. 完全平方数

`f[i - j * j] + 1`

- `f[i - j * j]` 表示组成 `i - j * j` 所需的最少平方数。
- `+ 1` 是因为我们使用了 `j * j` 这个平方数。

```c++
class Solution {
public:
    int numSquares(int n) {
        // 初始设为一个n + 1不可能的大值
        vector<int> f(n + 1, n + 1);
        f[0] = 0;
        for(int i = 1; i <= n; i ++) {
            for(int j = 1; j * j <= i; j ++) {
                f[i] = min(f[i], f[i - j * j] + 1);
            }
        }
        return f[n];
    }
};
```



### 322. 零钱兑换

和上一道题完全平方数是一个思想。

```c++
class Solution {
public:
    int coinChange(vector<int>& coins, int amount) {
        int n = coins.size();
        vector<int> dp(amount + 1, amount + 1);
        dp[0] = 0;
        for(int i = 1; i <= amount; i ++) {
            for(int j = 0; j < n; j ++) {
                if(coins[j] <= i) {
                    dp[i] = min(dp[i], dp[i - coins[j]] + 1);
                }
            }
        }
        return dp[amount] > amount ? -1 : dp[amount];
    }
};
```



### *139. 单词划分

`dp[i]` 表示字符串 `s` 的前 `i` 个字符（即 `s[0..i-1]`）是否可以被拆分成字典中的单词

初始状态：`dp[0] = true`（空串可以被拆分）

内层循环遍历 `0` 到 `i-1`，检查所有可能的拆分点 `j`。

举例子s = "leetcode", wordDict = ["leet", "code"]

当j = 0时，i = 4时，dp[j] = true && wordset中存在切割字符串“leet”，所以dp[4] = true；

等到j = 4，i = 8时dp[j] = true && wordset中存在切割字符串“code”，所以dp[8] = true;

```c++
class Solution {
public:
    bool wordBreak(string s, vector<string>& wordDict) {
        unordered_set<string> wordSet(wordDict.begin(), wordDict.end());
        int n = s.size();
        vector<bool> dp(n + 1, false);
        dp[0] = true;
        for(int i = 1; i <= n; i ++) {
            for(int j = 0; j < i; j ++) {
                // 如果 s[0...j-1] 可以拆分，并且 s[j...i-1] 是字典里的单词，那我们就可以说 s[0...i-1] 也能拆分出来。
                if(dp[j] && wordSet.count(s.substr(j, i - j))) {
                    dp[i] = true;
                    break;
                }
            }
        }
        return dp[n];
    }
};
```



### 300. 最长递增子序列LIS

初始化dp(n, 1) 因为每个数字长度都是1。

每次遍历i的时候，站在i位置，判断i之前的元素是否都小于i，哪些可以接在i前面。

最后要取最大的dp

```c++
class Solution {
public:
    int lengthOfLIS(vector<int>& nums) {
        int n = nums.size();
        vector<int> dp(n, 1);
        dp[0] = 1;
        for (int i = 1; i < n; i++) {
            for (int j = 0; j < i; j++) {
                if (nums[j] < nums[i]) {
                    dp[i] = max(dp[i], dp[j] + 1);
                }
            }
        }
        return *max_element(dp.begin(), dp.end());
    }
};
```



### 152. 乘积最大子数组

因为有负数的存在，所以要维护两个数组，分别记录最小的值和最大的值。

而且需要res来记录当中的最大值，因为包含结尾数字的不一定是最大值，可能是中间的连续数字。

```c++
class Solution {
public:
    int maxProduct(vector<int>& nums) {
        int n = nums.size();
        vector<int> mindp(n), maxdp(n);
        mindp[0] = maxdp[0] = nums[0];
        int result = nums[0];
        for(int i = 1; i < n; i ++){
            mindp[i] = min({nums[i], mindp[i - 1] * nums[i], maxdp[i - 1] * nums[i]});
            maxdp[i] = max({nums[i], maxdp[i - 1] * nums[i], mindp[i - 1] * nums[i]});
            result = max(maxdp[i], result);
        }
        return result;
    }
};
```



### 416. 分割等和子集

我们选择的元素是否等于整个数组和的一半？

初始化一个布尔数组，长度为target + 1，表示和在0 - target之间的可能。

初始化dp[0] = true, 倒着从target开始循环。

```c++
class Solution {
public:
    bool canPartition(vector<int>& nums) {
        int sum = 0;
        int n = nums.size();
        for(int i = 0; i < n; i ++) sum += nums[i];
        if(sum % 2 != 0) return false;
        int target = sum / 2;
        vector<bool> dp(target + 1, false);
        dp[0] = true;
        for(int i = 0; i < n; i ++) {
            for(int j = target; j >= nums[i]; j --) {
                dp[j] = dp[j] || dp[j - nums[i]];
            }
        }
        return dp[target];
    }
};
```



### 32. 最长有效括号

*栈*：

对于遇到的每个 ‘(’ ，我们将它的下标放入栈中
对于遇到的每个 ‘)’ ，我们先弹出栈顶元素表示匹配了当前右括号：
如果栈为空，说明当前的右括号为没有被匹配的右括号，我们将其下标放入栈中来更新我们之前提到的「最后一个没有被匹配的右括号的下标」
如果栈不为空，当前右括号的下标减去栈顶元素即为「以该右括号为结尾的最长有效括号的长度」

```
class Solution {
public:
    int longestValidParentheses(string s) {
        int maxans = 0, n = s.length();
        stack<int> stk;
        stk.push(-1);
        for(int i = 0; i < n; i ++) {
            if(s[i] == '(') {
                stk.push(i);
            }
            else {
                stk.pop();
                if(stk.empty()) {
                    stk.push(i);
                }
                else {
                    maxans = max(maxans, i - stk.top());
                }
            }
        }
        return maxans;
    }
};
```



## 多维动态规划

### 62. 不同路径

```c++
int uniquePaths(int m, int n) {
    int dp[m][n];
    dp[0][0] = 1;
    for (int i = 0; i < m; i++) {
        dp[i][0] = 1;
    }
    for (int i = 0; i < n; i++) {
        dp[0][i] = 1;
    }
    for (int i = 1; i < m; i++) {
        for(int j = 1; j < n; j ++) {
            dp[i][j] = dp[i - 1][j] + dp[i][j - 1];
        }
    }
    return dp[m - 1][n - 1];
}
```



### 64. 最小路径和

和62不同路径这道题很像。不同的是需要初始化dp的第一行和第一列的累加和。

```c++
class Solution {
public:
    int minPathSum(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        int dp[m][n];
        dp[0][0] = grid[0][0];
        for(int i = 1; i < m; i ++) dp[i][0] = grid[i][0] + dp[i - 1][0];
        for(int i = 1; i < n; i ++) dp[0][i] = grid[0][i] + dp[0][i - 1];
        for(int i = 1; i < m; i ++) {
            for(int j = 1; j < n; j ++) {
                dp[i][j] = min(dp[i - 1][j] + grid[i][j], dp[i][j - 1] + grid[i][j]);
            }
        }
        return dp[m - 1][n - 1];
    }
};
```



### 5. 最长回文子串

初始化dp布尔数组，`dp[i][i] = true`，表示单个字符是回文。

双重循环，第一层循环遍历长度len，范围是[2, n]；第二层循环遍历起始位置i，范围是[0, n - len]

临时变量j为当前长度len的子串末尾，类似双指针。

分为情况讨论，当s[i] == s[j]时，如果len == 2则直接为true，如果大于2，则双指针移动，`dp[i][j] = dp[i + 1][j - 1]`；

如果s[i] != s[j]，则直接为false；

更新并记录下最大长度maxlen。

```c++
const int N = 1010;
bool dp[N][N];
class Solution {
public:
    string longestPalindrome(string s) {
        int n = s.length();
        if(n < 2) return s;
        int maxlen = 1, start = 0;
        for(int i = 0; i < n; i ++) dp[i][i] = true;
        for(int len = 2; len <= n; len ++) {
            for(int i = 0; i <= n - len; i ++) {
                int j = i + len - 1;
                if(s[i] == s[j]) {
                    if(len == 2) dp[i][j] = true;
                    else dp[i][j] = dp[i + 1][j - 1];
                }
                else {
                    dp[i][j] = false;
                }
                if(dp[i][j] && len > maxlen) {
                    maxlen = len;
                    start = i;
                }
            }
        }
        return s.substr(start, maxlen);
    }
};
```



### 1143. 最长公共子序列

`dp[i][j]`表示text1的前i个字符和text2的前j个字符的最长公共子序列长度。

如果两个字符相等，则长度 +1

如果当前字符不相等 要么丢掉text1当前字符 要么丢掉text2字符 看哪个长度更大。

```c++
const int N = 1010;
int dp[N][N];
class Solution {
public:
    int longestCommonSubsequence(string text1, string text2) {
        int n = text1.length(), m = text2.length();
        for(int i = 1; i <= n; i ++) {
            for(int j = 1; j <= m; j ++) {
                if(text1[i - 1] == text2[j - 1]) dp[i][j] = dp[i - 1][j - 1] + 1;
                else dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
        return dp[n][m];
    }
};
```



### 72. 编辑距离

这道题和上一道题最长公共子序列是类似的，都是遍历两个字符串，并且判断两个字符串的i - 1和j - 1位置是否相等。

**三种操作都是对word1操作**：插入字符、删除字符、替换字符。

**dp\[i][j] 表示将 word1 的前 i 个字符转换成 word2 的前 j 个字符的最小编辑距离。**

初始化dp数组，编辑距离`dp[i][0] = i; dp[0][j] = j;`

- 如果两个字符相等

  `dp[i][j] = dp[i - 1][j - 1]    // 无需操作`

- 如果两个字符不相等

  - 插入word2的一个字符

    `dp[i][j - 1] + 1`

  - 删除word1的一个字符

    `dp[i - 1][j] + 1`

  - 替换

    `dp[i - 1][j - 1] + 1`

```c++
const int N = 510;
int dp[N][N];
class Solution {
public:
    int minDistance(string word1, string word2) {
        int n = word1.size(), m = word2.size();
        for(int i = 0; i <= n; i ++) dp[i][0] = i;
        for(int j = 0; j <= m; j ++) dp[0][j] = j;
        for(int i = 1; i <= n; i ++) {
            for(int j = 1; j <= m; j ++) {
                if(word1[i - 1] == word2[j - 1]) dp[i][j] = dp[i - 1][j - 1];
                else
                    dp[i][j] = min({dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + 1});
            }
        }
        return dp[n][m];
    }
};
```



## 技巧

### 颜色分类（荷兰国旗）

*双指针 + 一趟扫描*

遍历数组，直到 `i > p2`：

- 如果 `nums[i] == 0`：与 `nums[p0]` 交换，然后 `p0++`, `i++`
- 如果 `nums[i] == 2`：与 `nums[p2]` 交换，然后 `p2--`（但 **i 不动**，因为换过来的元素还没处理）
- 如果 `nums[i] == 1`：直接 `i++`

```c++
class Solution {
public:
    void sortColors(vector<int>& nums) {
        int n = nums.size();
        int p0 = 0, p2 = n - 1;
        for(int i = 0; i <= p2; i ++) {
            while (nums[i] == 2 && i <= p2) {
                swap(nums[i], nums[p2]);
                p2 --;
            }
            if(nums[i] == 0) {
                swap(nums[i], nums[p0]);
                p0 ++;
            }
        }
    }
};
```



### 31. 下一个排列

举例子：158476531

先从后往前寻找第一个非降序的数字，找到了4。

然后再从后往前寻找第一个大于4的数字，找到了5。

然后交换他俩的位置，变成158576431。

最后再翻转5之后的数字（nums.begin() + i + 1），变成了158513467。

注意：要做边界检查，i--之后要判断是否i >= 0 因为有可能是1234这样的序列。

那为什么不用对j检查，因为当你从后往前找第一个非降序的数字也就是nums[i]，就意味着i的后方一定有数字是大于nums[i]的，不然也不会破坏降序这个规则。

```c++
class Solution {
public:
    void nextPermutation(vector<int>& nums) {
        int n = nums.size();
        int i = n - 2;
        while(i >= 0 && nums[i] >= nums[i + 1]) i --;
        if(i >= 0) {
            int j = n - 1;
            while(j >= 0 && nums[j] <= nums[i]) j --;
            swap(nums[i], nums[j]);
        }
        reverse(nums.begin() + i + 1, nums.end());
    }
};
```



### 287. 寻找重复数

与寻找环形链表入口结点相似，区别在于快慢指针的下一步是寻找下标为nums[i]的值。

`slow = nums[slow];`

`fast = nums[nums[fast]];`

```c++
class Solution {
public:
    int findDuplicate(vector<int>& nums) {
        int slow = 0, fast = 0;
        do {
            slow = nums[slow];
            fast = nums[nums[fast]];
        } while(fast != slow);
        fast = 0;
        while(slow != fast) {
            slow = nums[slow];
            fast = nums[fast];
        }
        return slow;
    }
};
```

