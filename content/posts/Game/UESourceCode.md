

启动文件 `Launch.cpp`

**预初始化PreInit**，此时会发生：

- 对于每个UCLASS类，引擎会创建一个类默认对象（CDO），为实例提供默认值，注册完成后
- 调用StartupModule进行模块初始化、对应的有ShutdownModule函数（卸载模块清理资源）

---

主循环中**初始化阶段Init**

- 读取引擎配置文件来确定使用哪个GameEngine类作为核心引擎
- 创建GEngine实例（全局对象），可以通过它访问渲染、输入、音频等
- 创建了引擎，它就会被初始化，完成后引擎循环会触发一个全局委托以指示引擎现已初始化。
- 加载配置为延迟加载的模块/项目
- 加载地图（加载之前已经有了UGameEngine、GameInstance、GameViewportClient、LocalPlayer）

​	

1.其中在`GameEngine.cpp`中

> ​	引擎初始化阶段，主要做了些什么？

​	**1.1 创建GameInstance**

```c++
// Create game instance.  For GameEngine, this should be the only GameInstance that ever gets created.
{
	FSoftClassPath GameInstanceClassName = GetDefault<UGameMapsSettings>()->GameInstanceClass;
	UClass* GameInstanceClass = (GameInstanceClassName.IsValid() ? LoadObject<UClass>(NULL, *GameInstanceClassName.ToString()) : UGameInstance::StaticClass());
	GameInstance = NewObject<UGameInstance>(this, GameInstanceClass);
	GameInstance->InitializeStandalone();
}
```

​	**1.2 初始化视口GameViewportClient**

```c++
ViewportClient = NewObject<UGameViewportClient>(this, GameViewportClientClass);
ViewportClient->Init(*GameInstance->GetWorldContext(), GameInstance);
GameViewport = ViewportClient;
GameInstance->GetWorldContext()->GameViewport = ViewportClient;
```

​	**1.3 创建LocalPlayer**



2.其中在`UnrealEngine.cpp`中

> 加载地图时，主要做了什么？

​	2.1 触发一个全局委托表明地图即将更改，如果已经加载一个地图，会清理。FWorldContext用于跟踪当前加载的任何UWorld。

加载MapPackage（保存关卡时会被序列化为一个MapPackage（.umap））

初始化UWorld，设置系统（物理、AI等），创建一个GameMode Actor。

注册关卡中所有的Actor，在循环中迭代Actor，分别循环调用了关卡里所有Actor的PreInitializeComponents， InitializeComponents和PostInitializeComponents。

生成一个PlayerController