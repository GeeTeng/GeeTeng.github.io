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



### 3169. 无需开会的工作日

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



### 66. 加一

给定一个表示 **大整数** 的整数数组 `digits`，其中 `digits[i]` 是整数的第 `i` 位数字。这些数字按从左到右，从最高位到最低位排列。这个大整数不包含任何前导 `0`。

将大整数加 1，并返回结果的数字数组。

**示例 1：**

> 输入：digits = [1,2,3]
> 输出：[1,2,4]
> 解释：输入数组表示数字 123。
> 加 1 后得到 123 + 1 = 124。
> 因此，结果应该是 [1,2,4]。

一开始使用数组转为数字，发现大整数溢出问题。因此要在数组原地操作。

当数字为9的时候，它之后+1就一定会变为0，前面的数字如果还是9那么需要继续向前查找，直到有一个数字不为9。

比如说123999，直接将3变为4，后面的数字都变为0就可以得到答案；当然需要考虑到如果是9的话，数组需要新增一位。

```c++
class Solution {
public:
    vector<int> plusOne(vector<int>& digits) {
        int n = digits.size();
        for(int i = n - 1; i >= 0; i --) {
            if(digits[i] != 9) {
                digits[i] += 1;
                for(int j = i + 1; j < n; j ++) {
                    digits[j] = 0;
                }
                return digits;
            }
        }
        vector<int> res(n + 1);
        res[0] = 1;
        return res;
    }
};
```



### 判断能否形成等差数列

如果能形成等差数列，那么：
$$
d= \frac{max - min}{n - 1}
$$
因此d只能为整数，并且`(x - min) % d != 0` → false，下标 `(x - min) / d` 已出现 → false。

```c++
class Solution {
public:
    bool canMakeArithmeticProgression(vector<int>& arr) {
        int n = arr.size();
        int minnum = 1e6, maxnum = -1e6;
        for(int i = 0; i < n; i ++) {
            if(minnum > arr[i]) minnum = arr[i];
            if(maxnum < arr[i]) maxnum = arr[i];
        }
        if((maxnum - minnum) % (n - 1)) return false;
        int d = (maxnum - minnum) / (n - 1);
        if(d == 0) return true;
        vector<bool> seen(n, false);
        for(int i = 0; i < n; i ++) {
            int diff = arr[i] - minnum;
            if(diff % d) return false;
            int idx = diff / d;
            if(seen[idx]) return false;
            seen[idx] = true;
        }
        return true;
    }
};
```



### 回文数（不转换字符串）

反转数字的一半，举例子12321。

通过x % 10能得到末尾的1，x / 10得到还剩下的数字也就是1232。

接下来反转的数字需要不断 * 10 + 当前位2，也就是1 * 10 + 2 = 12。

当x大于反转的数字，比如说x现在为123 反转的数字为12。接下来继续

反转数字变成123，x为12。所以奇数就判断反转数字/10是否=x，偶数就是反转数字是否=x。

```c++
class Solution {
public:
    bool isPalindrome(int x) {
        if(x < 0 || x != 0 && x % 10 == 0) return false;
        int revertNum = 0;
        while(x > revertNum) {
            revertNum = revertNum * 10 + x % 10;
            x /= 10;
        }
        return revertNum == x || revertNum / 10 == x;
    }
};
```



### 可被K整除的最小整数

真正在枚举的不是数字，而是余数`r = (111…1) % K`

因此`r ∈ {0, 1, 2, ..., K-1}`一共有k种可能性

每一次for循环n就相当于r，超出k种可能性就会无限循环-》无解

```c++
class Solution {
public:
    int smallestRepunitDivByK(int k) {
        if(k % 2 == 0 || k % 5 == 0) return -1;
        int n = 1;
        for(int len = 1; len <= k; len ++) {
            if(n % k == 0) return len;
            n = (n * 10 + 1) % k;
        }
        return -1;
    }
};
```



### 排列序列

输入：n = 4, k = 9
输出："2314"

它的cnt是(n - 1)! = 6代表第一位选定后有6个组合，k-- / 6 = 1，则第一个数字为1234数组中的第1个数字（从0开始）则是2。需要从数组中删掉2元素。k = 8 % 6 = 2

第二个数字需要从1 3 4 里面选，那么cnt就是 （3-1）! = 2 代表第二位数字固定后有2种组合，那么k现在是2，因此round = k / cnt = 1，需要选择3并且从数组删掉。此时k = 2 % 2= 0

选第三个数字时cnt = 1， round = k / cnt = 0，则选定1。最后选择4。

