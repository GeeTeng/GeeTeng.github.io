#include<iostream>
using namespace std;

template<typename T>
class Node {
public:
	T data;
	Node<T>* next;
	Node(const T& value) :data(value), next(nullptr) {}
};

template<typename T>
class List {
protected:
	Node<T>* head;

public:
	List();
	List(const List<T>& other);
	List<T>& operator=(const List<T>& other);
	~List();
	// 尾插法
	void push_back(const T& value);
	// 头插法
	void push_front(const T& value);
	// 删除指定元素
	void remove(const T& value);
	// 打印
	void print();
	// 是否存在指定元素
	bool find(const T& value);
	// 清空链表
	void clear();
};

template<typename T>
List<T>::List()
{
	head = nullptr;
}

template<typename T>
List<T>::List(const List<T>& other)
{
	if (!other.head) {
		head = nullptr;
	}
	this->head = other.head;
	Node<T>* cur = this->head;
	Node<T>* otherCur = other.head->next;
	while (otherCur) {
		cur->next = new Node<T>(otherCur->data);
		cur = cur->next;
		otherCur = otherCur->next;
	}
}

template<typename T>
List<T>& List<T>::operator=(const List<T>& other)
{
	if (this == &other) return *this;
	clear();
	if (!other.head) {
		head = nullptr;
		return *this;
	}
	this->head = new Node<T>(other.head->data);
	Node<T>* cur = head;
	Node<T>* otherCur = other.head->next;
	while (otherCur) {
		cur->next = new Node<T>(otherCur->data);
		cur = cur->next;
		otherCur = otherCur->next;
	}
	return *this;
}

template<typename T>
List<T>::~List()
{
	clear();
}

template<typename T>
void List<T>::push_back(const T& value)
{
	Node<T>* newNode = new Node<T>(value);
	if (head == nullptr) {
		head = newNode;
		return;
	}
	Node<T>* cur = head;
	while (cur->next) {
		cur = cur->next;
	}
	cur->next = newNode;
}

template<typename T>
void List<T>::push_front(const T& value)
{
	Node<T>* newNode = new Node<T>(value);
	newNode->next = head;
	head = newNode;
}

template<typename T>
void List<T>::remove(const T& value)
{
	if (!head) return;
	// 删除头节点需要单独考虑
	if (head->data == value) {
		Node<T>* deleteNode = head;
		head = head->next;
		delete deleteNode;
		return;
	}
	// 删除中间节点
	Node<T>* cur = head;
	while (cur->next && cur->next->data != value) {
		cur = cur->next;
	}
	if (cur->next) {
		Node<T>* deleteNode = cur->next;
		cur->next = cur->next->next;
		delete deleteNode;
	}
}

template<typename T>
void List<T>::print()
{
	Node<T>* cur = head;
	while (cur) {
		cout << cur->data << "->";
		cur = cur->next;
	}
	cout << "nullptr" << endl;
}

template<typename T>
bool List<T>::find(const T& value)
{
	Node<T>* cur = head;
	while (cur) {
		if (cur->data == value) return true;
		cur = cur->next;
	}
	return false;
}

template<typename T>
void List<T>::clear()
{
	while (head) {
		Node<T>* tmp = head;
		head = head->next;
		delete tmp;
	}
}

int main() {
	List<int> list;
	list.push_back(10);
	list.push_back(20);
	list.push_front(30);
	list.print();
	cout << (list.find(20) ? "Yes" : "No") << endl;
	cout << (list.find(40) ? "Yes" : "No") << endl;
	list.remove(30);
	list.print();
	
	List<int> list2;
	list2 = list;
	list2.remove(10);
	list2.print();

	List<int> list3 = list;
	list3.print();
	return 0;
}