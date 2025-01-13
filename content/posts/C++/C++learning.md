---
title: "C++知识回顾"
date: 2024-12-11
tags: [C++]
description: "更新中"
showDate: true
math: true
chordsheet: true
---









---

## decltype

```c++
int a = 10;
decltype(a) b = 20;
cout << typeid(b).name() << endl;

int e = 4;
const int* f = &e;
decltype(auto) j = f; // decltype(auto) 会保留引用类型，但 auto 会丢弃引用类型。
cout << typeid(j).name() << endl;
```

---

## this指针

> 为什么要使用this指针？

两个用法：

- 区分成员变量和函数形参
- 需要返回类本身

当在类的非静态成员函数访问类的非静态成员时，编译器会自动将对象的地址传给作为隐含参数传递给函数，这个隐含参数就是this指针。

```c++
class Person
{
public:
	int getAge()
	{
		return age;
	}
	void setAge(int age)
	{
		// 相当于自己赋值给自己 并不是赋值给Person的成员变量
		age = age;
        // this->age = age;
		cout << "age = " << age << endl; // 输出age = 23
	}
    // 或者
    Person setAge(int age)
	{
		this->age = age;
		cout << "age = " << age << endl;
		return *this;
	}
private:
	int age;
};

int main()
{
	Person p;
	p.setAge(23);
	cout << p.getAge(); // 输出随机值
	return 0;
}
```

this只能在成员函数中使用，**全局函数、静态函数都不能使用this**（因为静态成员函数不需要实例化对象可以直接调用，先于对象存在）。

### delete this

在类中定义一个如下的函数，当调用`delete this`时，内存会被释放，很**危险**。要确保调用后不会再访问该对象。

```c++
// 类内函数
void destroy()
{
	delete this;
}
int main()
{
	Person p;
	p.destroy(); // 会出错，因为在栈空间分配 尝试释放非堆内存是错误的
	return 0;
}
```

在析构函数中调用时，delete this会去调用本对象的析构函数，而析构函数中又调用delete this，形成无限递归，造成**堆栈溢出**，系统崩溃。

---

## 右值引用、移动语义



```c++
void func(int& a)
{
	cout << "调用左值引用" << a << endl;
}

void func(int&& a)
{
	cout << "调用右值引用" << a << endl;
}

int main()
{
	int a = 10;
	func(20); // 输出：调用右值引用20
	func(a); // 输出：调用左值引用10
	return 0;
}
```



---

## 友元函数

全局函数做友元

```c++
class Building
{
	friend void goodGuy(Building* building);
public:
	Building()
	{
		m_SittingRoom = "客厅";
		m_BedRoom = "卧室";
	}
public:
	std::string m_SittingRoom; // 客厅

private:
	std::string m_BedRoom; // 卧室
};

// 全局函数
void goodGuy(Building* building)
{
	std::cout << "好朋友全局函数正在访问：" << building->m_SittingRoom << std::endl;
	std::cout << "好朋友全局函数正在访问：" << building->m_BedRoom << std::endl;
}

int main()
{
	Building building; // 输出：好朋友全局函数正在访问：客厅
	goodGuy(&building);// 输出：好朋友全局函数正在访问：卧室
}
```

**类做友元**也是同样的道理

`friend class GoodGuy;`

**类的成员函数**也是同样的道理

`friend void GoodGuy::visit();`

---

## 模板函数

##### 类模板和函数模板区别

- 类模板没有自动类型推导

- 类模板可以设置默认类型

  `template<class NameType, class AgeType = int>`



##### 类模板与继承

```c++
template<class T>
class Base
{
	T m;
};
class Son :public Base{}; // 错误：必须要知道父类中的T类型才能继承给子类
class Son :public Base<int>{}; // 正确

template<class T1, class T2>
class Son2 : public Base<T2>
{
public:
	T1 obj;
	Son2()
	{
        // 声明Son2<int, char> S2;
		std::cout << "T1类型为" << typeid(T1).name() << std::endl; // int
		std::cout << "T2类型为" << typeid(T2).name() << std::endl; // char
	}
};
```

