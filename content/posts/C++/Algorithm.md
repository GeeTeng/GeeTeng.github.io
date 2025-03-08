---
title: "刷题笔记"
date: 2025-03-1
tags: [C++]
description: "练习算法题时整理的思路"
showDate: true
math: true
chordsheet: true

---

## 剑指offer

### JZ23 链表中环的入口结点

*方法一：哈希集合*

环的入口结点就是第一个在哈希集合中已存在的结点。

```c++
ListNode* EntryNodeOfLoop(ListNode* pHead) {
    unordered_set<ListNode*> st;
    while(pHead){
        if(st.count(pHead)) return pHead;
        st.insert(pHead);
        pHead = pHead->next;
    }
    return pHead;
}
```

*方法二：快慢指针*

1. 判断是否有环：快指针每次向后移动2位，慢指针每次向后移动1位，如果有环，两个指针必定会相遇；
2. 如果有环，就将快指针从头重新开始，每次移动1位，慢指针也每次移动1位，最终相遇的点就是环的入口结点。

```c++
ListNode* EntryNodeOfLoop(ListNode* pHead) {
    ListNode* fast = pHead, *slow = pHead;
    while(fast != nullptr){
        if(fast->next == nullptr) return nullptr;
        fast = fast->next->next;
        slow = slow->next;
        if(fast == slow){
            fast = pHead;
            while(fast != slow){
                fast = fast->next;
                slow = slow->next;
            }
            return fast;
        }
    }
    return nullptr;
}
```



### JZ24 反转链表

*迭代法:*

声明2个指针cur和pre，分别指向当前结点和前一个结点，同时用tmp来临时存储cur的后一个结点，以防cur在将next指针指向pre的时候断开链表。

```c++
ListNode* ReverseList(ListNode* head) {
    if(head == nullptr) return nullptr;
    ListNode* cur = head;
    ListNode* pre = nullptr;
    while(cur != nullptr){
        ListNode* tmp = cur->next;
        cur->next = pre; // 指向前一个
        pre = cur;
        cur = tmp;
    }
    return pre;
}
```



### JZ25 合并两个排序的链表

新建一个头节点，如果p1值更小，则连p1；如果p2小则连p2，不断更新下一位置。

如果其中一个遍历完，另外一个链表还有剩余节点，则全部接到res之后。

```c++
ListNode* Merge(ListNode* p1, ListNode* p2) {
    if(p1 == nullptr) return p2; // 一个空了返回另一个
    if(p2 == nullptr) return p1;
    ListNode* res = new ListNode(0);
    ListNode* cur = res;
    while(p1 && p2){
        if(p1->val < p2->val){
            cur->next = p1;
            p1 = p1->next;
        }
        else{
            cur->next = p2;
            p2 = p2->next;
        }
        cur = cur->next;
    }
    if(p1) cur->next = p1;
    else cur->next = p2;
    return res->next;
}
```



### JZ26 树的子结构

首先要判断pRoot1和pRoot2树是否为空，如果为空则返回false。在它俩都不为空的前提条件下去判断pRoot2是否为pRoot1的子树，首先去找pRoot1是否某个节点和pRoot2相等，如果存在的的话，就调用recursion函数，递归遍历判断是否完全匹配。

如果不完全匹配的话，就向下继续寻找，递归调用HasSubtree的左子树和右子树。

```c++
class Solution {
public:
    bool HasSubtree(TreeNode* pRoot1, TreeNode* pRoot2) {
		bool result = false;
		if(pRoot1 != nullptr && pRoot2 != nullptr){
			if(pRoot1->val == pRoot2->val)
				result = recursion(pRoot1, pRoot2);
            // 如果匹配失败就向下继续寻找
			if(!result)
			{
				result = HasSubtree(pRoot1->left, pRoot2) ||
				HasSubtree(pRoot1->right, pRoot2);
			}
		}
		return result;

    }
    // 当匹配到pRoo1的某个结点值与pRoot2根结点相等时，进行递归 判断是否完全匹配
	bool recursion(TreeNode* pRoot1, TreeNode* pRoot2){
		if(pRoot2 == nullptr) return true;
		if(pRoot1 == nullptr) return false;
		if(pRoot1->val != pRoot2->val) return false;
		return recursion(pRoot1->left, pRoot2->left) && recursion(pRoot1->right, pRoot2->right);
	}
};
```



