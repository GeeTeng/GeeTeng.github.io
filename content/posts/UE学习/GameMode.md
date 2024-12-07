---
title: "GameMode游戏模式"
date: 2024-11-23
tags: [UE5]
description: "课程笔记及作业"
showDate: true
math: true
chordsheet: true
---

### 笔记

---



#### 平行世界？World

UE中有三个世界

- 游戏世界 
- 编辑器世界 
- 预览世界

关卡蓝图-定义关卡规则

`UGameInstance`  存在于游戏的整个生命周期中 不会随着地图切换 销毁 适合业务全局管理操作，Uengine  管理 GameInstance

---

#### **游戏中最重要的实体AActor**

AActor 标志所有权的Owner指针，标志本地权限的Role枚举，作为网格同步的基础单位，根组件提供世界变化信息

Actor不只是3D中的表示，一些不可见对象也可以是Actor，如*AInfo*（派生类*Aworldsetting*，*AGameMode*，*AgameState*...）

**Actor在UE中就像是世界里的元素**

**Component 是 Actor 的组成部分，用于定义 Actor 的特性和行为**

1. *RootComponent*是Actor的核心组件，决定了Actor在场景中的位置、旋转和缩放
2. *SceneComponent* 是一个基础组件，主要用于定义场景中对象的位置、旋转和缩放。它本身是一个抽象组件，不会直接渲染或显示。

**Actor生命周期详解**

- 生命周期1

  actor分为

  - 静态的actor - 在关卡编辑器里直接放置的Actor  
  - 动态创建的actor（*spawnActor*） - 通过代码或蓝图在运行时动态生成的 Actor

- 生命周期2
  重要的生命周期函数

  BeginPlay - 在Actor激活时调用 初始化

  Tick - 每帧调用一次 处理逻辑

  EndPlay - 在Actor即将销毁时调用 清理资源

  

- 生命周期3
  GC完成收尾 注意有效性的判断

  ```c++
  if (IsValid(MyActor)) {    // Actor 有效 }
  
  if (MyActor && !MyActor->IsPendingKill()) {    // Actor 没有被标记为销毁 }
  ```

  *isPendingKill*是否已被标记为即将销毁，但尚未被垃圾回收系统（GC）完全移除。

---

#### **APawn（可操控的棋子)**

最重要的特性：可以被controller控制 提供了输入、移动框架的支持

UE提供了一个默认的Pawn - **DeflaultPawn** 

**默认三件套**

- ***DefaultPawnMovementComponent*** 
- **sphericalCollisionComponent**
-  ***StaticMeshComponent*** 



***SpectatorPawn*** 

观众想观战但并不需要真正的表示他们 所以提供了**USpectatorPawnMovement**（不带重力漫游） 并关闭了StaticMesh的显示，碰撞设置到了Spectator通道

---

**Character像是Pawn的加强特化版本**

**ACharacter**人形角色（人形Pawn）

- 近似仿真人形的胶囊体碰撞盒***UCapsuleComponent***

- 骨骼模型***USkeletalMeshComponent***

- 人物移动组件***UCharacterMovementComponent***

  - 配合胶囊体完成了各种仿真移动计算Walking、Falling...

  - 提供了Custom自定义移动模式供扩展

  - 网络游戏移动同步架构 主控端预表现（网断了还能跑）服务器端校验 模拟端预测

---

#### **MVC模式（显示、数据、算法）** 

**显示（View）**：

- 由骨骼模型和动画组件（`USkeletalMeshComponent`）负责呈现角色的外观。
- 包括实时动画播放、物理效果展示等。

**数据（Model）**：

- 角色的核心属性和状态（如速度、位置、旋转、是否跳跃）。
- 包括状态同步的网络消息结构和角色实例化的变量集合。

**算法（Controller）**：

- 负责移动逻辑、动画状态切换、物理计算等行为。
- 包括人物移动组件的各种模式扩展与自定义处理逻辑。

抽象这三个变化并归纳关系就是MVC模式



#### **继承和层次结构**

1. **AController 继承自 AActor**：
   - AController 是一个专门用于控制 APawn 行为的类。
   - 它充当“提线木偶操控者”，通过指令驱动角色的移动、行为和其他逻辑。
2. **UObject**：
   - Unreal Engine 中最基础的类，所有对象的根基。
   - **功能**：提供反射、序列化、垃圾回收（GC）和内存管理的机制。
   - **反射机制**：支持运行时类型信息 (RTTI) 和蓝图扩展。
   - **序列化**：用于保存和加载对象状态，广泛用于游戏保存、配置文件等。
