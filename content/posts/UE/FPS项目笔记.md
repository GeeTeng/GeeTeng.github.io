---
title: "FPS游戏项目制作笔记"
date: 2025-04-15
tags: [UE5]
description: "记录游戏中的核心代码和解决过的问题"
showDate: true
math: true
---





---



## 枪械

考虑到不同枪有不同后坐力，所以在ShooterGun.h中创建了结构体用来存储枪械的后坐力。

```c++
USTRUCT(BlueprintType)
struct FRecoilConfig
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	float VerticalBase = -0.1f; // 基础垂直后坐力

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	float VerticalRandomRange = 0.02f; // 垂直随机扰动范围

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	float HorizontalRandomRange = 0.05f; // 水平随机扰动范围
};
```

在该函数中改变控制器的Pitch和Yaw偏移量

```c++
void AShooterGun::MakeRecoilCPP()
{
	AShooterCharacter* Character = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (Character)
	{
		// 根据不同枪设置不同后坐力
		float FinalPitch = RecoilConfig.VerticalBase + FMath::FRandRange(-RecoilConfig.VerticalRandomRange, RecoilConfig.VerticalRandomRange);

		float FinalYaw = FMath::FRandRange(-RecoilConfig.HorizontalRandomRange, RecoilConfig.HorizontalRandomRange);

		Character->AddControllerPitchInput(FinalPitch);
		Character->AddControllerYawInput(FinalYaw);
	}
}
```

添加镜头抖动

![01](/images/UE/FPS/01.png)



换弹必须要在子弹小于最大容量并且不在开火时换弹。

并且在换弹夹时，改变弹夹的可见性。

开火时，如果子弹剩余<= 0则不能发射子弹，会出现空枪开火的声音。开火会在枪口前方生成子弹，`FireHitscan`的`LineTraceSingleByChannel`用来检测是否hit到物体了。枪口和射击点会生成开火的粒子特效。

当按下射击键时，立刻调用`Fire`方法发射第一发子弹，然后调用`RegisterNextFireCPP()`，设置一个定时器使每隔一段时间就调用`Fire`方法，当玩家松开射击键时，`FireReleaseCPP()` 方法将 `IsFirePressedCPP` 设置为 `false`，并停止定时器，终止连发。

```c++
// 注册下一次开火的定时器
void AShooterGun::RegisterNextFireCPP(float Duration)
{
	TimerFireCPP = UKismetSystemLibrary::K2_SetTimer(this, TEXT("Fire"), Duration, false);
}
// 清除 RegisterNextFireCPP 中注册的定时器 防止继续自动射击
void AShooterGun::FireReleaseCPP()
{
	IsFirePressedCPP = false;
	GetWorld()->GetTimerManager().ClearTimer(TimerFireCPP);
}
// 判断是否在射击
bool AShooterGun::IsFiringCPP()const
{
	return IsFirePressedCPP && GetWorld()->GetTimerManager().IsTimerActive(TimerFireCPP);
}
```



## 排行榜

### 排行榜更新

之前写的排行榜顺序查找插入位置，所以优化了一下，利用二分查找法，将时间复杂度从 *O(N)* 降到 *O(logN)*。

```c++
void AShooterGameMode::UpdateRankingCPP()
{ 
	// 本地排行榜更新函数，排行榜最多保留前10名玩家分数。
	// 排行榜是按分数降序排列的，使用了 Algo::LowerBound 来进行二分查找插入位置，从而将查找复杂度优化到 O(log n)
	const int32 MaxRankingCount = 10;
	if (RankingScoresCPP.Num() >= MaxRankingCount && ScoreCPP <= RankingScoresCPP.Last())
	{
		RankingCPP = -1; // 未上榜
		return;
	}
    // lambda表达式自定义排序比较
	int32 RankingIndex = Algo::LowerBound(RankingScoresCPP, ScoreCPP, [](float A, float B) {
		return A > B;
		});
	RankingCPP = RankingIndex + 1;
	RankingScoresCPP.Insert(ScoreCPP, RankingIndex);
	RankingTimesCPP.Insert(EndTimeCPP.ToString(), RankingIndex);
	if (RankingScoresCPP.Num() > MaxRankingCount) 
	{
		RankingScoresCPP.RemoveAt(MaxRankingCount);
		RankingTimesCPP.RemoveAt(MaxRankingCount);
	}
}
```