### JZ33 二叉搜索树的后序遍历序列

写一个判断check函数，用来递归判断子树是否符合后序遍历序列，传递sequence的左右位置。

空树的时候返回false；

首先从后往前找右子树的结束位置（左右子树的分界线），然后再从前往后遍历判断左子树中是否有大于根节点的值，如果有说明不符合后序遍历；否则就继续递归它的左右子树。

```c++
#include <vector>
class Solution {
public:
    bool VerifySquenceOfBST(vector<int> sequence) {
        if(sequence.size() == 0) return false;
        return check(sequence, 0, sequence.size() - 1);
    }

    bool check(vector<int>& sequence, int l, int r){
        if(l >= r) return true; // 如果只剩下1个结点
        int root = sequence[r];
        int j = r - 1;
        while(j >= 0 && sequence[j] > root) j --;
        for(int i = l; i <= j; i ++){
            if(sequence[i] > root) return false;
        }
        return check(sequence, l, j) && check(sequence, j + 1, r - 1);
    }
};
```



### JZ35 复杂链表的复制

将原链表的结点对应的拷贝结点连在其后，链表从A - B - C变成A - A' - B - B' - C - C'。

然后再去连接random指针；

最后将原链表和复制的链表拆分，各自指向各自的下一节点。

```c++
public:
    RandomListNode* Clone(RandomListNode* pHead) {
        if(!pHead) return pHead;
        RandomListNode* cur = pHead;
        while (cur) {
            RandomListNode* tmp = new RandomListNode(cur->label);
            tmp->next = cur->next;
            cur->next = tmp;
            cur = tmp->next;
        }
        RandomListNode *old = pHead, *clone = pHead->next, *res = pHead->next;
        while (old) {
            clone->random = old->random == nullptr ? nullptr : old->random->next;
            if(old->next) old = old->next->next;
            if(clone->next) clone = clone->next->next;
        }
        old = pHead, clone = pHead->next;
        while(old){
            if(old->next) old->next = old->next->next;
            if(clone->next) clone->next = clone->next->next;
            old = old->next;
            clone = clone->next;
        }
        return res;
    }
};
```



### JZ36 二叉搜索树与双向链表

举例子将二叉搜索树 1 - 2 - 3 变成双向链表，实际上是二叉树的中序遍历。

先找到最左的叶节点Convert(1)，然后递归调用返回

Convert(2)时pre = 1， pRootOfTree = 2；

Convert(3)时pre = 2，pRootOfTree = 2；最后回溯到Convert(2)。

```c++
class Solution {
public:
	TreeNode* head = nullptr;
	TreeNode* pre = nullptr;
    TreeNode* Convert(TreeNode* pRootOfTree) {
		if(pRootOfTree == nullptr) return nullptr;
		Convert(pRootOfTree->left);
		if (pre == nullptr) {
			head = pRootOfTree;
			pre = pRootOfTree;
		}
		else {
			pre->right = pRootOfTree; // 1->right = 2 | 2->right = 3
			pRootOfTree->left = pre; // 2->left = 1 | 3->left = 2
			pre = pRootOfTree; // pre = 2 | pre = 3
		}
		Convert(pRootOfTree->right);
		return head;
    }
};
```



### JZ38 字符串的排列

dfs 回溯方法，在原字符串的基础上交换，然后再交换回来。

```c++
    void dfs(string &s, int index, vector<string> &res) {
        if(index == s.size() - 1) {
            res.push_back(s);
            return;
        }
        unordered_set<char> vis;
        for(int i = index; i < s.size(); i ++) {
            if(!vis.count(s[i])){
                vis.insert(s[i]);
                swap(s[i], s[index]);
                dfs(s, index + 1, res);
                swap(s[i], s[index]);
            }
        }
    }

    vector<string> Permutation(string str) {
        vector<string> res;
        dfs(str, 0, res);
        return res;
    }
```



### JZ39 数组中出现次数超过一半的数字

*摩尔投票法步骤*