---

## 回调函数

在函数中把你的回调函数名称转化为地址作为 一个参数，以便于系统调用。

```c++
void myCallbcak(int value)
{
	cout << "callback called with value: " << value << endl;
}
void process(int x, void(*callback)(int))
{
	cout << "Processing Value: " << x << endl;
	callback(x * 2);
}
int main()
{
	process(5, myCallbcak);
}
```

---

## 一致性哈希

一致性哈希用于数据分区，帮助数据库最大限度地减少平衡期间的数据移动。一致性hash的基本思想就是使用相同的hash算法将数据和结点都映射到图中的环形哈希空间中。

- 我们使用均匀分布的哈希函数将服务器和对象映射到哈希环上
- 为了找到对象的服务器，我们从对象的位置沿着环顺时针移动直到找到服务器。

当删除其中一个对象的时候，只需要将服务器分给逆时针最近的环。

当增加一个对象的时候，只需要将逆时针第一个对象移动到它的右边（防止这个对象找不到它对应的服务器），这样无需移动所有对象的位置，更方便。

![consistenthash](/images/C++basic/consistenthash.png)

缺点：环上服务器节点分布不均匀，就算是一开始分布均匀，如果服务器频繁的上下线，也会变得分布不均匀。最终导致很多个客户端。

于是提出**虚拟节点**，通过虚拟节点，每个服务器可以处理环上的多个网段。

![consistenthash01](/images/C++basic/consistenthash01.png)

---

## 静态多态和动态多态

静态多态 -> **函数重载、泛型编程（模板）**

动态多态 -> **虚函数**

静态多态带来了泛型设计的概念，比如STL库。

**区别：**静态多态调用发生在编译时，效率高；动态多态函数调用发生在运行时，会引入一些性能开销。

## 虚函数

在多态会使用到虚函数，**通过基类访问派生类定义的函数**。

它的特点是**动态联编**（函数的调用不是在编译时刻被确定的，而是在运行时刻被确定的）。

**虚函数只能借助指针 / 引用达到多态的效果**

```C++
class A
{
public:
    virtual void foo()
    {
        cout<<"A::foo() is called"<<endl;
    }
};
class B:public A
{
public:
    void foo()
    {
        cout<<"B::foo() is called"<<endl;
    }
};
int main(void)
{
    A *a = new B();
    a->foo();   // 在这里，a虽然是指向A的指针，但是被调用的函数(foo)却是B的!
    return 0;
}
```



## 纯虚函数

在基类中没有定义，在函数原型后加 =0

```c++
virtual void funtion1()=0
```

> 为什么会引入纯虚函数？

很多情况下，基类本身生成对象是不合理的。比如动物可以派生老虎、狮子等，但是动物生成对象不合理。

**在派生类必须予以重写以实现多态性，含有纯虚函数的类称为抽象类，抽象类是不能生成对象的。**



## 虚析构函数

**虚析构函数的作用**：当声明析构函数为虚函数时，执行 `delete a;` 会采用动态联编，首先调用派生类的析构函数，然后再调用基类的析构函数。

如果没有声明基类的析构函数为虚函数，则在执行 `delete a;` 时，只会调用基类 `A` 的析构函数，而不会调用派生类 `B` 的析构函数。这会导致派生类 `B` 的资源未被正确释放，从而可能引发内存泄漏。



## 虚函数表

虚函数通过一张虚函数表来实现的，虚表（vtable）是一个**指针数组**，每个元素对应一个虚函数的函数指针，在代码编译阶段构造出来的虚表。

每个对象都拥有一个虚表指针（vptr），基类和派生类对象的`vptr`指向各自的虚函数表。当你通过基类指针或引用调用虚函数时，程序会通过该对象的vptr找到虚函数表，从虚函数表中获取函数地址，最后调用对应的虚函数。