3. **UActorComponent**：
   - **功能载体**：AActor 的组件化架构。
   - 逻辑代码或功能模块的载体（如健康系统、AI逻辑等）。
   - **嵌套能力**：USceneComponent 提供世界变换（位置、旋转、缩放）以及嵌套结构支持。
4. **AActor**：
   - 游戏对象的基础类，是组件的容器。
   - **功能**：管理生命周期（Spawn/Destroy）、事件（Tick/Overlap）以及和世界的交互（物理、渲染等）。
5. **APawn**：
   - AActor 的子类，表示玩家或 AI 可控制的实体。
   - 特点：
     - 提供基本的物理表示（如碰撞体）。
     - 支持玩家输入和基本移动能力。
     - 可被 AController 绑定以实现复杂逻辑。
   - 当前常用于角色控制和 AI 代理。

------

#### **具体功能与模块**

**APlayerController（提线木偶操控者）**：

- **定义**：玩家的控制代理，负责接收输入并将其转换为游戏行为。
- **职责：**
  - 处理输入：将玩家输入映射到动作或功能。
  - 控制 Pawn：通过控制接口驱动角色行为。
  - 管理玩家视角：负责管理摄像机、HUD 和其他 UI 组件。
- **网络连接与所有权**：
  - 在多人游戏中，APlayerController 维护客户端和服务器之间的连接。
  - 确保特定 APawn 的所有权归特定客户端（或服务器）所有。

**UInputComponent**：

- **功能**：用于绑定输入映射，将玩家输入转化为游戏事件。
- **特点**：
  - 支持蓝图和 C++ 输入绑定。
  - 使用输入映射表（Input Mapping Context）实现灵活的按键配置。

**APlayerCameraManager**：

- **职责**：
  - 管理玩家的摄像机视角和摄像机效果。
  - 支持视角切换（第一人称/第三人称）。
  - 应用于动态摄像机逻辑，如抖动、跟随目标、动态缩放等。

**`AHUD`**：

- **定义**：传统的屏幕 UI 渲染类。
- **特点**：
  - 早期用于绘制简单的屏幕元素（如血量、雷达等）。
  - 被更灵活的 UMG 取代，但仍可用作低级别绘制需求。
- **区别**：
  - HUD 是屏幕上的信息展示，通常绑定到玩家控制器。
  - UI 更复杂，可通过 UMG 实现交互式用户界面。

------

#### AGameMode 真·游戏模式

玩家如何加入游戏 游戏特定行为（获胜）
所有GameMode均为 AGameMode的子类 AGameModeBase包含大量可覆盖的基础功能

---

#### AgameState游戏状态

同mode基类子类

APlayerState玩家状态 
playerState Character、Controller的职责区别

---

#### Question？

> 如果你来设计游戏，以下数据应该存放在哪里，是否需要同步给玩家？
> 玩家血量、玩家得分、队友位置、毒圈空头信息

**1.玩家血量**：

玩家血量是游戏的核心状态 需要在服务器存储并管理（防止作弊）

需要实时同步给本地的UI更新，但是不需要同步给敌人玩家，在团队作战中可以同步给队友。

**2.玩家得分：**

玩家得分是游戏结果的重要数据，需要服务器去存储，也需要同步给玩家的UI，但是客户端仅需要展示不需要存储。自己的得分需要同步，其他玩家可以定期的同步，如排行榜。

**3.队友位置：**

存储在服务器和客户端

需要同步给同队玩家，不能同步给敌人

**4.空投信息：**

毒圈和空投是全局信息，在服务器存储，客户端需要同步给所有玩家

需要同步给所有玩家，同步半径变化，毒圈缩小等，

---

### 作业

---

创建一个空项目，新建C++Character类。

#### 创建摄像机

**PlayerCharaCharacter.h**文件

```c++
#include <GameFramework/SpringArmComponent.h>
class UCameraComponent;

protected:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;
```

**PlayerCharaCharacter.cpp**文件

```c++
#include "Camera/CameraComponent.h"
APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	/* camera boom */ 
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 180.0f;
	SpringArmComponent->bUsePawnControlRotation = true;

	/* camera */
	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
}
```

红色线是摄像机杆，之所以设置这个位置是为了在制作的过程中可以更方便观察角色，后面会改动至第一人称位置。

设置 Spring Arm（弹簧臂）组件，使其跟随玩家角色的旋转。当角色转动时，Spring Arm 会自动旋转，保持与角色的相对位置。

设置相机不随人物旋转，让视角保持固定位置。

![Camera](/images/GameMode/Camera.png)

---

#### 角色移动增强输入