1. 候选者 candidate：初始化为数组的第一个元素
2. 计数器 count：初始化为 1
3. 遍历数组：
   - 当前元素等于 candidate：计数 `+1`
   - 当前元素不等于 candidate：计数 `-1`
   - 当计数变为 0：更换候选者，并将 `count` 设为 1
4. 最终的 candidate 就是超过一半的元素（在题目保证存在的情况下）

```c++
    int MoreThanHalfNum_Solution(vector<int>& numbers) {
        int candidate = numbers[0], count = 1;
        for(int i = 1; i < numbers.size(); i ++) {
            if(numbers[i] == candidate) {
                count ++;
            }
            else {
                count --;
                if(count == 0) {
                    candidate = numbers[i];
                    count = 1;
                }
            }
        }
        return candidate;
    }
```



### JZ41 数据流中的中位数

*堆排序（双堆法）*

使用**最大堆 + 最小堆**的方式：

- **最大堆min**（存储较小的一半数据，堆顶是最大值）
- **最小堆max**（存储较大的一半数据，堆顶是最小值）

需要不断平衡两个堆的数量（min永远不会比max元素少），奇数个就在min堆里，偶数就通过求两个堆的堆顶元素就可以得到中位数。

```c++
public:
    priority_queue<int> min;
    priority_queue<int, vector<int>, greater<int>> max; 
    void Insert(int num) {
        // 先加入到min中，取出最大值加入max
        min.push(num);
        max.push(min.top());
        min.pop();
        if(min.size() < max.size()) {
            min.push(max.top());
            max.pop();
        }
    }

    double GetMedian() { 
        // 奇数个
        if(min.size() > max.size()) return double(min.top());
        else return double (min.top() + max.top()) / 2;
    }
```



### JZ43 整数中1出现的次数

*1.如何获取每一位数字的 左边数字 和 右边数字？*

  数字3101592，假如现在cur = 0，base = 10000（是当前考虑的位数）， high是cur左边的部分、cur是当前位的数字、low是cur右边的部分。

  high = n / （base * 10） = n / 100000 = 31

​    cur = (n / base) % 10 = (n / 10000) % 10 = 0

​    low = n % base = n % 10000 = 1592

*2.出现的次数取决于小于n的那些数，分情况讨论。*

计算1出现的次数需要计算所有位上1出现的次数的加和，所以要遍历每一位，不断更新当前位前后的数字是什么。假设当前位是1时，计算1出现的次数。

- **当cur = 0时**

还是拿31 0 1592举例子，如果当前位出现1了，那必然是high是0-30区间内，因为如果是311开头就比n要大了。当high是0-30的时候，无论low怎么选都比n小，所以low的选法可以有0-9999种，而low的选法正好=base10000。将high * base就是这种情况下1出现的次数。

- **当cur = 1时**

cur = 1时 比如说310 1 592，需要分类讨论：

​        high从0 - 309时，low可以是0-999。所以是和cur = 0的情况一样的 => high * base

​        high是310时（1种选法），low只能是0 - 592（593种选法，是low + 1），low如果再大于592就比n要大了。 => 1 * （low + 1）

所以1的出现次数是（high * base） + （low + 1）

- **当cur > 1时**

cur大于1时，不需要再去考虑high和low怎么选不会比n大了。比如3101 5 92，high可以从 0 - 3101 而 low 可以从 0 - 99，所以直接就是 （high + 1） * base。

```c++
int NumberOf1Between1AndN_Solution(int n) {
    int base = 1;
    int cnt = 0;
    while(base <= n) {
        int high = n / (base * 10);
        int low = n % base;
        int cur = (n / base) % 10;
        if(cur == 0) {
            cnt += high * base;
        }
        else if(cur == 1) {
            cnt += (high * base) +  (low + 1);
        }
        else {
            cnt += (high + 1) * base;
        }
        base *= 10;
    }
    return cnt;
}
```



### JZ44 数字序列中某一位的数字

*位数减法*：

​	1-9 有9个数字

​	10-99 有90个数字

​	100-999 有900个数字

