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
	// ��β����
	void push(const T& value);
	// ��ͷ����
	void pop();
	// ��ȡ��ͷԪ��
	T getFront() const;
	// �����Ƿ�Ϊ��
	bool isEmpty() const;
	// ��ӡ����
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
		// ����Ϊ��ʱҪ��rear�ÿգ����������ָ�룬ָ��һ���Ѿ��ͷŵ��ڴ�ռ䡣
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
	
	cout << "��ͷԪ��Ϊ��" << q.getFront() << endl;
	q.pop();
	q.print();
	cout << "��ͷԪ��Ϊ��" << q.getFront() << endl;
	cout << "�Ƿ�Ϊ�գ�" << (q.isEmpty() == 0 ? "��Ϊ��" : "Ϊ��") << endl;

	return 0;
}