![vfptr](/images/C++basic/vfptr.png)

![vfptr1](/images/C++basic/vfptr1.png)

---

## 智能指针

### shared_ptr

**实现原理**：共享式指针：采用引用计数器的方法，允许多个智能指针指向同一个对象，每当多一个指针指向该对象时，指向该对象的所有智能指针内部的引用计数加1，每当减少一个智能指针指向对象时，引用计数会减1，当计数为0的时候会自动的释放动态分配的资源。

```c++
#include<memory>

class MyClass
{
public:
	MyClass(const std::string& name) : name(name)
	{
		std::cout << "MyClass " << name << " constructed.\n";
	}
	~MyClass()
	{
		std::cout << "MyClass " << name << " destroyed.\n";
	}
	void sayHello() const
	{
		std::cout << "Hello from " << name << "!\n";
	}
private:
	std::string name;
};

int main()
{
	std::shared_ptr<MyClass> shared1 = std::make_shared<MyClass>("Shared1"); // MyClass Shared1 constructed.
	{
		std::shared_ptr<MyClass> shared2 = shared1; // 共享所有权
		shared2->sayHello(); // Hello from Shared1!
		std::cout << "Shared pointer use count: " << shared1.use_count() << "\n"; // Shared pointer use count: 2
	}
	std::cout << "Shared pointer use count after shared2 out of scope: " << shared1.use_count() << "\n"; // Shared pointer use count after shared2 out of scope: 1
	shared1->sayHello(); // Hello from Shared1!
	// MyClass Shared1 destroyed.
	return 0;
}
```

### unique_ptr

独占式指针，同一时刻只能有一个指针指向同一个对象

```c++
// 上面的类
	std::unique_ptr<MyClass> unique = std::make_unique<MyClass>("unique"); // MyClass unique constructed.
	unique->sayHello(); // Hello from unique!
	// unique_ptr不允许拷贝，但是可以转移所有权
	std::unique_ptr<MyClass> unique2 = std::move(unique);
	if (!unique)
	{
		std::cout << "unique pointer has been moved.\n"; // unique pointer has been moved.
	}
	unique2->sayHello();// Hello from unique!
	// MyClass unique destroyed.
```

当你通过 `std::move` 将所有权从 `unique` 转移到 `unique2` 时，`unique` 不再持有该对象，也就是说它的内部指针变成了空指针（`nullptr`）。此时，**资源并没有被销毁，而是由新的 `unique_ptr`（即 `unique2`）接管管理权**。

### weak_ptr

弱引用，用来解决shared_ptr相互引用导致的死锁问题，类似一个观察者，不增加引用计数。

例子：当两个对象（例如父节点和子节点）通过 `std::shared_ptr` 相互持有对方时，会导致 **循环引用**。

- 父节点持有对子节点的 `shared_ptr`。
- 子节点持有对父节点的 `shared_ptr`。

由于 `shared_ptr` 通过引用计数来管理资源，这样的循环关系会导致引用计数永远不为 0，因为每个对象的引用计数都会被对方的 `shared_ptr` 所增加。这样，即使这两个对象都超出了作用域，它们的引用计数仍然没有减少到零，因此资源无法被释放，最终导致**内存泄漏**。

```c++
#include <iostream>
#include <memory>

class Node {
public:
    std::shared_ptr<Node> child; // 子节点，shared_ptr
    std::weak_ptr<Node> parent;  // 父节点，使用 weak_ptr 避免循环引用

    Node() {
        std::cout << "Node created\n";
    }

    ~Node() {
        std::cout << "Node destroyed\n";
    }
};

int main() {
    // 创建父节点和子节点
    std::shared_ptr<Node> parent = std::make_shared<Node>();
    std::shared_ptr<Node> child = std::make_shared<Node>();

    // 创建父子关系
    parent->child = child; // 父节点持有子节点的 shared_ptr
    child->parent = parent; // 子节点持有父节点的 weak_ptr

    // 这里，parent 和 child 会正确销毁，因为 weak_ptr 不增加引用计数
    return 0;
}

```

