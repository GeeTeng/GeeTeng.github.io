---
title: "Warrior项目笔记 1"
date: 2025-11-05
tags: [UE5]
description: "Warrior项目笔记第一部分"
showDate: true
math: true
chordsheet: true
---



## Hard Reference & Soft Reference

**Reference Viewer** 引用查看器显示的白色线条是硬引用关系，粉色线条是软引用关系。比如一个蓝图在属性里引用了某个材质、贴图、Mesh等。

如下图所示，右边的所有内容是角色的依赖项，左边的所有内容依赖于角色资产：

![1761894933114](/images/UE/WarriorProject/0.png)



硬引用示例：

1. 蓝图的 `Cast to XXXX` 节点会导致硬引用对象，即使存在但不使用该节点也会硬引用cast对象。这是因为：当你使用 `Cast To BP_XXX` 节点时，蓝图编译器会在生成的 bytecode 里记录该蓝图类的引用（`UBlueprintGeneratedClass` 对象）以及`SpawnActorFromClass`也会导致硬引用。
2. 蓝图变量引用资源或者继承自另一个BP类。
3. C++中使用FObjectFinder，构造期强制加载资源。
4. DataTable / DataAsset 直接存放UObject*

​	....

总之，只要蓝图或C++中有明确的对象引用或类引用（即编译器能追踪到的UObject、UClass），都会形成硬引用。



**SizeMap** 如下图所示， *BP_ThirdPersonCharacter* 占据了207.4MB，其中*SKM_Quinn_Simple*占据了198.6MB，像一个盒子层层嵌套，你可以观察到内存占比最大的是哪个引用：

![1761894985945](/images/UE/WarriorProject/1.png)



**硬引用和软引用的区别是** —— 硬引用可以更快速的访问对象，对象已经被加载到内存中，所以可以直接使用；而软引用可以减少内存占用，但是速度会比较慢，如果被使用，先要访问是否存在，才能访问它指向的对象。所以相当于内存换取了速度。



> 避免硬引用的方法

使用软引用（SoftObjectPtr / SoftClassPtr）、接口可以避免硬引用、使用字符串路径 + 动态加载。



> 什么时候用软引用什么时候用硬引用

应该在关键对象上使用硬引用（GameMode、PlayerController等） 还有只会加载一次的资源地形等。

对于暂时不需要且体积大的对象用软引用，比如武器库、剧情章节等。



对象指针 `TobjectPtr` 虽然也属于硬引用，但相比传统 `UObject*`，它的确能**减少一些不必要的加载链触发和GC压力**，因为它底层是句柄索引，普通指针在 GC 或序列化时可能触发对象加载，而 `TObjectPtr` 通过索引追踪，避免了这种不必要的加载链。

在发布构建的时候会转化原始指针，为了提升运行时性能（少一次句柄解析开销），减少内存占用（不需要额外维护句柄表引用）



## 输入绑定流程

Native Gameplay Tags 创建全局访问的GameplayTags

Input Config Data Asset 创建DataAsset用于映射gameplaytag--->input action

Custom Input Component 自定义输入组件

Binding Inputs 定义input callbacks

Assign assets in editor 

### GameplayTags全局访问

创建WarriorGameplayTags文件，用于集中管理标签。

```c++
#include "NativeGameplayTags.h"

namespace WarriorGameplayTags
{
	/** Input Tags **/
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move) // 声明一个外部可访问的FGameplayTag
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look)
}

```

```c++
#include "WarriorGameplayTags.h"

namespace WarriorGameplayTags
{
	/** Input Tags **/
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Move, "InputTag.Move") // 创建 FGameplayTag 对象并初始化
	UE_DEFINE_GAMEPLAY_TAG(InputTag_Look, "InputTag.Look") // 第一个参数是c++中的名称 第二个是编辑器中看到的名字
}
```

### Input Config Data Asset

之后创建一个DataAsset的cpp文件

定义结构体，每个Tag关联一个InputAction。

```c++
USTRUCT(BlueprintType)
struct FWarriorInputActionConfig
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Category = "InputTag"))
	FGameplayTag InputTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputAction;
};
```

并且在该文件中，DataAsset中有一个输入映射和包含很多个`FWarriorInputActionConfig`结构体的数组。

```c++
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
UInputMappingContext* DefaultMappingContext;

UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
TArray<FWarriorInputActionConfig> NativeInputActions;
```

![2](/images/UE/WarriorProject/2.png)

并且还有一个通过gameplaytag去查找inputaction的函数`FindNativeInputActionByTag`

### 自定义输入组件

创建一个WarriorInputComponent文件用于自定义输入组件代替增强输入组件，在其中定义一个模板内联函数用于BindAction

