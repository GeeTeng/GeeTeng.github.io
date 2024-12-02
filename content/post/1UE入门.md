---
title: "UE_入门" 

date: 2024-11-16 

draft: false
---


title: "GAMES101_Work01" 

date: 2024-11-13 

draft: false

#### 安装UE5.4.4源码

##### 1 在GitHub上下载UE5.4.4源码

在这之前需要关联账号并验证至虚幻引擎

[EpicGames/UnrealEngine at 5.4.4-release](https://github.com/EpicGames/UnrealEngine/tree/5.4.4-release)

我直接下载文件压缩包，没有使用git拉取，原因是网络不稳定



##### 2 生成sln文件 

解压文件，并将文件夹改名为UE，以防后续因为文件名太长而出错

启动Setup.bat文件  注意：网络一定要通畅并且不能自行关闭 安静等待...

之后运行GenerateProjectFiles.bat文件 会生成一个UE5.sln文件



##### 3 配置VS

一定要增加UnrealEngine安装程序 建议VisualStudio2022 17.8版本

因为之前试过17.12版本发现一些NuGet包无法还原 不知道什么原因

打开UE5.sln 在右侧会提示安装插件 点击后可以安装额外的插件 配置如下：

![VSConfig](/images/UE入门/VSConfig.png)



##### 4 生成UE5

将UE5设为启动项 点击生成UE5 不要点生成解决方案

为了使编译速度快一些 我试过使用Incredibuild 速度是很快 但是会发现有一些文件属于嵌套关系 并行编译时会出错 于是乎换用其他方式

"C:\Users\用户名\AppData\Roaming\Unreal Engine\UnrealBuildTool\BuildConfiguration.xml"修改BuildConfiguration.xml文件

```xml
// 指定构建时可以同时运行8个编译任务
<MaxParallelActions>8</MaxParallelActions>
// 设置最少需要使用 PCH 文件的源文件数量
<MinFilesUsingPrecompiledHeader>6</MinFilesUsingPrecompiledHeader>
```

这样就将编译时间从五六个小时 缩短到了一个小时左右

切记：

不要参照网上exclude一些VS2013等文件 会导致之后因为奇怪的原因无法运行 白白浪费时间

成功生成UE5后不要再点任何重新生成解决方案/项目 否则会清理掉几个小时编译的文件

编译完成后会生成这样的文件结构 exe文件位于\Engine\Binaries\Win64中 双击即可运行

![UEFileStructure](/images/UE入门/UEFileStructure.png)



##### 5 新建C++项目

创建一个第一人称模板项目 选择C++

点击创建之后会跳出vs界面

点击执行 会加载出UE5界面 第一次加载时间会很长 之后时间会缩短一些

![creatProject](/images/UE入门/creatProject.png)

将General - SourceCode设置为VisualStudio2022编辑器

在下方内容区可以右键创建C++ Class

我没有关闭实时编译Living Code 但是新建项目后可以在偏好设置中 将AssetEditorOpenLocation设置为MainWindow

![createCppClass](/images/UE入门/createCppClass.png)

##### 6 打包

选择Platform后的windows平台 其中Development是方便调试的打包 而Shipping是发布的打包

这里我没有选择打包Android平台 是因为我没有安卓手机 所以就只打包了windows平台

在outputLog窗口中等待一段时间会出现BUILD SUCCESSFUL 代表打包成功

![package](/images/UE入门/package.png)