---

## 手写实现智能指针

### UniquePtr

```c++
#include<iostream>

template<typename T>
class UniquePtr
{
public:
	explicit UniquePtr(T* p = nullptr) :ptr(p) {}
	// 禁止拷贝构造
	UniquePtr(const UniquePtr&) = delete;
	// 禁止拷贝赋值
	UniquePtr& operator=(const UniquePtr&) = delete;
	// 移动构造函数 在构造一个新对象时被调用
	UniquePtr(UniquePtr&& other) noexcept : ptr(other.ptr)
	{
		other.ptr = nullptr;
	}
	//移动赋值运算符 在已经存在的对象上进行资源转移时被调用
	UniquePtr& operator=(UniquePtr&& other)noexcept
	{
		if (this != &other)
		{
			delete ptr;
			ptr = other.ptr;
			other.ptr = nullptr;
		}
		return *this;
	}
	// 重载 * 和 -> 运算符 让智能指针表现得像一个原始指针一样
	T& operator*() const
	{
		return *ptr;
	}

	T* operator->() const
	{
		return ptr;
	}
	// 释放控制权
	T* release()
	{
		T* temp = ptr;
		ptr = nullptr;
		return temp;
	}
	// 重置指针
	void reset(T* p = nullptr)
	{
		delete ptr;
		ptr = p; // // 指向新资源，或nullptr
	}

	~UniquePtr()
	{
		delete ptr;
	}

private:
	T* ptr;
};
```

### SharedPtr

```c++
#include<iostream>

template<typename T>
class SharedPtr
{
private:
	T* ptr;
	// size_t是专门用于表示内存大小的类型 语义清晰 平台兼容性（大小自动调整）
	size_t* refCount;

public:
	explicit SharedPtr(T* p = nullptr) : ptr(p), refCount(new size_t(1)) { }

	SharedPtr(const SharedPtr& other) : ptr(other.ptr), refCount(other.refCount)
	{
		++(*refCount); // 增加引用计数
	}

	SharedPtr& operator=(const SharedPtr& other)
	{
		if (this != &other)
		{
			release();
			ptr = other.ptr;
			refCount = other.refCount;
			++(*refCount);
		}
		return *this;
	}

	T& operator*() const
	{
		return *ptr;
	}
	T* operator->() const
	{
		return ptr;
	}
	size_t use_count() const
	{
		return *refCount;
	}
	~SharedPtr()
	{
		release();
	}
private:
	void release()
	{
		if (--(*refCount) == 0)
		{
			delete ptr;
			delete refCount;
		}
	}
};

int main()
{
	SharedPtr<int> sptr1(new int(20));
	std::cout << "sptr1 use_count: " << sptr1.use_count() << std::endl;

	{
		SharedPtr<int> sptr2 = sptr1;
		std::cout << "sptr1 use_count: " << sptr1.use_count() << std::endl;
		std::cout << "sptr2 use_count: " << sptr2.use_count() << std::endl;
	}

	std::cout << "sp1 use_count after sptr2 destroyed: " << sptr1.use_count() << std::endl;
	return 0;
}
```

---

## Lambda函数 / 匿名函数 / 闭包

用法：参数列表两个int，返回int。

```c++
auto f1 = [](int a, int b)->int {return a + b; };
std::cout << f1(1, 2) << std::endl;
```

### 变量捕获

按引用捕获、按值捕获

```c++
	int N = 100, M = 10;
	auto g = [N, &M, k = 5](int i)
	{
		M = 20; // &M：按引用捕获，Lambda 表达式对 M 的修改会直接反映到外部变量 M 上。
		std::cout << k << std::endl; // 5
		return N * i;
	};
	std::cout << g(10) << std::endl;
	std::cout << M << std::endl;
```

