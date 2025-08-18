#include <iostream>
using namespace std;

template<typename T>

class MyArray
{
protected:
    T* array;
    unsigned int length;

public:
    MyArray();
    MyArray(unsigned int len);
    MyArray(const MyArray& a);
    MyArray& operator=(const MyArray& b);
    T& operator[](int index);
    // 增加元素在末尾
    void push_back(const T& value);
    // 删除指定位置元素
    void remove_at(const unsigned int index);
    // 获取大小
    unsigned int size() const;
    ~MyArray();
};

template<typename T>
MyArray<T>::~MyArray() {
    delete[] this->array;
    this->array = NULL;
}

template<typename T>
MyArray<T>::MyArray() {
    this->array = NULL;
    this->length = 0;
}

template<typename T>
MyArray<T>::MyArray(unsigned int len) {
    this->array = new T[len];
    this->length = len;
    memset(this->array, 0, sizeof(T) * len);
}

template<typename T>
MyArray<T>::MyArray(const MyArray& a) {
    this->length = a.length;
    this->array = new T[a.length];
    memset(this->array, 0, sizeof(T) * a.length);
    for (int i = 0; i < a.length; i++) {
        *(this->array + i) = *(a.array + i);
    }
}

template<typename T>
MyArray<T>& MyArray<T>::operator=(const MyArray& b)
{
    if (this == &b) return *this;
    if (this->array != NULL) {
        delete[] this->array;
        this->array = NULL;
    }
    this->array = new T[b.length];
    this->length = b.length;
    for (int i = 0; i < b.length; i++) {
        *(this->array + i) = *(b.array + i);
    }
    return *this;
}

template<typename T>
T& MyArray<T>::operator[](int index)
{
    if (index < 0 || index >= this->length) {
        throw out_of_range("Array index out of bounds.");
    }
    return *(this->array + index);
}

template<typename T>
void MyArray<T>::push_back(const T& value)
{
    T* newArray = new T[this->length + 1];
    for (int i = 0; i < this->length; i++) {
        *(newArray + i) = *(this->array + i);
    }
    newArray[this->length] = value;
    delete []this->array;
    this->array = newArray;
    this->length++;

}

template<typename T>
void MyArray<T>::remove_at(const unsigned int index)
{
    if (index < 0 || index >= this->length) {
        throw out_of_range("Array index out of bounds.");
    }
    T* newArray = new T[this->length - 1];
    for (int i = 0, j = 0; i < this->length; i++) {
        if (i != index) {
            newArray[j ++] = this->array[i];
        }
    }
    delete[] this->array;
    this->array = newArray;
    this->length--;
}

template<typename T>
unsigned int MyArray<T>::size() const
{
    return this->length;
}

int main() {
    MyArray<int> arr1;
    // 增加测试
    arr1.push_back(10);
    arr1.push_back(20);
    arr1.push_back(30);
    for (int i = 0; i < arr1.size(); i++) {
        printf("arr1的第%d个元素是%d\n", i, arr1[i]);
    }
    // 删除测试
    arr1.remove_at(1);
    for (int i = 0; i < arr1.size(); i++) {
        printf("删除后arr1的第%d个元素是%d\n", i, arr1[i]);
    }
    // 赋值运算符测试
    MyArray<int> arr2(2);
    arr2 = arr1;
    for (int i = 0; i < arr1.size(); i++) {
        printf("arr2第%d个元素是%d\n", i, arr2[i]);
    }
    // 赋值构造函数
    MyArray<int> arr3 = arr1;
    for (int i = 0; i < arr1.size(); i++) {
        printf("arr3第%d个元素是%d\n", i, arr3[i]);
    }

    return 0;
}