*举例子：*n = 327，第一轮循环digit = 1， base是开始数字 = 1， sum = 9 * digit * base = 9， n -= 9 = 318；第二轮循环digit = 2，base = 10，sum = 180，n = 138；第三轮循环 n  <  2700(sum在循环里的更新)，所以停止循环，但是我们确定了n是三位数。

然后确认是哪一个数字，且是该数字的哪一位。

```c++
int findNthDigit(int n) {
    long long base = 1;
    long long digit = 1; // 数字的位数
    long long sum = 9; // 当前位数的总位数 9 180 2700
    while(n > sum) {
        n -= sum;
        digit ++;
        base *= 10;
        sum = 9 * digit * base;
    }
    // 找到是哪个数字 n这个时候是n减去从该位数开始的数字
    int num = base + (n - 1) / digit;
    // 求数字的某一位是多少
    int index = (n - 1) % digit;
    return to_string(num)[index] - '0';
}
```



### JZ45 把数组排成最小的数

 *重载比较排序*

```c++
static bool cmp(string &x, string &y) {
    return x + y < y + x;
}

string PrintMinNumber(vector<int>& numbers) {
    string res = "";
    if(numbers.size() == 0) return res;
    vector<string> nums;
    for(int i = 0; i < numbers.size(); i ++) {
        nums.push_back(to_string(numbers[i]));
    }
    sort(nums.begin(), nums.end(), cmp);
    for(int i = 0; i < nums.size(); i ++) {
        res += nums[i];
    }
    return res;
}
```



### JZ46 把数字翻译成字符串

*动态规划*

首先排除一些特殊情况：

1. 当字符串是0时，没有对应编码
2. 当字符串是10或20时，对应1种编码
3. 当字符串当中某个数字是0，但是这个数字的前一位不是1或0。比如说是30 40等，则没有对应编码。

其次对字符串进行动态规划

如果在11-19和21-26之间，则会有2种编码方式，dp[i] = dp[i - 1] + dp[i - 2]

如果是在1-10之间和20时，则只有1种编码方式，dp[i] = dp[i - 1]

```c++
int solve(string nums) {
    // 如果是0就返回0种编码，是10 20则只有一种编码，如果是0且前面不是1或2则是不规则编码。
    if(nums == "0") return 0;
    if(nums == "10" || nums == "20") return 1;
    for(int i = 1; i < nums.length(); i ++) {
        if(nums[i] == '0' && nums[i - 1] != '1' && nums[i - 1] != '2')
                return 0;
    }
    // 动态规划 初始dp为1
    vector<int> dp(nums.length() + 1, 1);
    for(int i = 2; i <= nums.length(); i ++) {   
        // 在11 - 19 和 21 - 26的情况
        if(nums[i - 2] == '1' && nums[i - 1] != '0' || (nums[i - 2] == '2' && nums[i - 1] > '0' && nums[i - 1] < '7')) {
            dp[i] = dp[i - 1] + dp[i - 2];
        }
        // 1 - 10和20情况
        else {
            dp[i] = dp[i - 1];
        }
    }
    return dp[nums.length()];
}
```



### JZ48 最长不含重复字符的字符串

*哈希 双指针*：滑动窗口思想。

r在每次循环中向右移动遍历字符串，每次记录在哈希表里。当遇到重复字符时，l也开始向右移动，直到重复字符消失。这个时候取现有子串和之前字串长度的最大值。

```c++
int lengthOfLongestSubstring(string s) {
    unordered_map<char, int> mp;
    int res = 0;
    for(int l = 0, r = 0; r < s.length(); r ++)
    {
        mp[s[r]] ++;
        while(mp[s[r]] > 1)
        {
            mp[s[l]] --;
            l ++;
        }
        res = max(r - l + 1, res);
    }
    return res;
}
```



### JZ49 丑数

把只包含质因子2、3和5的数称作丑数，有了定义我们就可以知道，**丑数的形式就是2^x 3^y 5^z**。

定义一个res数组存储丑数。已知第一个丑数是1，那么**根据公式乘2、3、5，可以得到之后一系列丑数**。

1. *最小堆法*

