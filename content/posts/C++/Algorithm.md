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

**方法一：哈希集合**

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

**方法二：快慢指针**

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

**迭代法:**

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

**摩尔投票法步骤**

1. **候选者 candidate**：初始化为数组的第一个元素
2. **计数器 count**：初始化为 1
3. 遍历数组：
   - **当前元素等于 candidate**：计数 `+1`
   - **当前元素不等于 candidate**：计数 `-1`
   - **当计数变为 0**：更换候选者，并将 `count` 设为 1
4. **最终的 candidate 就是超过一半的元素**（在题目保证存在的情况下）

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

