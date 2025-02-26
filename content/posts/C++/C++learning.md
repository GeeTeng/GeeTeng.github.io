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



![01](/images/C++basic/01.png)

随机存储器RAM（主存）

与CPU直接交换数据，可读写区域，存放**全局（静态）区**、**堆区**、**栈区**。

只读存储器ROM

只能读出 所存数据稳定，断电后数据不会改变。存放**代码区**和**常量区**。

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

### 类模板和函数模板区别

- 类模板没有自动类型推导

- 类模板可以设置默认类型

  `template<class NameType, class AgeType = int>`



### 类模板与继承

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



### 模板特例化

**全特例化**：全特例化必须为特定类型提供完整的实现，并且不能包含模板参数。

例如，`template <> class MyClass<int>` 就是对 `MyClass` 模板在 `int` 类型上的完全特例化。

**偏特例化**：偏特例化允许你为模板的部分类型参数提供一个特定实现。例如，`template <typename T> class MyClass<T*, int>` 就是为所有指针类型和 `int` 提供一个偏特例化的实现。

```c++
template <typename T, typename U>
class MyClass
{
public:
	void print()
	{
		cout << "Generic Template" << endl;
	}
};

template<typename T>
class MyClass<T*, int>
{
public:
	void print()
	{
		cout << "Template specialized for pointer types and int" << endl;
	}
};

template<>
class MyClass<char, float>
{
public:
	void print()
	{
		cout << "Template specialized for char and float" << endl;
	}
};

int main()
{
	MyClass<int, float> obj1; // 使用普通模板
	MyClass<int*, int> obj2; // 使用偏特例化
	MyClass<char, float> obj3; // 使用完全特例化模板

	obj1.print(); // 输出 "Generic Template"
	obj2.print(); // 输出 "Template specialized for pointer types and int"
	obj3.print(); // 输出 "Template specialized for char and float"
}
```

### 类模板特例化