```c++
int GetUglyNumber_Solution(int index) {
    if(index == 0) return 0;
    unordered_set<long> mp;
    priority_queue<long, vector<long>, greater<long>> pq;
    mp.insert(1);
    pq.push(1);
    long ugly = 1;
    vector<int> primes = {2, 3, 5};
    for(int i = 1; i <= index; i ++) {
        ugly = pq.top();
        pq.pop();
        for(int prime : primes) {
            long newUgly = ugly * prime;
            if(!mp.count(newUgly)) {
                pq.push(newUgly);
                mp.insert(newUgly);
            }
        }
    }
    return ugly;
}
```

2. *动态规划*

```c++
int GetUglyNumber_Solution(int index) {
    if(index == 0) return 0;
    vector<int> dp(index, 1);
    int p2 = 0, p3 = 0, p5 = 0;
    for(int i = 1; i < index; i ++) {
        int nextUgly = min({dp[p2] * 2, dp[p3] * 3, dp[p5] * 5});
        dp[i] = nextUgly;
        if(nextUgly == dp[p2] * 2) p2 ++;
        if(nextUgly == dp[p3] * 3) p3 ++;
        if(nextUgly == dp[p5] * 5) p5 ++;
    }
    return dp[index - 1];
}
```



### JZ51 数组中的逆序对

*通过归并排序统计逆序对的方法*：

左半部分 [l, mid] 已经是递增序列

右半部分 [mid+1, r]也是递增序列

但是当它们合并在一起时可能会形成逆序对，i在左半部分，j在右半部分。

nums[j] < nums[i]时，nums[j]肯定大于nums[i]之前的所有数（否则早就加到tmp里了，也不会等到现在）。这个时候意味着，nums[i + 1], nums[i + 2], nums[i + 3]......都大于nums[j] (因为左半部分递增，nums[i]你都不大于，剩下的咋可能大于)，但是同时nums[j]是比右半部分的nums[j - 1], nums[j - 2]...那些大（也因为右半部分递增）。所以逆序对的数量就是 i 到 mid 中间那些大于j的数，包括i和mid，所以是i - mid + 1。

动笔画一下就明白了。

```c++
const int MOD = 1e9 + 7;
vector<int> tmp;
long long mergesort(vector<int>& nums, int l, int r) {
    if(l >= r) return 0;
    int mid = l + r >> 1;
    long long res = mergesort(nums, l, mid) + mergesort(nums, mid + 1, r);
    int k = 0, i = l, j = mid + 1;
    while(i <= mid && j <= r) {
        if(nums[i] <= nums[j]) tmp[k ++] = nums[i ++];
        else {
            tmp[k ++] = nums[j ++]; 
            res = (res + mid - i + 1) % MOD;
        }
    }
    while(i <= mid) tmp[k ++] = nums[i ++];
    while(j <= r) tmp[k ++] = nums[j ++];
    for(int i = l, j = 0; i <= r; i ++, j ++) nums[i] = tmp[j];
    return res;
}

int InversePairs(vector<int>& nums) {
    tmp.resize(nums.size());
    return mergesort(nums, 0, nums.size() - 1);
}
```



### JZ52 两个链表的第一个公共结点

两个指针p1，p2分别在两条链表上同时走，如果它们到达尾部则指向另一条链表继续走，当双指针相遇时则是它们第一个公共节点。如果没有公共结点也会同时走到nullptr。

```c++
ListNode* FindFirstCommonNode( ListNode* pHead1, ListNode* pHead2) {
    ListNode *p1 = pHead1, *p2 = pHead2;
    while(p1 != p2) {
        p1 = p1 ? p1->next : pHead2;
        p2 = p2 ? p2->next : pHead1;
    }
    return p1;
}
```



### JZ54 二叉搜索树的第k个结点

中序排列，不断k --，k=0时返回。

