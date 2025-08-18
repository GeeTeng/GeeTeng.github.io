#include<iostream>
using namespace std;

template<typename T>
class Node {
public:
	int val;
	Node<T>* next;
	Node(const int value) :val(value), next(nullptr) {}
};

template<typename T>
class Stack {
protected:
	Node<T>* head;
public:
	Stack();
	~Stack();
	// 入栈
	void push(const int val);
	// 出栈
	void pop();
	// 获取栈顶元素
	T top() const;
	// 栈是否为空
	bool isEmpty() const;
	// 打印单链表栈
	void print();
};

template<typename T>
Stack<T>::Stack()
{
	head = nullptr;
}

template<typename T>
Stack<T>::~Stack()
{
	while (!isEmpty()) {
		pop();
	}
}

template<typename T>
void Stack<T>::push(const int val)
{
	Node<T>* newNode = new Node<T>(val);
	if (isEmpty()) {
		head = newNode;
		return;
	}
	newNode->next = head;
	head = newNode;
}

template<typename T>
void Stack<T>::pop()
{
	if (!isEmpty()) {
		Node<T>* deleteNode = head;
		head = head->next;
		delete deleteNode;
	}
}

template<typename T>
T Stack<T>::top() const
{
	if (!isEmpty()) {
		return head->val;
	}
	// 栈空则返回-1
	return -1;
}

template<typename T>
bool Stack<T>::isEmpty()const
{
	return head == nullptr;
}

template<typename T>
void Stack<T>::print()
{
	Node<T>* cur = head;
	while (cur) {
		cout << cur->val << "->";
		cur = cur->next;
	}
	cout << "nullptr" << endl;
}



int main() {
	Stack<int> s1;
	s1.push(10);
	s1.push(20);
	s1.print();

	cout << "s1栈顶元素为：" << s1.top() << endl;

	s1.pop();
	s1.print();

	cout << "s1是否为空：" << (s1.isEmpty() == 0 ? "不为空" : "为空") << endl;


	return 0;
}