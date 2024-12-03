---
title: "UE-c++"
date: 2024-12-3
tags: [UE5]
description: "C++学习笔记"
showDate: true
math: true
chordsheet: true
---

#### 设置摄像机位置

```c++
AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// 创建一个相机臂 用来控制角色和相机的相对位置
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	// 将相机臂附着到角色的根组件
	CameraBoom->SetupAttachment(RootComponent);
	// 相机杆长度
	CameraBoom->TargetArmLength = 600.f;
	// 设置俯仰角 pitch yaw roll
	CameraBoom->SetRelativeRotation(FRotator(-40.f, 0.f, 0.f));
	// 相机附着在boom
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>("Player Camera");
	PlayerCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// 附着某一位置
	//PlayerCamera->SetupAttachment(GetMesh(), FName("RootSocket"));
}

```

---

#### 虚幻打印的两种方式


```c++
UE_LOG(LogTemp, Warning, TEXT("CameraBoom : TargetArmLength: %f"), CameraBoom->TargetArmLength);
// 消息的唯一标识 打印10s color
GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, FString::Printf(TEXT("CameraBoom : TargetArmLength: %f"), CameraBoom->TargetArmLength));
```

---

#### 增强输入 - 人物移动

在 Unreal Engine 中，角色和摄像机的旋转是通过 **控制器**（`Controller`）来处理的，控制器会记录角色的旋转角度，通常由玩家的输入（如鼠标、右摇杆等）或者 AI 控制。

- **`Yaw`**：水平旋转，表示角色面朝的方向。例如，`Yaw` = 0 时，角色正朝向世界的正前方；`Yaw` = 90 时，角色面朝右方。
- **`Pitch`**：垂直旋转，表示角色视角的俯仰角度。例如，`Pitch` = 0 时，角色眼睛水平；`Pitch` = 90 时，角色会向上看；`Pitch` = -90 时，角色会向下看。
- **`Roll`**：绕角色前后轴的旋转，通常我们不太关注，因为它并不影响角色的视角或朝向。

```c++
// 引入增强输入的头文件
#include <EnhancedInputSubsystems.h>
#include <EnhancedInputComponent.h>
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 获取当前关卡中的第一个本地玩家
	if (const ULocalPlayer* Player = (GEngine && GetWorld()) ? GEngine->GetFirstGamePlayer(GetWorld()) : nullptr){
		// 获取增强输入子系统 用于处理输入映射
		UEnhancedInputLocalPlayerSubsystem* Subsystem= ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Player);
		// 如果设置了输入映射上下文 就添加到子系统中
		if (DefaultMapping) {
			Subsystem->AddMappingContext(DefaultMapping, 0);
		}
	}
}
void AMyCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MoveVector = Value.Get<FVector2D>();
	if (Controller) {
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
void AMyCharacter::Look(const FInputActionValue& Value)
{
	// 角色看向 视角旋转
	FVector2D LookVector = Value.Get<FVector2D>();
	if (Controller) {
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}
// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// 确保输入组件是增强输入组件
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
	}
}


```

---

#### 动画Anim Instance - 实现人物的跑动

```c++
	// 引入GetCharacterMovement
	#include<GameFramework/CharacterMovementComponent.h>	
	// 1 不要让角色随着控制器旋转
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// 2 相机杆旋转 相机不旋转
	CameraBoom->bUsePawnControlRotation = true;
	PlayerCamera->bUsePawnControlRotation = false;

	// 3 角色要根据其运动方向进行旋转
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
```

在编译完成后，需要在角色蓝图中检查是否为设置状态，如果不是，则需要取消勾选

新建一个Anim Instance c++类

```c++
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"
public:
	// 相当于BeginPlay
	virtual void NativeInitializeAnimation() override;
	// 相当于Tick
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AMyCharacter> PlayerCharacter;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCharacterMovementComponent> PlayerCharacterMovement;

	UPROPERTY(BlueprintReadOnly)
	float Speed;
```

```c++
#include "MyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UMyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	// 获取player
	PlayerCharacter = Cast<AMyCharacter>(TryGetPawnOwner());
	if (PlayerCharacter) {
		PlayerCharacterMovement = PlayerCharacter->GetCharacterMovement();
	}
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (PlayerCharacterMovement) {
		Speed = UKismetMathLibrary::VSizeXY(PlayerCharacterMovement->Velocity);
	}
}
```

新建一个存放动画蓝图的Anim文件夹，并在文件夹中创建一个动画蓝图 Animation - AnimationBlueprint

新建一个状态机，双击进到状态机里面 AddState 加入Run Idle两个状态

双击这两种状态 如果速度大于0则切换到奔跑状态 速度等于0则切换到静止状态

---