创建InputAction(IA_Look和IA_Move)和InputMappingContext，并设置如下图。

![EnhancedInput](/images/GameMode/EnhancedInput.png)

**PlayerCharaCharacter.h**文件

```c++
#include "InputActionValue.h"
protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
```

**PlayerCharaCharacter.cpp**文件

```c++
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (const ULocalPlayer* Player = (GEngine && GetWorld()) ? GEngine->GetFirstGamePlayer(GetWorld()) : nullptr) 
    {
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Player);
		if (DefaultMappingContext) 
        {
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MoveVector = Value.Get<FVector2D>();
	if (Controller) 
    {
		// 获取控制器的旋转角度
		const FRotator Rotation = Controller->GetControlRotation();
		// 提取Yaw 水平旋转角度
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// 根据旋转角度计算前进方向
		// FRotationMatrix(YawRotation) 会生成一个旋转矩阵，表示角色的旋转状态
		// 通过 GetUnitAxis(EAxis::X)，我们从矩阵中获取角色在水平方向上的前进方向（即角色面朝的方向）
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// 根据旋转角度计算右侧方向
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// 将输入的 X 值映射到角色的前进方向
		AddMovementInput(ForwardDirection, MoveVector.X);
		// 将输入的 Y 值映射到角色的右侧方向
		AddMovementInput(RightDirection, MoveVector.Y);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	// 角色看向 视角旋转
	FVector2D LookVector = Value.Get<FVector2D>();
	if (Controller) 
    {
		AddControllerYawInput(-LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 确保输入组件是增强输入组件 绑定InputAction
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Fire);
	}
}

```

---

#### 角色动画

完成人物的移动后，需要加上动画效果，应该让人物在静止的时候是一个自然的状态，跑动的时候是跑动的动画。

新建一个C++ - AnimInstance类，并增加类的生命周期。

PlayerAnimInstance.h文件

```c++
UCLASS()
class FPSCPP_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	/* UAnimInstance类的虚函数 生命周期初始化 和 每一帧调用 */
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float Speed;
};
```

PlayerAnimInstance.cpp文件

```c++
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	
	if (PlayerCharacter == nullptr) return;
    // 得到角色的速度 设置z方向速度为0 因为不需要
	FVector Velocity = PlayerCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();
}
```

创建一个ABP_AnimInstance蓝图继承PlayerAnimInstance，Target Skeleton选择玩家的骨骼

创建BlendSpace1D用于平滑动画，选择玩家的骨骼。

在BlendSpace中设置水平轴名称为Speed最大值为350，Loop勾选上。在AssetBrowser中找到人物的走路动画和跑步动画拖动到下方窗口中。可以使用ctrl + shift + 鼠标单击看效果。

![BlendSpace1D](/images/GameMode/BlendSpace1D.png)

在蓝图中创建一个状态机（StateMachine 命名为IdleWalkRun）指向Output Pose。

双击进入到StateMachine中，新增一个状态IdleWalkRun，再次双击进去。

![ABP_AnimInstance](/images/GameMode/ABP_AnimInstance.png)

在角色蓝图中添加做好的AnimClass后，运行就可以看到角色有动画了，但是会发现鼠标移动转向的时候，角色的跑动方向也会发生变化。

我希望只有键盘会操纵角色的移动朝向，而不是鼠标。

![ControllerRotation01](/images/GameMode/ControllerRotation01.png)

使角色的朝向与其移动的方向一致。

![Movement](/images/GameMode/Movement.png)

---

#### 音效

导入音效资源，选取喜欢的一些开枪射击音效，右键Creat Sound Cue，制作一个随机生成且混合起来的音效。

这样音效就会从两组音效中随机挑出两个音效合成。当音源和听者之间的距离发生变化时，**Crossfade by Distance** 会动态调整不同音轨（或音频配置）的混合比例。

![SourceRandom](/images/GameMode/SourceRandom.png)

在这之前我们要创建一个InputAction-IA——Fire用于射击，方法与之前人物移动同理。

**PlayerCharacter.h文件**

```c++
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class USoundCue* FireSoundEffect;

```

**PlayerCharacter.cpp文件**

```c++
#include "Sound/SoundCue.h"
void APlayerCharacter::Fire(const FInputActionValue& Value)
{
	/* 开火声音 */ 
	if (FireSoundEffect) 
    {
		UGameplayStatics::PlaySound2D(this, FireSoundEffect);
	}
}
```

编译后暴露给蓝图，在蓝图中添加制作的SoundCue

![SoundCue](/images/GameMode/SoundCue.png)

---

#### 特效

