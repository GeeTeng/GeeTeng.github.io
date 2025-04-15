---
title: "FPS游戏项目制作笔记"
date: 2025-04-15
tags: [UE]
description: "记录游戏中的核心代码和解决过的问题"
showDate: true
math: true
---





---



## 枪械后坐力

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



## 解决问题

在制作PSBullet子弹时，cpp中声明静态网格体发现Details面板是空白的。

一开始以为是代码错误之类的问题，结果是UE的一个Bug。

解决方法是：将父类更改为Actor然后重新更换父类，发现Details面板恢复了。