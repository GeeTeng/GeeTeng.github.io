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

#### 友元函数

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

#### 模板函数

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

#### 回调函数

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

#### 一致性哈希

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

#### 静态多态和动态多态

静态多态 -> **函数重载、泛型编程（模板）**

动态多态 -> **虚函数**

静态多态带来了泛型设计的概念，比如STL库。

**区别：**静态多态调用发生在编译时，效率高；动态多态函数调用发生在运行时，会引入一些性能开销。

#### 虚函数

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



#### 纯虚函数

在基类中没有定义，在函数原型后加 =0

```c++
virtual void funtion1()=0
```

> 为什么会引入纯虚函数？

很多情况下，基类本身生成对象是不合理的。比如动物可以派生老虎、狮子等，但是动物生成对象不合理。

**在派生类必须予以重写以实现多态性，含有纯虚函数的类称为抽象类，抽象类是不能生成对象的。**



#### 虚析构函数

**虚析构函数的作用**：当声明析构函数为虚函数时，执行 `delete a;` 会采用动态联编，首先调用派生类的析构函数，然后再调用基类的析构函数。

如果没有声明基类的析构函数为虚函数，则在执行 `delete a;` 时，只会调用基类 `A` 的析构函数，而不会调用派生类 `B` 的析构函数。这会导致派生类 `B` 的资源未被正确释放，从而可能引发内存泄漏。



#### 虚函数表

虚函数通过一张虚函数表来实现的，虚表（vtable）是一个**指针数组**，每个元素对应一个虚函数的函数指针，在代码编译阶段构造出来的虚表。

每个对象都拥有一个虚表指针（vptr），基类和派生类对象的`vptr`指向各自的虚函数表。当你通过基类指针或引用调用虚函数时，程序会通过该对象的vptr找到虚函数表，从虚函数表中获取函数地址，最后调用对应的虚函数。

![vfptr](/images/C++basic/vfptr.png)

![vfptr1](/images/C++basic/vfptr1.png)

---

#### 模板编程

**模板编程**是C++**实现泛型编程**的手段，同一段代码逻辑可以接收多个类型的参数，无需为每种数据类型都编写单独的代码。

 