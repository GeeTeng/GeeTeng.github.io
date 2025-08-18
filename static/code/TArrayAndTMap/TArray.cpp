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

	// ȷ���ڴ�ռ��㹻 ��placement new�ֶ�������Ԫ��
	void add(const T& item) {
		ensureCapacity(size + 1);
		new (data + size) T(item);
		++size;
	}

	// ԭ�ع��� �ƶ����崫����ֵ
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
		// ���Ԫ�ظ���С������ �������� ֱ�ӷ���
		if (minSize <= capacity) return;
		// ���ݣ���ʼ״̬Ϊ0ʱ����4����С����������2��
		int newCapacity = capacity == 0 ? 4 : capacity * 2;
		while (newCapacity < minSize) {
			newCapacity *= 2;
		}
		// ����һ���µ��ڴ� ���Ұ�ԭ����ת�Ƶ��µ�ַ�� Ȼ������ԭ��������
		// Ϊʲôʹ�� operator new����Ϊֻ�������ڴ棬֮�����placementnew���ֶ�����
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
	cout << "��ʼ������Ϊ��" << arr.getCapacity() << "��Ԫ�ظ���Ϊ��" << arr.getSize() << endl;
	
	arr.add(10);
	arr.add(20);
	cout << "���Ԫ�غ�����Ϊ��" << arr.getCapacity() << "��Ԫ�ظ���Ϊ��" << arr.getSize() << endl;
	cout << "arr[1]Ԫ��Ϊ" << arr[1] << endl;

	arr.emplace(30);
	arr.emplace(40);
	arr.emplace(50);
	cout << "���Ԫ�غ�����Ϊ��" << arr.getCapacity() << "��Ԫ�ظ���Ϊ��" << arr.getSize() << endl;
	for (int i = 0; i < arr.getSize(); i++) {
		cout << arr[i] << ' ';
	}
	cout << endl;

	arr.removeAt(1);
	cout << "removeAt(1)��������Ԫ�أ�" << endl;
	for (int i = 0; i < arr.getSize(); i++) {
		cout << arr[i] << ' ';
	}
	cout << endl;

	arr.shrinkToFit();
	cout << "���������������Ϊ��" << arr.getCapacity() << "��Ԫ�ظ���Ϊ��" << arr.getSize() << endl;

	arr.clear();
	cout << "��������������Ԫ�أ�" << endl;
	for (int i = 0; i < arr.getSize(); i++) {
		cout << arr[i] << ' ';
	}
	cout << endl;
	return 0;
}
/*
	��ʼ������Ϊ��0��Ԫ�ظ���Ϊ��0
	���Ԫ�غ�����Ϊ��4��Ԫ�ظ���Ϊ��2
	arr[1]Ԫ��Ϊ20
	���Ԫ�غ�����Ϊ��8��Ԫ�ظ���Ϊ��5
	10 20 30 40 50
	removeAt(1)��������Ԫ�أ�
	10 30 40 50
	���������������Ϊ��4��Ԫ�ظ���Ϊ��4
	��������������Ԫ�أ�
*/