### 排行榜显示

UI生命周期函数 `NativeConstruct()`和`NativeDestruct()`

其中NativeConstruct()这个函数会在 **UI Widget 被完全创建并初始化后** 被调用，可以安全地访问和初始化控件、绑定事件等。这个函数通常用于在 UI 加载完成后，做一些数据绑定和界面更新。。

而NativeDestruct()当 `UUserWidget` 被销毁时，`NativeDestruct()` 会被调用

```c++
//  NativeConstruct() 是确保控件树已经被创建并且初始化完的地方
// 如果你在 BeginPlay() 或者其他时机进行初始化，可能 UI 还没有完全加载好
void UShooterResultWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InitRankingCPP();
}
void UShooterResultWidget::InitRankingCPP()
{
	// 在游戏模式中创建函数返回排名 分数 时间
	AShooterGameMode* GameMode = Cast<AShooterGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		// 获取排名 分数 时间
		int32 Ranking = GameMode->GetRankingCPP();
		TArray<float> Scores = GameMode->GetRankingScoresCPP();
		TArray<FString> Times = GameMode->GetRankingTimesCPP();
		// 设置TextRanking为指定字符串
		FString Text = TEXT("Your ranking = #") + FString::FromInt(Ranking);
		TextRanking->SetText(FText::FromString(Text));

		// 遍历每个玩家显示排行榜数据
		for (int32 Index = 0; Index < Scores.Num(); ++Index)
		{
			// 创建排名 分数 时间控件 用 WidgetTree 来动态构建 UI 元素
			int32 Row = Index + 1;
			// 用 WidgetTree 在运行时动态创建一个 UTextBlock 控件
			UTextBlock* Rank = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
			if (Rank)
			{
				Rank->SetText(FText::AsNumber(Row));
				UGridSlot* ChildSlot = RankingTable->AddChildToGrid(Rank, Row, 0);
				ChildSlot->SetNudge(FVector2D(0.0, 20.0 * Row));
			}
			UTextBlock* Score = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
			if (Score)
			{
				Score->SetText(FText::AsNumber(Scores[Index]));
				UGridSlot* ChildSlot = RankingTable->AddChildToGrid(Score, Row, 1);
				ChildSlot->SetNudge(FVector2D(350.0, 20.0 * Row));
			}
			UTextBlock* Time = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
			if (Time)
			{
				Time->SetText(FText::FromString(Times[Index]));
				UGridSlot* ChildSlot = RankingTable->AddChildToGrid(Time, Row, 2);
				ChildSlot->SetNudge(FVector2D(700.0, 20.0 * Row));
			}
		}
	}
```



## GameMode

定义游戏玩法，逃脱时间 = endtime - starttime，得分将逃脱时间 - 120s，如果＞0则调用`AddScore`，然后调用`UpdateRankingCPP`、`SaveRanking`和`CallOnEndGameCPP`。

其中`SaveRanking`是创建了SaveGame存储得分和逃脱时间的数组，`SaveGameToSlot`将数据保存到磁盘中。等加载排行榜的时候再`Load Game from Slot`。



## 游戏结束的多播委托

在GmeMode中定义一个多播委托

```c++
// 声明动态多播委托的宏 委托类型名称 拥有该委托的类 委托变量名
DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE(FShooterGameModeOnEndGame, AShooterGameMode, OnEndGameCPP);
public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FShooterGameModeOnEndGame OnEndGameCPP;
```

在PlayerController中定义OnEndGameCPP函数用于显示排行榜（ResultWidget），在BeginPlay中绑定委托。

```c++
void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	AShooterGameMode* GameMode = Cast<AShooterGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		// 使用了 动态多播委托（Dynamic Multicast Delegate） 来调用 OnEndGameCPP
		GameMode->OnEndGameCPP.AddUniqueDynamic(this, &AShooterPlayerController::OnEndGameCPP);
	}
}
```