```c++
public:
	template<class UserObject, typename CallbackFunc>
	void BindNativeInputAction(const UDataAsset_InputConfig* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func);
	
};
template<class UserObject, typename CallbackFunc>
inline void UWarriorInputComponent::BindNativeInputAction(const UDataAsset_InputConfig* InInputConfig, const FGameplayTag& InInputTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func)
{
	checkf(InInputConfig,TEXT("InputConfigDataAsset is null"));
	if (UInputAction* FindAction = InInputConfig->FindNativeInputActionByTag(InInputTag))
	{
		BindAction(FindAction, TriggerEvent, ContextObject, Func);
	}
}
```



### 绑定输入定义callbacks

创建虚函数继承SetupPlayerInputComponent，在其中获取本地玩家增强输入子系统，添加输入映射上下文，然后再通过自定义输入组件绑定在角色类中定义的callback函数，分别是`Input_Move`和`Input_Look`。

这样角色就实现了移动和视角。

```c++
void AWarriorHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	checkf(InputConfigDataAsset, TEXT("Forget to assign inputconfig data asset."));
	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	check(Subsystem);
	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);
	UWarriorInputComponent* WarriorInputComponent = CastChecked<UWarriorInputComponent>(PlayerInputComponent);
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset, WarriorGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset, WarriorGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::Input_Look);
}

void AWarriorHeroCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MoveVector = InputActionValue.Get<FVector2D>();
	const FRotator MoveRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
	if (MoveVector.Y != 0.f)
	{
		// 把世界坐标系下的“前方向量”(0,1,0)用 MoveRotator（控制器朝向）旋转一下，得到角色当前朝向的前方方向
		const FVector ForwardVector = MoveRotator.RotateVector(FVector::ForwardVector);
		AddMovementInput(ForwardVector, MoveVector.Y);
	}
	if (MoveVector.X != 0.f)
	{
		const FVector RightVector = MoveRotator.RotateVector(FVector::RightVector);
		AddMovementInput(RightVector, MoveVector.X);
	}
}

void AWarriorHeroCharacter::Input_Look(const FInputActionValue& InputActionValue)
{
	const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
	if (LookAxisVector.X != 0.f)
	{
		AddControllerYawInput(LookAxisVector.X);
	}
	if (LookAxisVector.Y != 0.f)
	{
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

```



## 角色动画

创建一系列C++文件分别是

WarriorBaseAnimInstance继承AnimInstance

|——WarriorHeroLinkedAnimLayer

|——WarriorCharacterAnimInstance

​	  |——WarriorHeroAnimInstance



首先在WarriorCharacterAnimInstance中定义两个函数

```c++
virtual void NativeInitializeAnimation() override;
// 这个函数是动画系统的线程安全更新入口，用于在多线程模式下在工作线程中执行动画更新逻辑，从而分担主线程负载、提升性能
virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
```

在初始化动画函数中获取当前角色和角色的移动组件存入到OwningCharacter和OwningMovementComponent中。

在线程安全更新函数中去获取角色当前地面速度和是否有加速度。这两个变量中的加速度用于切换动画状态机的静止和移动状态，而地面速度用于BlendSpace混合空间处理奔跑动画。

```c++
void UWarriorCharacterAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter =  Cast<AWarriorBaseCharacter>(TryGetPawnOwner());
	if (OwningCharacter)
	{
		OwningMovementComponent = OwningCharacter->GetCharacterMovement();
	}
}

void UWarriorCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!OwningCharacter || !OwningMovementComponent)
	{
		return;
	}
	GroundSpeed = OwningCharacter->GetVelocity().Size2D();
	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;
}
```

因此现在我们有了两个状态，在动画蓝图ABP_Hero继承自WarriorHeroAnimInstance分别是Idle和Jog。

继续添加一个新状态Relax，用于在角色静止达到一定时间后切换一个放松的动作，在状态机中通过RandomSequencePlayer达到随机多个relax动画的效果。

在WarriorHeroAnimInstance文件中这个新状态的过渡条件是通过C++中的`bShouldEnterRelaxState`，同时有EnterRelaxStateThreshold和IdleElapsedTime变量，分别是进入到放松状态所需时间和目前经过了多久的时间。

```c++
void UWarriorHeroAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (OwningCharacter)
	{
		OwningHeroCharacter = Cast<AWarriorHeroCharacter>(OwningCharacter);
	}
}

void UWarriorHeroAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	if (bHasAcceleration)
	{
		bShouldEnterRelaxState = false;
		IdleElapsedTime = 0.f;
	}
	else
	{
		IdleElapsedTime += DeltaSeconds;
		bShouldEnterRelaxState = (IdleElapsedTime >= EnterRelaxStateThreshold);
	}
}
```