首先要找到开火的位置，在gun_barrel下新建一个Socket，将枪口位置命名为GunBarrelSocket，确认枪口的坐标轴X方向指向枪口正对位置。

![e01d93502179c66d0006e93365bcde74_](/images/GameMode/GunSocket.png)

**PlayerCharacter.h文件**

```c++
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	class UParticleSystem* MuzzleFlash;
```

**PlayerCharacter.cpp文件 - Fire方法**

```c++
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Engine/SkeletalMeshSocket.h"
/* 获取开火枪口socket */ 
	const USkeletalMeshSocket* GunBarrelSocket = GetMesh()->GetSocketByName("GunBarrelSocket");
    if (GunBarrelSocket) 
    {
        const FTransform SocketTransform = GunBarrelSocket->GetSocketTransform(GetMesh());
        /* 开火粒子特效 */ 
        if (MuzzleFlash) {
            UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
        }
    }
```

在角色控制蓝图中添加特效

![MuzzleFlash](/images/GameMode/MuzzleFlash.png)

---

#### 射线检测

**PlayerCharacter.cpp文件 - Fire方法**

```c++
#include "DrawDebugHelpers.h"
	/* LineTrace Hit */
	// 存储碰撞结果
	FHitResult HitRst;
	// 开火位置为射线起始位置
	const FVector Start{ SocketTransform.GetLocation() };
	// 获取枪口的朝向 和 子弹发射的方向
	const FQuat StartRotation{ SocketTransform.GetRotation() };
	const FVector RotationAxis{ StartRotation.GetAxisX()};
	// 确认射线的终点位置
	const FVector End{ Start + RotationAxis * 500000.f };
	// 碰撞结果 射线起始位置 射线终点位置 碰撞通道射线检测
	GetWorld()->LineTraceSingleByChannel(HitRst, Start, End, ECollisionChannel::ECC_Visibility);

	// 如果碰撞就发出一条描绘出来的射线
	if (HitRst.bBlockingHit)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, 0, 2);
		DrawDebugSphere(GetWorld(), HitRst.Location, 10, 10, FColor::Blue, 0, 2);
	}
```

![FireLineTrace](/images/GameMode/FireLineTrace.gif)

增加弹道轨迹和碰撞到物体的特效，隐藏掉绘制的射线。

在资源包中找到一个合适的弹道轨迹的Texture，新增一个Material材质，将Texture添加进去。

新增一个Cascade Particle System，新增Target、Source，对各项进行一些设置，如：Target最小输入输出、最大输入输出、Lifetime、Required中的Material、Kill on Deativate、Kill on Completed等。

![WeaponTrail](/images/GameMode/WeaponTrail.png)

![ParticleSystem](/images/GameMode/ParticleSystem.png)

**PlayerCharacter.cpp文件 - Fire方法**

```c++
	/* Impact Effect */ 
	if (HitRst.bBlockingHit)
	{
		if (ImpactEffect) 
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, HitRst.Location);
		}
	}

	/* 子弹发射轨迹 */
	if (BeamParticles) 
	{
		FVector BeamEndPoint{ FVector::ZeroVector };

		if (HitRst.bBlockingHit)
		{
			BeamEndPoint = HitRst.Location;
		}

		UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticles, SocketTransform);
		if (Beam) 
		{
			UE_LOG(LogTemp, Warning, TEXT("Beam->SetVectorParameter()"));
			Beam->SetVectorParameter(FName("Target"), BeamEndPoint);
		}

	}
```

---

#### 更改第一人称视角

将角色蓝图中Pawn - Use Controller Rotation Yaw勾选

将摄像机调整到人物的头部位置，Mesh - Owner No See勾选

添加准星，新建HUD蓝图，在World Setting中设置HUD Class为新建的蓝图

准星位置为屏幕大小 / 2 - 准星大小 / 2

![CrossHUD](/images/GameMode/CrossHUD.png)

更改发射位置，起点为枪口位置，终点为准星的屏幕坐标转化成世界坐标位置延长点。

```c++
    // 获取枪口位置（起点）
    FVector Start = SocketTransform.GetLocation();
    // 获取玩家控制器
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        // 获取屏幕中心准星位置
        FVector2D ScreenCenter = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY()) / 2;
		// StartWorld 是由 DeprojectScreenPositionToWorld 函数赋值的
        // 将屏幕空间中的位置（例如屏幕中心）转换为世界空间中的位置
        FVector StartWorld, Direction;
        // 将屏幕中心位置转换为世界空间中的位置和方向
        if (PlayerController->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, StartWorld, Direction))
        {
            FVector End = StartWorld + Direction * 10000.0f;
            FHitResult HitRst;
            GetWorld()->LineTraceSingleByChannel(HitRst, Start, End, ECollisionChannel::ECC_Visibility);
        }
    }
```