```c++
template<typename T>
class Foo
{
public:
	void Bar();
};

template<>
void Foo<int>::Bar()
{
	cout << "Bar的int类型特例化" << endl;
}
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

## 内联函数

内联函数是通过 `inline` 关键字声明的函数，目的是告诉编译器尝试将函数调用的代码插入到调用点，而不是进行常规的函数调用（即跳转到函数实现的过程）。这种方法通常能**减少函数调用的开销**，尤其是对于短小的函数。

然而，内联并不是绝对的，**编译器有时会根据函数的复杂度或其他因素决定是否内联展开**。编译器会根据启用内联的提示做优化，但**并不会强制内联**。

```c++
class Base
{
public:
	virtual inline void foo()
	{
		cout << "Base foo" << endl;
	}
};
class Derived : public Base
{
public:
	inline void foo() override
	{
		cout << "Derived foo" << endl;
	}
};
int main()
{
	Base* bPtr = new Derived();
	bPtr->foo(); // 通过指针调用虚函数（不会内联展开）

	Derived dObj;
	dObj.foo(); // 通过对象调用虚函数（可能内联展开）
	//是当用对象调用虚函数（此时不具有多态性）时，就内联展开
	delete bPtr;
	return 0;
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

## 多态

### 静态多态和动态多态

静态多态 -> **函数重载、泛型编程（模板）**

动态多态 -> **虚函数**

静态多态带来了泛型设计的概念，比如STL库。

**区别：**静态多态调用发生在编译时，效率高；动态多态函数调用发生在运行时，会引入一些性能开销。

### 虚函数

多态的过程：

1. 编辑器发现基类中有虚函数时，会自动为每个含有虚函数的类生成一份虚表，该表是一个一维数组，保存了虚函数的入口地址。
2. 在每个对象前4个字节中保存一个虚表指针vptr，指向对象所属类的虚表。在构造时初始化vptr。
3. 在构造子类对象时，会先调用父类的构造函数，**为父类对象初始化虚表指针**，令它指向父类的虚表；当调用子类的构造函数时，为**子类对象初始化虚表指针**，令它指向子类的虚表。
4. **当派生类对基类的虚函数没有重写时，派生类的虚表指针指向的是基类的虚表**；**当派生类对基类 的虚函数重写时，派生类的虚表指针指向的是自身的虚表**；



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



### 纯虚函数

在基类中没有定义，在函数原型后加 =0

```c++
virtual void funtion1()=0
```

> 为什么会引入纯虚函数？

很多情况下，基类本身生成对象是不合理的。比如动物可以派生老虎、狮子等，但是动物生成对象不合理。

**在派生类必须予以重写以实现多态性，含有纯虚函数的类称为抽象类，抽象类是不能生成对象的。**



### 虚析构函数

**虚析构函数的作用**：当声明析构函数为虚函数时，执行 `delete a;` 会采用动态联编，首先调用派生类的析构函数，然后再调用基类的析构函数。

如果没有声明基类的析构函数为虚函数，则在执行 `delete a;` 时，只会调用基类 `A` 的析构函数，而不会调用派生类 `B` 的析构函数。这会导致派生类 `B` 的资源未被正确释放，从而可能引发内存泄漏。



### 虚函数表

虚函数通过一张虚函数表来实现的，虚表（vtable）是一个**指针数组**，每个元素对应一个虚函数的函数指针，在代码编译阶段构造出来的虚表。

每个对象都拥有一个虚表指针（vptr），基类和派生类对象的`vptr`指向各自的虚函数表。当你通过基类指针或引用调用虚函数时，程序会通过该对象的vptr找到虚函数表，从虚函数表中获取函数地址，最后调用对应的虚函数。

C++中虚函数表位于只读数据段（.rodata），也就是C++内存模型中的常量区；而虚函数则位于代码段 （.text），也就是C++内存模型中的代码区。

![vfptr](/images/C++basic/vfptr.png)

![vfptr1](/images/C++basic/vfptr1.png)



### 总结的疑问

一些我自己总结的疑问：

**1.子类虚表中会有父类的虚函数地址吗？**

回答：子类和父类各有自己的虚函数表。

子类虚表的结构

- **继承未重写的虚函数**
  如果子类没有重写父类的某些虚函数，子类的虚表中相应的条目会直接指向父类的实现。
- **重写的虚函数**
  当子类重写父类的虚函数时，子类的虚表会将父类虚函数的条目替换为子类实现的版本。
- **新增的虚函数**
  子类的虚表会新增条目，记录子类特有的虚函数。

当子类对象要调用自己的虚函数的时候，直接会在自己的虚表中查到。

**2.为什么析构函数一般写成虚函数？**

如果析构函数不被声明成虚函数，则编译器实施静态绑定，在删除基类指针时，只会调用基类的析构函数而不调用派生类析构函数，这样就会造成派生类对象析构不完全，造成内存泄漏。

如果析构函数是虚函数，**会从子类开始调用析构函数，然后依次调用父类的析构函数**，保证对象的完全销毁。

**3.那为什么构造函数不需要写成虚函数**

**构造顺序：**从父类开始，依次向下构造子类。

**析构顺序：**从子类开始，依次向上析构父类。

而且对象在构造时并未完全构造出来，因此无法正确地调用虚函数。

如果派生类构造函数是虚函数，创建派生类对象的时候，还没有初始化虚表和虚指针，就无法调用自己重写的虚函数，可能是调用基类的虚函数，调用错误的虚函数了。其次第二个可能发生的就是如果基类的构造函数是虚函数那么创建基类对象的时候无法调用虚函数，因为还没构造出虚表和虚指针。

---

### CRTP(奇特重现模板模式)

CRTP最常见的应用场景之一是实现静态多态性。在传统的多态性实现中，通常使用虚函数和动态绑定，而CRTP则通过模板和静态绑定实现同样的功能，避免了运行时开销。例如：

```c++
template<typename T>
class Shape
{
public:
	void draw()
	{
		static_cast<T*>(this)->draw();
	}
};

class Circle :public Shape<Circle>
{
public:
	void draw()
	{
		std::cout << "画圆\n";
	}
};
```

在游戏开发中，CRTP可以用于实现高效的游戏对象管理和行为控制，通过CRTP，可以在编译期间确定游戏对象的具体行为，减少运行时的多态性开销，从而提高游戏的执行效率。如下面在游戏开发中的典型应用场景：

游戏中的每个对象，如玩家和敌人，都需要在每一帧中进行更新操作。传统的多态性通常通过虚函数实现，这会在每次调用时引入一定的运行时开销。而使用CRTP，可以在编译期间确定对象的具体类型和行为，避免运行时的虚函数调用，从而提高效率。例如：

```c++
template <typename T>
class GameObject {
public:
    void update() {
        static_cast<T*>(this)->update_impl();
    }
};
 
class Player : public GameObject<Player> {
public:
    void update_impl() {
        // Implementation of player update logic
    }
};
 
class Enemy : public GameObject<Enemy> {
public:
    void update_impl() {
        // Implementation of enemy update logic
    }
};
```

在上面例子中需要显示转换成派生类类型，才能调用派生类方法，但是**C++23 deducing this**特性可以不用显式使用static_cast。

```c++
template <typename T>
class Base
{
public:
	void interface(this T& self)
	{
		self.impl();
	}
};

class Derived :public Base<Derived>
{
public:
	void impl()
	{
		std::cout << "Derived class specific implementation\n";
	}
};
```



---

## 智能指针

C++ 标准库中的智能指针（如 `std::unique_ptr` 和 `std::shared_ptr`）是 RAII 的典型应用。

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

---

## 左值与右值

 左值可以取地址&，右值通常是一个临时对象，无法取地址。

```c++
int a = 10;  // a 是左值
int* p = &a; // 可以对左值取地址

int b = a + 5; // (a + 5) 是右值，结果是一个临时值
int* p = &(a + 5); // 错误，不能对右值取地址
```

C++11之后将右值进一步细分：纯右值、将亡值

**纯右值**：某个对象的值/计算表达式的结果

**将亡值**：即将销毁的右值，资源很快会被回收，如下：

```c++
std::string createString() {
    return "Hello"; // 返回一个临时的 std::string 对象（将亡值）
}

std::string&& rvalueRef = createString(); // 捕获将亡值
```

---

## 右值引用

```c++
void PrintName(const std::string& name) {
    std::cout << "[lvalue] " << name << std::endl;
}
void PrintName(const std::string&& name) {
    std::cout << "[rvalue] " << name << std::endl;
}
 
std::string firstName = "Yan";
std::string lastName = "Chernikov";
std::string fullName = firstName + lastName;
 
PrintName(fullName); // [lvalue] YanChernikov
PrintName(firstName + lastName); // [rvalue] YanChernikov
```




## 移动语义

**移动语义**：C++11引入了右值引用（`T&&`）和 `std::move`，用于高效地转移临时对象的资源，而不是进行昂贵的深拷贝。**避免复制**。

比如UniquePtr智能指针就实现了移动语义，UniquePtr类中含有**移动构造函数**和**移动赋值运算符**，可以通过**move**来转移所有权给另一个对象。

使用 `std::move` 的对象会进入“将亡”状态。

---

## 完美转发

完美转发目的是让一个函数能够**将其接收到的参数原封不动地转发给另一个函数**。

主要用于**模板函数**，通过结合**万能引用**和`std::forward`。

```c++
void f(const string& s)
{
	cout << "左值" << s << endl;
}
void f(const string&& s)
{
	cout << "右值" << s << endl;
}
void g(const string& s)
{
	f(static_cast<const string&>(s));
}
void g(const string&& s)
{
	f(static_cast<const string&&>(s));
}
int main()
{
	g(string("Hello1")); // 右值Hello1
	string s1("Hello2");
	g(s1); // 左值Hello2
	return 0;
}
```

在完美转发中专门有forward做类型转换，防止编译器自动类型转换 `f(std::forward<const string&&>(s));`

当有多个函数参数时，我们不可能写很多句类型转换的代码对参数一个个强制转换，所以要结合函数模板使用。

```c++
template<class T>
// 万能引用 既可以绑定右值又可以绑定左值
void g(T&& v)
{
	f(std::forward<T>(v));
}
```

以`g(string("Hello1"));`为例，在这个过程中发生了，将string传递给g，T变成string，调用右值的函数。

如果是s1则是string&传递进去，T变成string& &&，**引用折叠规则**变成了string&，调用左值的函数。

---

## constexpr

当用constexpr声明一个函数/变量时，编译器可以**在编译时就计算出它的值**，提高程序的运行效率。

```c++
constexpr int GetLen(int a, int b)
{
	return a + b;
}

int main()
{
	// 不加constexpr就无法声明arr 加了之后就会返回常数
	int arr[GetLen(1, 2)];
	return 0;
}
```

### const 和 constexpr 变量之间的主要区别

- const 变量的初始化可以延迟到运行时，而 constexpr 变量必须在编译时进行初始化。
- const修饰一个对象表示它是常量。constexpr是修饰一个常量表达式。

---

## 多线程

传递函数指针

```c++
void sayhello(int a, int b)
{
	cout << "hello" << a << b << endl;
}
void sayhello2(int& a, int& b)
{
	a += 5, b += 10;
	cout << a << "," << b;
}
int main()
{
	// 创建子线程
	// 传递一个可调对象
	thread t1(sayhello, 10, 20);
	t1.join();// 阻塞主线程
	// main所在线程就是主线程
	cout << "this is main" << endl;

    int x = 10, y = 20;
	thread t2(sayhello2, ref(x), ref(y));//引用传递
	return 0;
}
```

传递成员函数指针

```c++
	MyClass obj;
	// 传递成员函数指针和成员函数对象
	thread t(&MyClass::memberFunctionTask, &obj);
	t.join();
```

传递lambda表达式

```c++
thread t1([]() {cout << "hello lambda\n"; });
t1.join();
```

传递仿函数

```c++
struct Functor
{
	void operator()()
	{
		cout << "Functor task executed\n";
	}
};
int main()
{
	Functor f;
	thread t(f);
	t.join();
	return 0;
}
```

传递绑定对象

```c++
auto boundFunc = bind(functionWithArgs, 1, 2);
thread t(boundFunc);
t.join();
```

### join与detach区别

**join方法**：当一个线程调用join方法时，会阻塞当前线程（调用join线程），直到join线程执行完成。这样确保了子线程执行完毕之前不会结束主线程的执行。使用join可确保资源被正确地回收和清理。

**detach方法**：当调用detach时，线程将与其所属的线程（即调用detach的线程分离），一旦线程被分离，将不再受到主线程的控制，主线程无法调用join来等待它的结束。分离的线程在执行完毕后系统会自动回收其资源。

### 线程组成部分

- **线程ID**:TID唯一标识符。
- **线程栈**：每个线程都有自己私有的栈空间，用于存储局部变量，函数调用时的参数和返回地址等信息。线程栈在创建线程时分配，并在线程结束时释放。
- **线程状态**：创建、就绪、允许、阻塞、终止。
- **线程上下文**：包含了线程执行时所有信息，包括cpu寄存器内容，程序计数器PC的值，栈指针等，当进程被切换时，上下文会被保存，以便之后恢复执行时能够继续执行。
- **线程函数**：线程执行的具体逻辑，包含线程需要执行的代码。
- **线程优先级**：优先级较高会得到更多处理器时间。
- **线程属性**：栈大小、安全属性。
- **线程同步原语**：为了协调多个线程并发执行，如互斥锁、条件变量、信号量等。

`this_thread::get_id()`：获取当前线程ID

`this_thread::sleep_for(1s)`:使当前线程休眠

### 线程同步机制

**互斥锁**：如果锁被其他线程持有，呢么该进程将被阻塞，直到锁释放。

**条件变量**：通常与互斥锁一起使用，在等待条件成立时释放锁。

**信号量**：PV操作，信号量内部维护一个计数器，表示可用资源的数量。

**原子操作**：不可中断操作。

---

### 互斥锁mutex

**互斥性**、**原子性**、**可重入性**（允许同一线程多次获取同一个锁，但不建议这么做，容易死锁）、**非阻塞性**（在某些高级实现比如尝试锁，不会阻塞，会做一些其他任务）

如果不加锁的话，两个线程可能会同时访问counter，最终结果是不可预计的。

```c++
#include<iostream>
#include<thread>
#include<mutex>
using namespace std;
// 共享变量 没有使用互斥锁/原子操作
int counter = 0;
mutex myMutex; //声明互斥锁
// 线程函数 对counter进行递增操作
void increment_counter(int times)
{
	for (int i = 0; i < times; i++)
	{
		myMutex.lock(); // 加锁
		counter++;
		myMutex.unlock(); // 解锁
	}
}
int main()
{
	thread t1(increment_counter, 100000);
	thread t2(increment_counter, 100000);
	t1.join();
	t2.join();
	cout << "最终的结果" << counter << endl;
	return 0;
}
```

使用RAII管理锁：通过`lock_guard` / `unique_lock`来确保锁在不需要时自动释放，类同智能指针。

避免长时间持有锁：提高程序的并发性能。

**mutex的4种类型**

1. `mutex`：不允许递归锁定，同一线程不能多次锁定同一个mutex，会引发死锁。

2. `recursive_mutex`：递归互斥量，与mutex相反.

   ```c++
   recursive_mutex mtx;
   void recursive_function()
   {
   	mtx.lock();
   	cout << "Thread locked mutex" << this_thread::get_id() << endl;
   	mtx.lock();
   	cout << "Thread locked mutex recursively" << this_thread::get_id() << endl;
   	mtx.unlock();
   	cout << "Thread unlocked mutex once" << this_thread::get_id() << endl;
   	mtx.unlock();
   	cout << "Thread unlocked mutex again" << this_thread::get_id() << endl;
   }
   ```

3. `timed_mutex`：时限锁，除了提供基本的锁定和解锁操作外，还允许线程在一定时间内锁定互斥量。如果在指定时间内无法获取锁，try_lock_for() / try_lock_until()将返回失败，而线程不会被阻塞。

   ```c++
   timed_mutex mtx2;
   void timed_lock_function()
   {
   	auto start = chrono::high_resolution_clock::now();
   	// 尝试在指定时间内获取锁
   	if (mtx2.try_lock_for(chrono::seconds(2)))
   	{
   		cout << "Thread locked mutex" << this_thread::get_id() << endl;
   		this_thread::sleep_for(chrono::seconds(3));
   		mtx2.unlock();
   		cout << "Thread unlocked mutex" << this_thread::get_id() << endl;
   	}
   	else
   	{
   		cout << "Thread failed to locked mutex" << this_thread::get_id() << endl;
   	}
   	auto end = chrono::high_resolution_clock::now();
   	chrono::duration<double> diff = end - start;
   	cout << "Thread took " << diff.count() << this_thread::get_id() << endl;
   }
   int main()
   {
   	thread t1(timed_lock_function);
   	thread t2(timed_lock_function);
   	t1.join();	
   	t2.join();
   	/*
   	Thread locked mutex 2736
   	Thread failed to locked mutex 26236
   	Thread took 2.01198 26236
   	Thread unlocked mutex 2736
   	Thread took 3.00277 2736
   	*/
   	return 0;
   }
   ```

4. `recursive_timed_mutex`：递归时限锁

### lock_guard

lock_guard是一个模板类，位于mutex头文件中，符合RAII风格，主要用于管理mutex的声明周期，确保mutex正确的上锁解锁。

> 为什么需要lock_guard？

- 抛出异常需要正确解锁
- 复杂的逻辑手动解锁复杂
- 防止死锁

```c++
void sum()
{
	lock_guard<mutex> lock(mymutex); // 保护的是mutex类型的锁mymutex
	for (size_t i = 0; i < 10000; i++)
	{
		mycount++;
	}// 超出生命周期自动解锁
}
```

**不可复制不可移动**（独占性质）因为禁用了拷贝构造函数和拷贝赋值函数。

### unique_lock

lock_guard的问题是只有生命周期结束才会解锁，无法在中途解锁（只管控一部分，除非使用{}作用域，但也不灵活），所以使用unique_lock

unique_lock有如下常用成员函数：

`lock()`、`unlock()`

`try_lock()`：尝试锁定mutex，如果锁定成功返回true，否则返回false。

`owns_lock()`：返回一个布尔值，指示unique_lock是否拥有mutex的所有权。

unique_lock的三个参数：

1. defer_lock

   ```c++
   mutex mtx;
   int counter = 0;
   int counter2 = 0;
   void example_defer_lock()
   {
   	unique_lock<mutex> lock(mtx, defer_lock); // 不会立刻锁定
   	// 执行一些操作
   	for (size_t i = 0; i < 100000; i++)
   	{
   		counter ++;
   	}
   	lock.lock();
   	for (size_t i = 0; i < 100000; i++)
   	{
   		counter2++;
   	}
   	cout << "locked with defer_lock" << endl;
   	lock.unlock();
   }
   
   int main()
   {
   	thread t1(example_defer_lock);
   	thread t2(example_defer_lock);
   	t1.join();
   	t2.join();
   	cout << counter << " " << counter2 << endl;
       /*
       locked with defer_lock
       locked with defer_lock
       101971 200000
       */
   	return 0;
   }
   ```

2. adopt_lock

   接管已上锁的锁

   ```c++
   void example_adopt_lock()
   {
   	mtx3.lock();
   	unique_lock<mutex> lock(mtx3, adopt_lock); // 接管已锁定的互斥锁
   	cout << "Locked with adopt_lock" << endl;
   }
   ```

3. try_to_lock

   以防没有锁上而重复释放锁 / 上锁了又重复上锁的操作

   ```c++
   void example_try_to_lock()
   {
   	unique_lock<mutex> lock(mtx, try_to_lock);
   	if (lock.owns_lock())
   	{
   		cout << "locked successfully" << endl;
   	}
   	else
   	{
   		cout << "failed to lock" << endl;
   	}
   }
   ```


---

**线程间同步的方式**：互斥锁、读写锁、条件变量、信号量、原子操作、栅栏（c++20）

### 条件变量

等待和通知机制：等待某个条件的改变，而不需要一直占用CPU资源

与互斥锁配合使用

工作原理：当线程调用wait方法时，会首先释放与unique_lock关联的互斥锁，为了允许其他线程可以访问共享数据。然后该线程进入等待队列，变成阻塞状态。等待条件/通知（notify_one / notify_all），重新尝试获取锁，再去判断条件是否为true，如果条件为真继续执行。

```c++
mutex mtx;
condition_variable cv;
bool flag = false;
void myprint(int i)
{
	unique_lock<mutex> lck(mtx);
	while (!flag)
	{
		cv.wait(lck); // 条件不成立 进入阻塞状态 释放掉锁 等待别人唤醒
	}
	cout << this_thread::get_id() << "-" << i << endl;
}
void updateflag()
{
	cout << "this is update" << endl;
	this_thread::sleep_for(3s);
	unique_lock<mutex> lck(mtx);
	flag = true;
	cv.notify_all(); // 通知所有的线程
	// cv.notify_one(); // 通知一个
}
int main()
{
	vector<thread> mybox;
	for (size_t i = 0; i < 10; i++)
		mybox.emplace_back(myprint, i);
	updateflag();
	for (thread& t : mybox)
		t.join();
	return 0;
}
```

```c++
mutex mtx;
condition_variable cv;
int myvalue = 0;
bool turn = false;
void increment(int id)
{
	for (int i = 0; i < 10; i++)
	{
		unique_lock<mutex> lock(mtx);
		cv.wait(lock, [&] {return (id == 1) ? !turn : turn; });
		++myvalue;
		cout << "thread " << id << " " << myvalue << endl;
		turn = !turn;
		cv.notify_all();
	}
}
int main()
{
	thread t1(increment, 1);
	thread t2(increment, 2);
	t1.join();
	t2.join();
	cout << myvalue << endl; // t1 t2交替执行 thread 1 1 thread 2 2 thread 1 3 thread 2 4...
	return 0;
}
```

### 读写锁

读操作可以共享，写操作必须互斥，读写之间也要互斥。适用于读多写少的场景。

**共享锁允许多个线程同时持有，不能和独占锁同时持有**。

只有一个线程能持有独占锁，**当持有独占锁时其他线程不能持有共享锁 / 独占锁**。

排他性：lock、try_lock、unlock

共享性：lock_shared、try_lock_shared、unlock_shared

```c++
shared_mutex rw_mutex;
int shared_data = 0;
void reader()
{
	shared_lock<shared_mutex> lock(rw_mutex);
	cout << "reader thread " << this_thread::get_id() << "value " << shared_data << endl;
}
void writer(int value)
{
	unique_lock<shared_mutex> lock(rw_mutex);
	shared_data = value;
	cout << "writer thread " << this_thread::get_id() << "value " << shared_data << endl;
}
int main()
{
	vector<thread> threads;
	for (int i = 0; i < 5; i++)
	{
		threads.emplace_back(reader);
	}
	for (int i = 0; i < 5; i++)
	{
		threads.emplace_back(writer, i);
	}
	for (auto& t : threads)
	{
		t.join();
	}
	return 0;
}
```