```c++
#include<iostream>

using namespace std;

struct TreeNode
{
	int val;
	struct TreeNode* left;
	struct TreeNode* right;
	TreeNode(int x) :val(x), left(nullptr), right(nullptr) {}
};

TreeNode* insertBST(TreeNode* root, int val) {
	if (!root) return new TreeNode(val);
	if (val < root->val) root->left = insertBST(root->left, val);
	else root->right = insertBST(root->right, val);
	return root;
}

void MidOrder(TreeNode* root, int& k, int& res) {
	if (root == nullptr || k <= 0) return;
	MidOrder(root->left, k, res);
	k--;
	if (k == 0) {
		res = root->val;
		return;
	}
	MidOrder(root->right, k, res);
}

int main() {
	int n;
	TreeNode* root = nullptr;
	while (cin >> n) {
		root = insertBST(root, n);
		if (cin.get() == '\n') break;
	}
	int k;
	cin >> k;
	int res = 0;
	MidOrder(root, k, res);
	cout << res;
	return 0;
}
```



### JZ56 数组中只出现一次的两个数字

异或运算，两个数一样异或结果为0，不一样则为1。

所以如果一个数组中有两个只出现一次的数字，那么这两个数一定二进制中有一位为1。比如说a的二进制有一位是1，b的二进制那一位是0，所以按这一位来分组。

首先把所有数异或，得到两个数不同的那几位的二进制，我们称为tmp。

然后遍历数组，和tmp相同的分为一组，和tmp不同的为一组。这样刚好就给两个只出现过一次的数字分开了，并且也把出现两次的数字也分到一组了（因为出现两次 => 二进制相同 => &tmp一定相同）。

```c++
#include<iostream>
using namespace std;

const int N = 1010;
int a[N], res[N];
int n;

void findNumAppearOnce(int* arr, int& res1, int& res2) {
	int tmp = arr[0];
	for (int i = 1; i < n; i++) {
		tmp ^= arr[i];
	}
	int k = 1;
	while ((tmp & k) == 0) k <<= 1;
	for (int i = 0; i < n; i++) {
		if (a[i] & k) res1 ^= arr[i];
		else res2 ^= arr[i];
	}
}

int main() {
	cin >> n;
	for (int i = 0; i < n; i++) {
		cin >> a[i];
	}
	int res1 = 0, res2 = 0;
	findNumAppearOnce(a, res1, res2);
	cout << res1 << " " << res2;
	return 0;
}
```



### JZ61 扑克牌顺子

先排序从大到小，然后计算有多少个0可以转换。

计算一共需要多少距离，即相邻两个数字之间相差多少个0才可以补成顺子。最后进行判断。

```c++
bool IsContinuous(vector<int>& numbers) {
    sort(numbers.begin(), numbers.end());
    int zeronum = 0;
    while(numbers[zeronum] == 0) zeronum ++;
    int distance = 0;
    for(int i = zeronum; i < numbers.size() - 1; i ++) { // 注意不要越界
        if(numbers[i + 1] == numbers[i]) return false; //如果遇到相等数字则不是顺子
        distance += numbers[i + 1] - numbers[i] - 1; 
    }
    if(distance <= zeronum) return true;
    return false;
}
```



### JZ62 孩子们的游戏（圆圈中最后剩下的数）

*递归*

n个数去掉第m位时，还剩下n - 1个数，但是m不变。所以从(n,m)的问题变成了(n−1,m)的子问题。

其中若是(n−1,m)的子问题返回的最后一个数是x，则(n,m)返回的结果就是(m+x)%n。

```c++
int LastRemaining_Solution(int n, int m) {
    if(n == 0 || m == 0) return 0;
    if(n == 1) return 0;
    int x = LastRemaining_Solution(n - 1, m);
    return (m + x) % n;
}
```



### JZ65 不用加减乘除做加法

*位运算非递归*

先异或，再与并向右移位，再更新sum。

```c++
int Add(int num1, int num2) {
    int add = num2;
    int sum = num1;
    while(add != 0) {
        int tmp = sum ^ add;
        add = (add & sum) << 1;
        // 更新sum为新的和
        sum = tmp;
    }
    return sum;
}
```



### JZ66 构建乘积数组

B[i]该位置 = B左边 * B右边，右边需要从右向左累乘，左边需要从左向右累乘。

```c++
vector<int> multiply(vector<int>& A) {
    vector<int> B(A.size(), 1);
    // 从左到右累乘
    for(int i = 1; i < A.size(); i ++) {
        B[i] = B[i - 1] * A[i - 1];
    }
    int tmp = 1;
    // 从右向左累乘
    for(int i = A.size() - 1; i >= 0; i --) {
        B[i] *= tmp;
        tmp *= A[i];
    }
    return B;
}
```



