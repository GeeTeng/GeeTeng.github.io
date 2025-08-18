#include<iostream>
using namespace std;

template<typename T>
class Node {
public:
	T val;
	Node<T>* next;
	Node(const T& value) :val(value), next(nullptr) {}
};

template<typename T>
class Queue {
protected:
	Node<T>* front;
	Node<T>* rear;
public:
	Queue();
	~Queue();
	// 队尾插入
	void push(const T& value);
	// 队头弹出
	void pop();
	// 获取队头元素
	T getFront() const;
	// 队列是否为空
	bool isEmpty() const;
	// 打印队列
	void print();
};

template<typename T>
Queue<T>::Queue()
{
	front = nullptr;
	rear = nullptr;
}

template<typename T>
Queue<T>::~Queue()
{
	while (!isEmpty()) {
		pop();
	}
}

template<typename T>
void Queue<T>::push(const T& value)
{
	Node<T>* newNode = new Node<T>(value);
	if (isEmpty()) {
		front = newNode;
		rear = newNode;
		return;
	}
	rear->next = newNode;
	rear = newNode;
}

template<typename T>
void Queue<T>::pop()
{
	if (!isEmpty()) {
		Node<T>* deleteNode = front;
		front = front->next;
		// 队列为空时要将rear置空，否则会悬空指针，指向一块已经释放的内存空间。
		if (front == nullptr) {
			rear = nullptr;
		}
		delete deleteNode;
	}
}

template<typename T>
T Queue<T>::getFront() const
{
	if(!isEmpty()) return front->val;
	return -1;
}

template<typename T>
bool Queue<T>::isEmpty() const
{
	return front == nullptr;
}

template<typename T>
void Queue<T>::print()
{
	Node<T>* cur = front;
	while (cur) {
		cout << cur->val << "->";
		cur = cur->next;
	}
	cout << "nullptr" << endl;
}


int main() {
	Queue<int> q;
	q.push(10);
	q.push(20);
	q.push(30);
	q.push(40);
	q.print();
	
	cout << "队头元素为：" << q.getFront() << endl;
	q.pop();
	q.print();
	cout << "队头元素为：" << q.getFront() << endl;
	cout << "是否为空：" << (q.isEmpty() == 0 ? "不为空" : "为空") << endl;

	return 0;
}