```c++
class Solution {
public:
    string getPermutation(int n, int k) {
        string s;
        vector<int> nums;
        for(int i = 1; i <= n; i ++) nums.push_back(i);

        k --;
        int tmp = n;
        for (int i = 0; i < n; i++) {
            int cnt = 1;
            for (int j = 1; j < tmp; j++) {
                cnt *= j;
            }
            int round = k / cnt;
            k %= cnt;
            // 排序找第round个元素 添加到s中
            s.push_back(nums[round] + '0');
            nums.erase(nums.begin() + round);
            tmp --;
        }
        return s;
    }
};
```





---

### 快速排序

```c++
void quickSort(int q[], int l, int r){
    if(r <= l) return;
    int tmp = q[l + r >> 1];
    int i = l - 1, j = r + 1;
    while(i < j){
        do i ++; while(tmp > q[i]);
        do j --; while(tmp < q[j]);
        if(i < j) swap(q[i], q[j]);
    }
    quickSort(q, l, j);
    quickSort(q, j + 1, r);
}
```



### 直接插入排序

找到比前一个元素小的元素，然后用key（有哨兵的改版）存储起来，之后向前去将每个元素往后移动一位，直到找到适合它的大小的位置，将key放入进去。

```c++
#include<iostream>
using namespace std;

const int N = 1010;
int nums[N];

void insertSort(int nums[], int n){
    int i, j, key;
    for(i = 1; i < n; i ++) {
        if(nums[i] < nums[i - 1]){
            key = nums[i];
            j = i - 1;
            while(j >= 0 && nums[j] > key){
                nums[j + 1] = nums[j];
                j --;
            }
            nums[j + 1] = key;
        }
    }
}

int main(){
    int n;
    cin >> n;
    for(int i = 0; i < n; i ++){
        cin >> nums[i];
    }
    insertSort(nums, n);
    for(int i = 0; i < n; i ++){
        cout << nums[i] << ' ';
    }
    return 0;
}
```



### 选择排序

每次选择最小的元素放在第0个位置、第2个位置, ...., 第n个位置。i是位置，j查找最小元素。

```c++
void chooseSort(int nums[], int n){
    for(int i = 0; i < n; i ++){
        int minIdx = i;
        for(int j = i + 1; j < n; j ++){
            if(nums[j] < nums[minIdx]) minIdx = j;
        }
        if(minIdx != i) swap(nums[i], nums[minIdx]);
    }
}
```



### 冒泡排序

每一轮都把最大的元素冒到最后，因此 j 遍历是在n - i - 1的地方。

i = 0时，固定n - 1位的最大元素

i = 1时，固定n - 2位的最大元素

i = n - 1时，固定第0位最大元素。

```c++
void BubbleSort(int nums[], int n){
    for(int i = 0; i < n; i ++){
        bool swapped = false;
        for(int j = 0; j < n - i - 1; j ++){
            if(nums[j] > nums[j + 1]){
                swap(nums[j], nums[j + 1]);
                swapped = true;
            }
        }
        if(!swapped) return; 
    }
}
```



### 归并排序

切记tmp存储每次归并的结果，再还原到q数组中[l,r]位置上。

```c++
#include<iostream>
using namespace std;
const int N = 1e6 + 10;
int q[N],tmp[N];
int n;

void merge(int q[], int l, int r){
    if(l >= r) return;
    int mid = l + r >> 1;
    int i = l, j = mid + 1, k = 0;
    merge(q, l, mid), merge(q, mid + 1, r);
    while(i <= mid && j <= r){
        if(q[i] < q[j]) tmp[k ++] = q[i ++];
        else tmp[k ++] = q[j ++];
    }
    while(i <= mid) tmp[k ++] = q[i ++];
    while(j <= r) tmp[k ++] = q[j ++];
    for(int i = l, j = 0; i <= r; i ++, j ++) q[i] = tmp[j];
}

int main(){
    scanf("%d", &n);
    int i;
    for ( i = 0; i < n; i++)
    {
        scanf("%d", &q[i]);
    }
    merge(q, 0, n - 1);
    for ( i = 0; i < n; i++)
    {
        printf("%d ", q[i]);
    }
    return 0;
}
```



### 堆排序

```
#include<iostream>
using namespace std;
const int N = 1e6 + 10;
int n, m;
int h[N], siz;

void down(int u){
    int t = u;
    if(u * 2 <= siz && h[u * 2] < h[t]) t = u * 2;
    if(u * 2 + 1 <= siz && h[u * 2 + 1] < h[t]) t = u * 2 + 1;
    if(u != t) {
        swap(h[u], h[t]);
        down(t);
    }
}

int main(){
    cin >> n >> m;
    for(int i = 1; i <= n; i ++){
        cin >> h[i];
    }
    siz = n;
    for(int i = n / 2; i; i --) down(i);
    while(m --){
        cout << h[1] << ' ';
        h[1] = h[siz];
        siz --;
        down(1);
    }
    return 0;
}
```