当游戏结束后，GameMode会通过广播委托来通知所有绑定了该委托的函数，一旦广播OnEndGameCPP就会被调用。



## PlayerCharacter

角色状态 —— 静止、瞄准、装弹、换枪、跑步

```c++
UENUM(BlueprintType)
enum class StateOfCharacterCPP : uint8 {
	Idle,
	Aiming,
	Reloading,
	Swapping,
	Running,
};
```



*角色跑步状态*

```c++
float AShooterCharacter::GetSpeedCPP() const
{
	return GetVelocity().Length();
}

bool AShooterCharacter::CanRunCPP() const
{
	return (MoveForwardValueCPP > 0.0 && IsRunPressedCPP);
}

void AShooterCharacter::OnTriggerRun(const FInputActionValue& Value)
{
	IsRunPressedCPP = Value.Get<bool>();
}
void AShooterCharacter::TickRunCPP()
{
	if (CanRunCPP())
	{
		switch (StateCPP)
		{
		case StateOfCharacterCPP::Idle:
			StateCPP = StateOfCharacterCPP::Running;
			GetCharacterMovement()->MaxWalkSpeed *= 2.0f;
		}
	}
	else 
	{
		switch (StateCPP)
		{
		case StateOfCharacterCPP::Running:
			StateCPP = StateOfCharacterCPP::Idle;
			GetCharacterMovement()->MaxWalkSpeed *= 0.5f;
		}
	}
}

```

*步伐声音播放*

```c++
void AShooterCharacter::OnFootstepLeftCPP()
{
	if (GetSpeedCPP() > 0)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FootstepLeftSoundCPP, LowerBodyCPP->GetSocketLocation("foot_l"));
	}
}

void AShooterCharacter::OnFootstepRightCPP()
{
	if (GetSpeedCPP() > 0)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FootstepRightSoundCPP, LowerBodyCPP->GetSocketLocation("foot_r"));
	}
}
```

瞄准：角色如果在开火和不支持瞄准的枪，是无法瞄准的。





## 角色动画

在构造函数中初始化角色组件`ShadowBodyCPP`（角色的影子）、`LowerBodyCPP`（角色下半身）、`FirstPersonCPP`（角色）、`WeaponInHandCPP`（手部武器）、`WeaponInBackCPP`（背部武器）和 `CameraCPP`（相机组件）

在BeginPlay中通过隐藏角色的头部FirstPersonCPP和角色的上半身LowerBodyCPP来实现来防止角色的腿部随着视角而抬起，并且关闭他俩的阴影，只会出现武器的阴影。ShadowBody开启Hidden in Game选项，使游戏时不出现阴影体的骨骼和开启HiddenShadow。

然后制作瞄准偏移动画，避免阴影手和枪不是跟随摆动。



使用Limb_IK让手绑定武器添加两个 IK目标，IKRig 绑定左右手位置，IKAlpha 输入权重值。

每一把枪械的动画状态机有3个状态，瞄准、开火、跑姿势。

使用`Layered Blend Per Bone`来播放上半身的开火 / 瞄准和下半身的奔跑动 / 走路画

在PlayerCharacter的BeginPlay绑定动画通知：

- 通知装弹动画完成
- 通知弹夹分离或装载动作完成
- 处理武器的拿取与放回
- 走路时的左右脚触发声音

动画通知是UE用于在动画播放过程中触发自定义事件的机制，在动画序列中添加动画通知，AddNotify。

然后在角色动画蓝图中需要捕捉通知。使用事件分发器把动画蓝图里收到的通知转发给角色蓝图。在角色蓝图中绑定动画通知事件。然后播放蒙太奇动画。



## 解决问题

在制作PSBullet子弹时，cpp中声明静态网格体发现Details面板是空白的。

一开始以为是代码错误之类的问题，结果是UE的一个Bug。

解决方法是：将父类更改为Actor然后重新更换父类，发现Details面板恢复了。