创建Cube蓝图类和C++类CubeBeShot，加入缩小方法和销毁方法

```c++
	// .h中
	UFUNCTION(BlueprintCallable, Category = "Cube")
	void Shrink();

	UFUNCTION(BlueprintCallable, Category = "Cube")
	void DestoryCube();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shooting")
	bool bHasBeenShotOnce = false;
	// cpp中
    void ACubeBeShot::Shrink()
    {
        FVector CurrentScale = GetActorScale3D();
        SetActorScale3D(CurrentScale * 0.5f);
    }

    void ACubeBeShot::DestoryCube()
    {
        Destroy();
    }
```

新建UserWidget蓝图类

简单添加一个TextBlock组件

![Score](/images/GameMode/Score.png)

**PlayerUserWidget.h文件**

```c++
UPROPERTY(EditAnywhere, meta = (BindWidget))
class UTextBlock* TextScore;

// 获取当前分数并返回文本
UFUNCTION(BlueprintCallable, Category = "UI")
FText GetScoreText() const;
```

**PlayerUserWidget.cpp文件**

写下获取玩家分数显示在屏幕上

```C++
FText UPlayerUserWidget::GetScoreText() const
{
    APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

    if (Player)
    {
        return FText::AsNumber(Player->GetScore());
    }

    // 如果没有获取到玩家角色，返回 0
    return FText::AsNumber(0);
}
```

**PlayerCharacter.h文件**

```c++
protected:
	float ShootCooldown = 0.2f;
	float LastShootTime;
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
    int32 AmountScore;

    // 指向 ScoreWidget 的类引用
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UPlayerUserWidget> UserWidgetClass;
    // 增加分数的方法
    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Score);

    // 获取分数的 getter 方法
    UFUNCTION(BlueprintCallable, Category = "Score")
    int32 GetScore() const;  // 返回分数
    UPlayerUserWidget* ScoreWidget;
```

**PlayerCharacter.cpp文件 - Fire方法**

```c++
// 设置开火间隔 避免连续射击方块直接销毁
float CurrentTime = GetWorld()->GetTimeSeconds();
if(CurrentTime - LastShootTime < ShootCooldown) {
        return;
}
LastShootTime = CurrentTime;

	AActor* HitActor = HitRst.GetActor();
    // 如果射线碰撞到物体
    if (HitRst.bBlockingHit && HitActor->IsA(ACubeBeShot::StaticClass()))
    {
        ACubeBeShot* Cube = Cast<ACubeBeShot>(HitActor);
        UE_LOG(LogTemp, Log, TEXT("Cube hit, bHasBeenShotOnce: %s"), Cube->bHasBeenShotOnce ? TEXT("True") : TEXT("False"));
        if (Cube)
        {
            // 判断是否是第一次射击
            if (!Cube->bHasBeenShotOnce)
            {
                // 第一次射击：缩小
                Cube->Shrink();
                // 标记被射击了一次
                Cube->bHasBeenShotOnce = true;
            }
            else
            {
                // 第二次射击：销毁
                // 随机生成 10 或 20 分
                int32 RandomScore = FMath::RandBool() ? 10 : 20;;
                AddScore(RandomScore);
                Cube->DestoryCube();

            }
        }
    }

```

在BeginPlay中绘制widget界面

```c++
if (UserWidgetClass)
{
    ScoreWidget = CreateWidget<UPlayerUserWidget>(GetWorld(), UserWidgetClass);
    if (ScoreWidget) {
        ScoreWidget->AddToViewport();
    }
}
```

以及得分方法

```c++
void APlayerCharacter::AddScore(int32 Score)
{
    AmountScore += Score;
}
int32 APlayerCharacter::GetScore() const
{
    return AmountScore;
}
```

最终效果图如下

![CubeScore](/images/GameMode/CubeScore.gif)

本次作业遗留下的改进点：

1. 在射击到Cube加分时，我使用的是随机生成一个分数，百分之五十的概率获取10分或者20分，如果更换成设置一些特殊的方块，比如其他颜色更大，就像游戏中的boss一样，每一个方块有自己的value，通过销毁然后获取value，效果会更好。

2. Cube如果开启了Simulate Physics，第一次射击看不到缩小的效果，两次射击后直接销毁。所以我没有增加物理效果，之后有待研究这个问题。
3. 人物从第三人称改成第一人称视角后，弹道轨迹变得很丑。
4. 第一人称视角应该看到手臂和枪的样子，有待改进。

