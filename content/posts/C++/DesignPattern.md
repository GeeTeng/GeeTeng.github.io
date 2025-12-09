---
title: "设计模式"
date: 2025-01-08
tags: [C++]
description: "设计模式 更新中"
showDate: true
math: true
chordsheet: true
---



## 简单工厂模式

**Factory**：工厂类是整个模式的关键.包含了必要的逻辑判断,根据外界给定的信息,决定究竟应该创建哪个具体类的对象。

客户端代码无需关心类的实现细节，如果需要新增运算，只需要新增对应的派生类，并在工厂中添加逻辑。

```c++
class Operation
{
protected:
	double numberA;
	double numberB;
public:
	void setNumberA(double a)
	{
		numberA = a;
	}

	void setNumberB(double b)
	{
		numberB = b;
	}

	virtual double GetResult()
	{
		double result = 0;
		return result;
	}
	virtual ~Operation() = default;
};

class OperationAdd : public Operation
{
public:
	double GetResult() override
	{
		return numberA + numberB;
	}
};

class OperationSub : public Operation
{
public:
	double GetResult() override
	{
		return numberA - numberB;
	}
};
class OperationFactory
{
public:
	static std::unique_ptr<Operation> createOperation(char op)
	{
		switch (op)
		{
		case '+':
			return std::make_unique<OperationAdd>();
		case'-':
			return std::make_unique<OperationSub>();
		default:
			throw std::invalid_argument("Invalid operation type");
			break;
		}
	}
};
int main()
{
	try 
	{
		char op;
		std::cin >> op;

		auto operation = OperationFactory::createOperation(op);

		double a, b;
		std::cin >> a >> b;
		operation->setNumberA(a);
		operation->setNumberB(b);
		
		double result = operation->GetResult();
		std::cout << result << std::endl;
	}
	catch(const std::exception& ex)
	{
		std::cerr << "Error " << ex.what() << std::endl;
	}
	return 0;
}
```

---

## 策略模式（Strategy）

举例超市促销活动，有打八折、满300 - 100的活动等，如果给每一种活动都增加一个实现方法，重复度太高不方便维护。所以要将每个算法封装起来，并且可以互相替换，不同的策略表现出不同的行为，这就是策略模式。

**组成**：

- 环境角色（Context）：屏蔽客户端对策略算法的直接访问，封装可能存在的变化。

- 抽象策略角色（Strategy）：抽象类，给出所有具体策略类所需的接口。

- 具体策略角色（ConcreteStrategy）：具体的策略、算法实现。

  

```c++
class CashStrategy
{
public:
	virtual double calculate(double amount) const = 0;
	virtual ~CashStrategy() = default;
};

// 正常收费
class CashNormal : public CashStrategy
{
public:
	double calculate(double amount) const override
	{
		return amount;
	}
};

// 满300 -100
class CashReturn :public CashStrategy
{
private:
	double condition;
	double cashback;
public:
	CashReturn(double condition, double cashback) : condition(condition), cashback(cashback){}
	double calculate(double amount)const override
	{
		if (amount >= condition)
		{
			return amount - (int(amount / condition)) * cashback;
		}
		return amount;
	}
};

class CashRebate : public CashStrategy
{
private:
	double rebate;
public:
	CashRebate(double rebate):rebate(rebate){}
	double calculate(double amount)const override
	{
		return amount * rebate;
	}
};

class CashContext
{
private:
	std::unique_ptr<CashStrategy> strategy;
public:
	explicit CashContext(std::unique_ptr<CashStrategy> strategy) : strategy(std::move(strategy)){}
	double getResult(double amount) const
	{
		return strategy->calculate(amount);
	}
};

int main()
{
	double amount;
	std::cin >> amount;

	CashContext contextNormal(std::make_unique<CashNormal>());
	std::cout << "正常收费" << contextNormal.getResult(amount) << std::endl;

	CashContext contextReturn(std::make_unique<CashReturn>(300, 100));
	std::cout << "满300减100: " << contextReturn.getResult(amount) << std::endl;

	CashContext contextRebate(std::make_unique<CashRebate>(0.8));
	std::cout << "打八折: " << contextRebate.getResult(amount) << std::endl;

	return 0;
}
```

---

## 单一职责原则

就一个类而言，应该仅有一个引起它变化的原因。

如果一个类承担的职责过多，就等于把这些职责耦合在一起，一个职责的变化可能会削弱或者抑制这个类完成其他职责的能力。这种耦合会导致脆弱的设计，当变化发生时，设计会遭受到意想不到的破坏。



## 开放-封闭原则

对于扩展是开放的，对于更改是封闭的。

