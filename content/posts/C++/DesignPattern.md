





#### 享元模式（FlyWeight）

主要目的是通过共享对象来减少系统种对象的数量，**其本质就是缓存共享对象，降低内存消耗**。

享元模式将需要重复使用的对象分为两个部分：**内部状态**和**外部状态**。比如用户的账号是外部状态不能被共享，内部状态是可以共享的。

**组成**：

- 抽象享元角色（FlyWeight）：基类，定义出对象的外部状态和内部状态。
- 具体享元角色（ConcreteFlyWeight）：实现抽象享元类中的方法，是需要共享的对象类。
- 享元工厂（FlyWrightFactory）：使用一个map存储已经创建的享元对象。

例子：如围棋游戏，只需要存储两个棋子对象，而不是每盘棋创建几百个棋子对象，比如.Net中的String，相同的字符串只有一个实例。

---

#### 单例模式（Singleton）

保证一个类只能产生一个实例，声明周期为整个程序的声明周期，通过Get访问到任何非静态方法。

懒汉式：需要时才会创建 线程不安全

饿汉式：一运行就创建实例，需要时直接调用。

```c++
class Singleton
{
private:
	// 不允许实例化 调用构造函数
	Singleton(){}
	static Singleton s_Instance;

public:
    // 删除类的拷贝构造函数 防止对象被复制
	Singleton(const Singleton&) = delete;
    // 删除赋值运算符
    Singleton& operator=(const Singleton&) = delete;
	// 返回单例实例
	static Singleton& Get()
	{
		return s_Instance;
	}
	void Func() {};
};
Singleton Singleton::s_Instance;


int main()
{
	// 不可以去掉引用 否则会拷贝 所以需要增加一个删除函数
	Singleton& instance = Singleton::Get();
	instance.Func();
	std::cin.get();
}

```

具体例子

```c++
class Random
{
private:

	Random(){}
	static Random s_Instance;
	float m_RandomGenerator = 0.5f;
	float IFloat() { return m_RandomGenerator; }

public:
	Random(const Random&) = delete;

	static Random& Get()
	{
		static Random instance;
		return instance;
	}
	static float Float() { return Get().IFloat(); }
};


int main()
{
	float number = Random::Float();
	std::cout << number << std::endl;
	std::cin.get();
}
```