### JZ68 二叉搜索树的最近公共祖先

```c++
int lowestCommonAncestor(TreeNode* root, int p, int q) {
    if(root == nullptr) return -1;
    if((p >= root->val && q <= root->val) || (p <= root->val && q >= root->val))
        return root->val;
    else if(p <= root->val && q <= root->val)
        return lowestCommonAncestor(root->left, p, q);
    else
        return lowestCommonAncestor(root->right, p, q);
}
```



### JZ69 跳台阶

*方法一：递归 和斐波那契数列一样做法*

```c++
int jumpFloor(int number) {
    if(number <= 1) return 1;
    return jumpFloor(number - 1) + jumpFloor(number - 2);
}
```

*方法二：记忆化搜索*

由于存在很多重复的计算，所以用f数组来存储计算过的数值。

```c++
int f[50]{0};
int jumpFloor(int number) {
    if(number <= 1) return 1;
    if(f[number] > 0) return f[number];
    return f[number] = jumpFloor(number - 1) + jumpFloor(number - 2);
}
```

*方法三：动态规划*

```c++
int dp[50]{0};
int jumpFloor(int number) {
    dp[0] = dp[1] = 1;
    for(int i = 2; i <= number; i ++) {
        dp[i] = dp[i - 1] + dp[i - 2];
    }
    return dp[number];
}
```

继续优化

```c++
    int jumpFloor(int number) {
        int a = 1, b = 1, c = 1;
        for(int i = 2; i <= number; i ++) {
            c = b + a, a = b; b = c;
        }
        return c;
    }
```

**跳台阶问题和矩形覆盖问题一样的解法**。都是dp[i - 1] + dp[i - 2];

*跳台阶扩展问题：*

f(n) = f(n - 1) + f(n - 2) + ... + f(0)

f(n - 1) = f(n - 2) + ... + f(0)

所以f(n) = 2 * f(n - 1)



### JZ74 和为S的连续正数序列

*滑动窗口*

```c++
vector<vector<int> > FindContinuousSequence(int sum) {
    vector<vector<int>> res;
    vector<int> tmp;
    for(int l = 1, r = 2; l < r; ) {
        int sum1 = (l + r) * (r - l + 1) / 2;
        if(sum1 == sum) {
            tmp.clear();
            for(int i = l; i <= r; i ++) {
                tmp.push_back(i);
            }
            res.push_back(tmp);
            l ++;
        }
        else if(sum1 < sum) r ++;
        else l ++;
    }
    return res;
}
```



### JZ75 删除链表中的重复节点

由于是排序链表，所以用哈希表存储，但值＞1时，cur后续结点肯定是重复的。

```c++
#include<iostream>
#include<unordered_map>
using namespace std;

struct ListNode
{
	int val;
	ListNode* next;
	ListNode(int x) :val(x),next(nullptr){ }
};

ListNode* insertList(ListNode* &head, int val) {
	ListNode* newNode = new ListNode(val);
	if (head == nullptr) return newNode;
	ListNode* cur = head;
	while (cur->next != nullptr) {
		cur = cur->next;
	}
	cur->next = newNode;
	return head;
}

ListNode* deleteDuplication(ListNode* head) {
	unordered_map<int, int> mp;
	ListNode* cur = head;
	while (cur != nullptr) {
		mp[cur->val]++;
		cur = cur->next;
	}
	ListNode* res = new ListNode(0);
	res->next = head;
	cur = res;
	while (cur->next != nullptr) {
		if (mp[cur->next->val] != 1) cur->next = cur->next->next;
		else cur = cur->next;
	}
	return res->next;
}
void printList(ListNode* head) {
	ListNode* cur = head;
	while (cur != nullptr) {
		cout << cur->val << ' ';
		cur = cur->next;
	}
}

int main() {
	int val;
	ListNode* head = nullptr;
	while (cin >> val) {
		head = insertList(head, val);
		if (cin.get() != ' ') break;
	}
	ListNode* res = deleteDuplication(head);
	printList(res);
	return 0;
}
```