但无论模块是多么的封闭，都会存在一些无法对之封闭的变化。设计人员必须对他设计的模块应该对哪种变化封闭做出选择。他必须先猜测出最有可能发生的变化种类，然后构造抽象来隔离那些变化。

**面对需求，对程序的改动是通过增加新代码进行的，而不是更改现有的代码**。



## 依赖倒转原则

高层模块不应该依赖低层模块，两个都应该依赖抽象。

抽象不应该依赖细节，细节应该依赖于抽象。

总结：谁也不要依赖谁，除了约定的接口，大家都可以灵活自如。



## 里氏替换原则

**子类可以替换父类，并且程序的行为不会发生变化**。

- 子类必须完全实现父类的所有行为：子类不能违背父类提供的行为契约。
- 子类可以扩展父类的功能，但不能改变父类原有功能的本意。
- 子类替代父类时，不需要额外检查类型或进行特殊处理。

例子：正方形在数学中属于长方形，于是在代码中将正方形继承于长方形，但是宽高在设置的时候会出现冲突，导致最后计算面积出错。所以它不遵循里氏替换原则，需要将正方形、长方形类分离，实现共同的Shape接口。

---

## 装饰模式（Decorator Pattern）

**组成**：

- 抽象接口：定义核心功能的接口（或基类）。
- 具体组件（Concrete Component）：实现抽象接口的基本功能类。
- 装饰器基类（Decorator Base Class）：继承抽象接口，包含对另一个抽象接口对象的引用。
- 具体装饰器（Concrete Decorator）：扩展装饰器基类，通过额外的功能装饰具体组件。

---

## 享元模式（FlyWeight）

主要目的是通过共享对象来减少系统种对象的数量，**其本质就是缓存共享对象，降低内存消耗**。

享元模式将需要重复使用的对象分为两个部分：**内部状态**和**外部状态**。比如用户的账号是外部状态不能被共享，内部状态是可以共享的。

**组成**：

- 抽象享元角色（FlyWeight）：基类，定义出对象的外部状态和内部状态。
- 具体享元角色（ConcreteFlyWeight）：实现抽象享元类中的方法，是需要共享的对象类。
- 享元工厂（FlyWrightFactory）：使用一个map存储已经创建的享元对象。

例子：如围棋游戏，只需要存储两个棋子对象，而不是每盘棋创建几百个棋子对象，比如.Net中的String，相同的字符串只有一个实例。

---

## 单例模式（Singleton）

懒汉式：需要时（第一次调用GetInstance时）才会创建 线程不安全（需要加锁）

适用于可能会用到该实例。

> 怎么保证单例？

**让构造函数 `private`**，并提供 **静态方法 `GetInstance()`** 来获取唯一实例。

```c++
class Singleton {
private:
	static Singleton* instance;
	Singleton() { cout << "构造函数\n"; }

public:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

	static Singleton* GetInstance() {
		if (instance == nullptr) {
			instance = new Singleton();
		}
		return instance;
	}
	void Show() { cout << "Singleton实例方法\n"; }
};

Singleton* Singleton::instance = nullptr;
```

懒汉式线程安全加锁版：

```c++
class Singleton {
private:
	static Singleton* instance;
	static mutex mtx;
	Singleton() { cout << "构造函数\n"; }
public:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	static Singleton* GetInstance() {
		lock_guard<mutex> lock(mtx);
		if (instance == nullptr) {
			instance = new Singleton();
		}
		return instance;
	}
};
Singleton* Singleton::instance = nullptr;
mutex Singleton::mtx;
```

**懒汉式线程安全局部静态变量版：**

```c++
class Singleton {
private:
	Singleton() { cout << "构造函数\n"; }
public:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
	static Singleton* GetInstance() {
		// 对象会自动初始化 但是指针不会自动初始化 所以要初始化对象 返回对象的地址
		static Singleton instance;
		return &instance;
	}
};

void ThreadFunc(int id) {
	cout << "id = " << id << ",Singleton = " << Singleton::GetInstance() << endl;
}

int main() {
	thread t1(ThreadFunc, 1);
	thread t2(ThreadFunc, 2);
	t1.join();
	t2.join();
	return 0;
}
```



*饿汉式：*一定会用到该实例，程序在启动时就会创建实例。

```c++
#include<iostream>

using namespace std;

class Singleton {
private:
	static Singleton* instance;
	Singleton() {};
	~Singleton() {};

	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

public:
	static Singleton* GetInstance() {
		return instance;
	}
	static void DeleteInstance() {
		if (instance) {
			delete instance;
			instance = nullptr;
		}
	}
};
Singleton* Singleton::instance = new Singleton();

int main() {
	Singleton* a = Singleton::GetInstance();
	Singleton* b = Singleton::GetInstance();
	cout << a << ' ' << b << endl;
	return 0;
}
```

