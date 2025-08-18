#include<iostream>
using namespace std;

template<typename T>
class TArray {
private:
	T* data;
	int size;
	int capacity;

public:
	TArray() :data(nullptr), size(0), capacity(0) {}

	~TArray() {
		clear();
	}

	// 确保内存空间足够 用placement new手动构造新元素
	void add(const T& item) {
		ensureCapacity(size + 1);
		new (data + size) T(item);
		++size;
	}

	// 原地构造 移动语义传递右值
	void emplace(T&& item) {
		ensureCapacity(size + 1);
		new (data + size) T(move(item));
		++size;
	}

	void removeAt(int index) {
		if (index < 0 || index >= size) {
			throw out_of_range("Array index out of bounds");
		}
		data[index].~T();
		for (int i = index + 1; i < size; i++) {
			new (data + i - 1) T(move(data[i]));
			data[i].~T();
		}
		--size;
	}

	int getSize()const {
		return size;
	}
	int getCapacity()const {
		return capacity;
	}

	T& operator[](int index) {
		if (index < 0 || index >= size) {
			throw out_of_range("Array index out of bounds.");
		}
		return data[index];
	}

	void clear() {
		for (int i = 0; i < size; i++) {
			data[i].~T();
		}
		size = 0;
		operator delete[](data);
		data = nullptr;
		capacity = 0;
	}

	void shrinkToFit() {
		if (size == capacity) return;
		T* newData = static_cast<T*> (operator new[](sizeof(T)* size));
		for (int i = 0; i < size; i++) {
			new(newData + i) T(move(data[i]));
			data[i].~T();
		}
		operator delete[](data);
		data = newData;
		capacity = size;
	}

private:
	void ensureCapacity(int minSize) {
		// 如果元素个数小于容量 则可以添加 直接返回
		if (minSize <= capacity) return;
		// 扩容：初始状态为0时申请4个大小，否则扩充2倍
		int newCapacity = capacity == 0 ? 4 : capacity * 2;
		while (newCapacity < minSize) {
			newCapacity *= 2;
		}
		// 申请一块新的内存 并且把原数组转移到新地址中 然后析构原来的数组
		// 为什么使用 operator new，因为只是申请内存，之后会用placementnew来手动构造
		T* newData = static_cast<T*>(operator new[](sizeof(T)* newCapacity));
		for (int i = 0; i < size; i++) {
			new (newData + i) T(move(data[i]));
			data[i].~T();
		}
		operator delete[](data);
		data = newData;
		capacity = newCapacity;
	}
};

int main() {
	TArray<int> arr;
	cout << "初始化容量为：" << arr.getCapacity() << "，元素个数为：" << arr.getSize() << endl;
	
	arr.add(10);
	arr.add(20);
	cout << "添加元素后容量为：" << arr.getCapacity() << "，元素个数为：" << arr.getSize() << endl;
	cout << "arr[1]元素为" << arr[1] << endl;

	arr.emplace(30);
	arr.emplace(40);
	arr.emplace(50);
	cout << "添加元素后容量为：" << arr.getCapacity() << "，元素个数为：" << arr.getSize() << endl;
	for (int i = 0; i < arr.getSize(); i++) {
		cout << arr[i] << ' ';
	}
	cout << endl;

	arr.removeAt(1);
	cout << "removeAt(1)后数组中元素：" << endl;
	for (int i = 0; i < arr.getSize(); i++) {
		cout << arr[i] << ' ';
	}
	cout << endl;

	arr.shrinkToFit();
	cout << "缩减容量后的容量为：" << arr.getCapacity() << "，元素个数为：" << arr.getSize() << endl;

	arr.clear();
	cout << "清空数组后数组中元素：" << endl;
	for (int i = 0; i < arr.getSize(); i++) {
		cout << arr[i] << ' ';
	}
	cout << endl;
	return 0;
}
/*
	初始化容量为：0，元素个数为：0
	添加元素后容量为：4，元素个数为：2
	arr[1]元素为20
	添加元素后容量为：8，元素个数为：5
	10 20 30 40 50
	removeAt(1)后数组中元素：
	10 30 40 50
	缩减容量后的容量为：4，元素个数为：4
	清空数组后数组中元素：
*/