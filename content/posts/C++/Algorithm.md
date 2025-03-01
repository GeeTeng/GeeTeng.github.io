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

### JZ23链表中环的入口结点

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



### JZ24反转链表

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



### JZ25合并两个排序的链表

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

