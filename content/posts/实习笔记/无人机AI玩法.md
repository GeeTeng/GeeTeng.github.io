---
title: "无人机AI玩法制作"
date: 2025-07-11
tags: [实习笔记]
description: "完整的任务支线制作流程"
showDate: true
math: true
chordsheet: true
---



## 无人机巡逻追踪玩家第一版：

目标：场景中有无人机巡逻飞行，无人机下挂着探照灯，巡逻时是白色灯光沿着样条线飞行；当遇到玩家后灯光变黄色并且追踪玩家，玩家在灯光区域内待到一定时间就会触发警报。

方法：

球形碰撞体为根组件位置在无人机的下方用于检测玩家是否进入这个区域（overlap）

骨骼网格体是无人机的骨骼 不做任何事

SpotLight灯光 更换灯光颜色

静态网格体是圆锥体形状的灯柱 更换材质上的颜色

SplineComponent 样条线组件 设置为绝对世界坐标

FloatingPawn移动组件用于飞行移动 更改速度

![04](/images/实习笔记/04.png)



样条线组件有自己的方法可以得到当前点的位置，当移动到起始位置和终点位置时切换方向，假如有3个点，呢就是0 1 2 1 0这样遍历每个点的位置，无人机AIController直接MoveToLocation到这个点。

```c++
void APMS_PatrolDrone::MoveAlongPatrol()
{
	if(PatrolRouteComponent)
	{
		FVector TargetPosition = GetSplinePointWorldPosition();
		if(AIController)
		{
			AIController->MoveToLocation(TargetPosition, 5);
			IncrementPatrolRoute();
		}
	}
}
void APMS_PatrolDrone::IncrementPatrolRoute()
{
	PatrolIndex += Direction;
	if(PatrolRouteComponent->GetNumberOfSplinePoints() - 1 == PatrolIndex)
	{
		Direction = -1;
	}
	else if(PatrolIndex == 0) {
		Direction = 1;
	}
}
FVector APMS_PatrolDrone::GetSplinePointWorldPosition()
{
	FVector Position = PatrolRouteComponent->GetLocationAtSplinePoint(PatrolIndex, ESplineCoordinateSpace::World);
	return Position;
}
```



在服务器端BeginPlay执行巡逻并且绑定球形碰撞体的Overlap委托。服务器用于改变速度，客户端更改灯光灯柱颜色。枚举变量FlightState有巡逻、追踪、警告状态，同步状态变量，在更改时调用OnSetState，

```c++
void APMS_PatrolDrone::BeginPlay()
{
	Super::BeginPlay();
	if (GetNetMode() != NM_Client)
	{
		if(SphereComponent)
		{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APMS_PatrolDrone::OnOverlapBegin);
			SphereComponent->OnComponentEndOverlap.AddDynamic(this, &APMS_PatrolDrone::OnOverlapEnd);
		}
		StartPatrol();
	}
}
void APMS_Drone::SetFlightState()
{
	if(GetNetMode() == NM_DedicatedServer)
	{
		if (FlightState == EFlightState::Patrol)
		{
			GetCharacterMovement()->MaxFlySpeed = PatrolSpeed;
		}
		else if (FlightState == EFlightState::Follow)
		{
			GetCharacterMovement()->MaxFlySpeed = FollowSpeed;
		}
		else if (FlightState == EFlightState::Alert)
		{
		
		}
	}
	else
	{
		if (FlightState == EFlightState::Patrol)
		{
			ChangeMaterialColor(PatrolColor);
			GetCharacterMovement()->MaxFlySpeed = PatrolSpeed;
		}
		else if (FlightState == EFlightState::Follow)
		{
			ChangeMaterialColor(FollowColor);
			GetCharacterMovement()->MaxFlySpeed = FollowSpeed;
		}
		else if (FlightState == EFlightState::Alert)
		{
		
		}
	}
}
void APMS_Drone::OnRep_FlightState()
{
	SetFlightState();
}
```

 

Overlap两个事件：

OnOverlapBegin玩家进入区域时，记录进入时间，设置布尔变量已经进入区域（用于），无人机停止巡逻并开始追踪玩家。

OnOverlapEnd玩家离开区域时，计算时间

```c++
void APMS_PatrolDrone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(GetNetMode() != NM_Client)
	{
		if(OtherActor && OtherActor != this)
		{
			APMS_HeroCharacter* PlayerCharacter = Cast<APMS_HeroCharacter>(OtherActor);
			if(!PlayerCharacter) return;
			bIsOverlap = true;
			OverlapStartTime = FPlatformTime::Seconds(); ;
			StopPatrol();
			StartFollowingPlayer(PlayerCharacter);
		}
	}
}
void APMS_PatrolDrone::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(GetNetMode() != NM_Client)
	{
		if(OtherActor && OtherActor != this)
		{
			APMS_HeroCharacter* PlayerCharacter = Cast<APMS_HeroCharacter>(OtherActor);
			if(!PlayerCharacter) return;
			bIsOverlap = false;
		}
	}
}
```

StartFollowingPlayer启用定时器调用UpdateFollowPosition更新玩家坐标

UpdateFollowPosition中累计跟踪时间，超出时间没捕捉到玩家就返回巡逻状态；并且记录在区域内的时间（如果不在区域内bIsOverlap会变成false就不会计算OverlapDuration了），超出时间就会触发警报。

StartPatrol也是设置定时器，StopPatrol和StopFollowing都是取消定时器。

```c++
void APMS_PatrolDrone::StartFollowingPlayer(APMS_HeroCharacter* PlayerCharacter)
{
	if(AIController && !bIsFollowTimerActive)
	{
		bIsFollowing = true;
		CurrentAimPlayer = PlayerCharacter;
		GetWorld()->GetTimerManager().SetTimer(FollowTimerHandle, this, &APMS_PatrolDrone::UpdateFollowPosition, FollowInterval, true);
		FlightState = EFlightState::Warning;
		SetFlightState();
		bIsFollowTimerActive = true;
		FollowTimeDuration = 0.f;
	}
}
void APMS_PatrolDrone::UpdateFollowPosition()
{
	if(bIsFollowing && CurrentAimPlayer)
	{
		FVector Location = CurrentAimPlayer->GetActorLocation();
		AIController->MoveToLocation(Location);
		FollowTimeDuration += FollowInterval;
		if(bIsOverlap)
		{
			OverlapDuration = FPlatformTime::Seconds() - OverlapStartTime;
			if(OverlapDuration > MaxOverlapTime)
			{
				OverlapDuration = 0.f;
				OverlapStartTime = 0.f;
				StopFollowingPlayer();
				// 警报逻辑
			}
		}
		if(FollowTimeDuration >= MaxFollowTime)
		{
			StopFollowingPlayer();
			StartPatrol();
		}
	}
}
void APMS_PatrolDrone::StopFollowingPlayer()
{
	if(AIController && bIsFollowTimerActive)
	{
		bIsFollowing = false;
		GetWorld()->GetTimerManager().ClearTimer(FollowTimerHandle);
		bIsFollowTimerActive = false;
		CurrentAimPlayer = nullptr;
		AIController->StopMovement();
	}
}
void APMS_PatrolDrone::StartPatrol()
{
	if(!bIsPatrolTimerActive)
	{
		FlightState = EFlightState::Patrol;
		SetFlightState();
		GetWorld()->GetTimerManager().SetTimer(PatrolTimerHandle, this, &APMS_PatrolDrone::MoveAlongPatrol, PatrolInterval, true);
		bIsPatrolTimerActive = true;
	}
}

void APMS_PatrolDrone::StopPatrol()
{
	if(bIsPatrolTimerActive)
	{
		GetWorld()->GetTimerManager().ClearTimer(PatrolTimerHandle);
		bIsPatrolTimerActive = false;
	}
}
```



## 无人机巡逻追踪玩家第二版:

由于第一版在巡逻时行走不够平滑，并且追踪玩家会有导航不到的情况（因为样条线不贴地 MoveTo是基于导航网格实现），而且FloatingPawnMovement不会处理无人机转向问题，等等不好的效果...

所以改进，继承Character，移动组件使用CharacterMovement，自动处理好了转向。

将沿着样条线移动更换成在Tick中不断设置无人机的位置，然后自定义一个任务，用于控制AI移动到指定位置一次性的或者不停跟随指定玩家。

**TickTask**：这是每一帧都调用的方法，用于控制角色的移动。

计算当前角色位置到目标位置的距离，如果距离小于容忍半径，则停止移动，并根据 bEndOnTargetReached 参数判断是否结束任务。

如果目标没有到达，计算角色的移动方向，并更新角色的位置和朝向。

```c++
UGameplayTask_FlyMoveTo::UGameplayTask_FlyMoveTo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
}
UGameplayTask_FlyMoveTo* UGameplayTask_FlyMoveTo::FlyMoveTo(
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner,
	FVector TargetLocation,
	float InAcceptableRadius,
	float InMoveSpeed,
	float InTurnRate)
{
	if (TaskOwner.GetInterface() == nullptr)
	{
		return nullptr;
	}

	AAIController* TaskOwnerActor = Cast<AAIController>(TaskOwner->GetGameplayTaskOwner(nullptr));
	if (!IsValid(TaskOwnerActor))
	{
		return nullptr;
	}

	auto ControlledPawn = TaskOwnerActor->GetPawn();
	if (!IsValid(ControlledPawn))
	{
		return nullptr;
	}
	
	UGameplayTask_FlyMoveTo* Task = NewTask<UGameplayTask_FlyMoveTo>(TaskOwner);
	Task->InitTask(*TaskOwner, 0);
	Task->GoalLocation       = TargetLocation;
	Task->AcceptableRadius   = FMath::Max(1.f, InAcceptableRadius);
	Task->AcceptableRadiusSq = FMath::Square(Task->AcceptableRadius);
	Task->MoveSpeed          = InMoveSpeed;
	Task->TurnRate           = InTurnRate;
	Task->ControlledPawn     = ControlledPawn;
	Task->ReadyForActivation();
	return Task;
}
UGameplayTask_FlyMoveTo* UGameplayTask_FlyMoveTo::FlyMoveToActor(
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner, AActor* TargetActor, float AcceptableRadius,
	float MoveSpeed, float TurnRate, float OffsetZ, bool bEndOnTargetReached)
{
	if (TaskOwner.GetInterface() == nullptr)
	{
		return nullptr;
	}

	AAIController* TaskOwnerActor = Cast<AAIController>(TaskOwner->GetGameplayTaskOwner(nullptr));
	if (!IsValid(TaskOwnerActor))
	{
		return nullptr;
	}

	auto ControlledPawn = TaskOwnerActor->GetPawn();
	if (!IsValid(ControlledPawn))
	{
		return nullptr;
	}
	
	UGameplayTask_FlyMoveTo* Task = NewTask<UGameplayTask_FlyMoveTo>(TaskOwner);
	Task->InitTask(*TaskOwner, 0);
	Task->TargetActor       = TargetActor;
	Task->AcceptableRadius   = FMath::Max(1.f, AcceptableRadius);
	Task->AcceptableRadiusSq = FMath::Square(Task->AcceptableRadius);
	Task->MoveSpeed          = MoveSpeed;
	Task->TurnRate           = TurnRate;
	Task->OffsetZ            = OffsetZ;
	Task->ControlledPawn     = ControlledPawn;
	Task->bEndOnTargetReached = bEndOnTargetReached;
	Task->ReadyForActivation();
	return Task;
}
void UGameplayTask_FlyMoveTo::Activate()
{
	Super::Activate();

	if (!ControlledPawn.IsValid())
	{
		FinishTask(false);
		return;
	}
	
	if (ACharacter* TempCharacter = Cast<ACharacter>(ControlledPawn.Get()))
	{
		UCharacterMovementComponent* MoveComp = TempCharacter->GetCharacterMovement();
		if (MoveComp && MoveComp->MovementMode != MOVE_Flying)
		{
			MoveComp->SetMovementMode(MOVE_Flying);
		}
	}
}
void UGameplayTask_FlyMoveTo::SetTargetLocation(const FVector& NewLocation)
{
	GoalLocation = NewLocation;
}
void UGameplayTask_FlyMoveTo::SetTargetActor(AActor* NewTarget)
{
	TargetActor = NewTarget;
}
void UGameplayTask_FlyMoveTo::TickTask(float DeltaTime)
{
	if (!ControlledPawn.IsValid())
	{
		FinishTask(false);
		return;
	}

	APawn* Pawn = ControlledPawn.Get();
	auto TempCharacter = Cast<ACharacter>(Pawn);
	FVector TargetLoc = TargetActor.IsValid() ? TargetActor->GetActorLocation() + FVector(0.f, 0.f, OffsetZ) : GoalLocation;
	const FVector CurrLoc = Pawn->GetActorLocation();
	const float DistSq = FVector::DistSquared(CurrLoc, TargetLoc);
	if (DistSq <= AcceptableRadiusSq)
	{
		if (IsValid(TempCharacter))
		{
			TempCharacter->GetCharacterMovement()->StopMovementImmediately();
		}
		if (bEndOnTargetReached)
		{
			FinishTask(true);
		}
		return;
	}
	const FVector Dir = (TargetLoc - CurrLoc).GetSafeNormal();
	if (IsValid(TempCharacter))
	{
		UCharacterMovementComponent* CMC = TempCharacter->GetCharacterMovement();
		if (CMC)
		{
			CMC->Velocity = Dir * MoveSpeed;
		}
		const FRotator TargetRot = Dir.Rotation();
		const FRotator NewRot    = FMath::RInterpConstantTo(TempCharacter->GetActorRotation(), TargetRot, DeltaTime, TurnRate);
		TempCharacter->SetActorRotation(NewRot);
	}
	else
	{
		Pawn->AddActorWorldOffset(Dir * MoveSpeed * DeltaTime, true);
	}
}
void UGameplayTask_FlyMoveTo::FinishTask(bool bSuccess)
{
	if (bSuccess)
	{
		OnSucceeded.Broadcast();
	}
	else
	{
		OnFailed.Broadcast();
	}

	EndTask();
}
void UGameplayTask_FlyMoveTo::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}
```

 

无人机在追踪玩家之后，得移动到原来的巡逻离开的当前位置。所以需要使用FlyMoveTo任务，通过广播的OnSucceed来判断是否到达地点。而FlyMoveToActor是指定玩家后，任务中自动Tick。只需要判断是否到达最大跟随时间，结束任务就行。

无人机在触发警报后会消失，同时触发警告逻辑。

```c++
APMS_Drone::APMS_Drone()
{
	PrimaryActorTick.bCanEverTick = true;
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLightComponent"));
	SpotLightComponent->SetupAttachment(GetMesh());
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SpotLightComponent);
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("PatrolRouteComponent"));
	SplineComponent->SetupAttachment(RootComponent);
}
void APMS_Drone::BeginPlay()
{
	Super::BeginPlay();
	if (GetNetMode() != NM_Client)
	{
		if(SphereComponent)
		{
			SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APMS_Drone::OnOverlapBegin);
			SphereComponent->OnComponentEndOverlap.AddDynamic(this, &APMS_Drone::OnOverlapEnd);
		}
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APMS_SaveArea::StaticClass(), SaveAreas);
		for(AActor* Actor : SaveAreas)
		{
			APMS_SaveArea* SaveArea = Cast<APMS_SaveArea>(Actor);
			SaveArea->OnPlayerEnterSafeArea.AddDynamic(this, &APMS_Drone::OnPlayerEnterSafeArea);
		}
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		FlightState = EFlightState::Idle; // 暂时设置为Idle 因为之后会有进入区域触发无人机巡逻
		OnSetFlightState();
	}
}
void APMS_Drone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(HasAuthority())
	{
		if(FlightState == EFlightState::Patrol)
		{
			StartPatrol(DeltaTime);
		}
		else if(FlightState == EFlightState::Follow)
		{
			if(!bisFollow && !bisAlert)
			{
				StartFollow();
			}
			FollowDuration += DeltaTime;
			// 超出最大跟随时间并且没有触发警报 则返回巡逻
			if(FollowDuration >= MaxFollowTime && !bisAlert)
			{
				FollowReturnPatrol();
			}
			// 超出最大停留时间 则触发警报
			if(OverlapStartTime)
			{
				OverlapDuration = GetWorld()->GetTimeSeconds() - OverlapStartTime;
				if(OverlapDuration >= MaxOverlapTime)
				{
					bisAlert = true;
					StartAlert();
				}
			}
		}
	}
}

void APMS_Drone::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AIController = Cast<AAIController>(GetController());
}

void APMS_Drone::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APMS_Drone, FlightState);
}

void APMS_Drone::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(GetNetMode() != NM_Client)
	{
		if(OtherActor && OtherActor != this)
		{
			APMS_HeroCharacter* PlayerCharacter = Cast<APMS_HeroCharacter>(OtherActor);
			if(!PlayerCharacter) return;
			TargetActor = PlayerCharacter;
			FlightState = EFlightState::Follow;
			OnSetFlightState();
			OverlapStartTime = GetWorld()->GetTimeSeconds();
		}
	}
}

void APMS_Drone::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if(GetNetMode() != NM_Client)
	{
		if(OtherActor && OtherActor != this)
		{
			APMS_HeroCharacter* PlayerCharacter = Cast<APMS_HeroCharacter>(OtherActor);
			if(!PlayerCharacter) return;
			OverlapStartTime = 0.f;
			OverlapDuration = 0.f;
		}
	}
}

void APMS_Drone::StartPatrol(float DeltaTime)
{
	if (SplineComponent)
	{
		DistanceAlongSpline += Direction * PatrolSpeed * DeltaTime;
		if (DistanceAlongSpline >= SplineComponent->GetSplineLength())
		{
			DistanceAlongSpline = SplineComponent->GetSplineLength();
			Direction = -1;
		}
		else if(DistanceAlongSpline <= 0)
		{
			DistanceAlongSpline = 0;
			Direction = 1;
		}
		CurrentPosition = SplineComponent->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
		SetActorLocation(CurrentPosition);
	}
}

void APMS_Drone::StartFollow()
{
	bisFollow = true;
	if(TargetActor)
	{
		if (IsValid(FlyMoveToActorTask))
		{
			FlyMoveToActorTask->EndTask();
			FlyMoveToActorTask = nullptr;
		}
		FlyMoveToActorTask = UGameplayTask_FlyMoveTo::FlyMoveToActor(AIController, TargetActor, AcceptableRadius, FollowSpeed, TurnRate, OffsetZ, false);
		FollowDuration = 0.f;
	}
}

void APMS_Drone::StartAlert()
{
	FlightState = EFlightState::Alert;
	OnSetFlightState();
    // ...警告逻辑
}

void APMS_Drone::FollowReturnPatrol()
{
	if (IsValid(FlyMoveToActorTask))
	{
		FlyMoveToActorTask->EndTask();
		FlyMoveToActorTask = nullptr;
	}
	bisFollow = false;
	if (IsValid(FlyMoveToTask))
	{
		FlyMoveToTask->EndTask();
		FlyMoveToTask = nullptr;
	}
	FlightState = EFlightState::Idle; // 状态过渡
	OnSetFlightState();
	FlyMoveToTask = UGameplayTask_FlyMoveTo::FlyMoveTo(AIController, CurrentPosition, 0.f, PatrolSpeed, TurnRate);
	if (IsValid(FlyMoveToTask))
	{
		FlyMoveToTask->OnSucceeded.AddDynamic(this, &APMS_Drone::OnFlyMoveToSuccess);
	}
}

void APMS_Drone::OnPlayerEnterSafeArea(APMS_HeroCharacter* EntryCharacter)
{
	if(EntryCharacter)
	{
		if(TargetActor && EntryCharacter == TargetActor)
		{
			FollowReturnPatrol();
		}
	}
}

void APMS_Drone::ChangeMaterialColor(FLinearColor FresnelColor)
{
	if(StaticMeshComponent && StaticMeshComponent->GetMaterial(0))
	{
		StaticMeshComponent->SetCustomPrimitiveDataVector4(0,FresnelColor);
	}
	if(SpotLightComponent)
	{
		SpotLightComponent->SetLightColor(FresnelColor);
	}
}

void APMS_Drone::OnSetFlightState()
{
	if(GetNetMode() == NM_DedicatedServer)
	{
		if(FlightState == EFlightState::Idle)
		{
			GetCharacterMovement()->MaxFlySpeed = 0.f;
		}
		else if (FlightState == EFlightState::Patrol)
		{
			GetCharacterMovement()->MaxFlySpeed = PatrolSpeed;
		}
		else if (FlightState == EFlightState::Follow)
		{
			GetCharacterMovement()->MaxFlySpeed = FollowSpeed;
		}
	}
	else
	{
		if(FlightState == EFlightState::Idle)
		{
			GetCharacterMovement()->MaxFlySpeed = 0.f;
			ChangeMaterialColor(PatrolColor);
		}
		else if (FlightState == EFlightState::Patrol)
		{
			ChangeMaterialColor(PatrolColor);
			GetCharacterMovement()->MaxFlySpeed = PatrolSpeed;
		}
		else if (FlightState == EFlightState::Follow)
		{
			ChangeMaterialColor(FollowColor);
			GetCharacterMovement()->MaxFlySpeed = FollowSpeed;
		}
		else if (FlightState == EFlightState::Alert)
		{
			SetActorHiddenInGame(true);
		}
	}
}
void APMS_Drone::OnRep_FlightState()
{
	OnSetFlightState();
}
void APMS_Drone::OnFlyMoveToSuccess()
{
	FlightState = EFlightState::Patrol;
	OnSetFlightState();
}
```

### 探照灯玩法之怪物攻击

当无人机发出警报后，无人机消失，怪物会增加仇恨值目标是距离它最近的玩家。

如果同时有4个玩家，玩家们聚集在一个地方，当无人机触发警报之后，会引发四周的怪物分别朝向距离最近的玩家攻击。

增加丧尸仇恨值方法，遍历世界中的怪物AIController，通过AIController获取Pawn，怪物的Location保存下来。

遍历GameState中的PlayState数组（一共四个玩家），如果玩家存活就计算怪物与玩家的最短距离，将怪物的目标设置为当前距离最短的玩家，再通过调用AddHatredActorWithSightCheck来增加丧失的仇恨值。

```c++
void APMS_Drone::AddHatredToClosestActor()
{
	if(APMS_ZombieGameState* GameState = GetWorld()->GetGameState<APMS_ZombieGameState>())
	{
		for(TActorIterator<APMS_ZombieMonsterAIController> It(GetWorld()); It; ++ It)
		{
			float MinDistance = FLT_MAX;
			APawn* Pawn = It->GetPawn();
			APMS_ZombiePlayerCharacter* TargetCharacter = nullptr;
			if(IsValid(Pawn))
			{
				APMS_ZombieAIMonsterCharacter* MonsterCharacter = Cast<APMS_ZombieAIMonsterCharacter>(Pawn);
				FVector MonsterLocation = MonsterCharacter->GetActorLocation();
				for(auto Element : GameState->PlayerArray)
				{
					APMS_ZombiePlayerState* PMS_PlayerState = Cast<APMS_ZombiePlayerState>(Element);
					APMS_ZombiePlayerCharacter* PlayerCharacter = PMS_PlayerState->GetPawn<APMS_ZombiePlayerCharacter>();
					if(IsValid(PMS_PlayerState) && IsValid(PlayerCharacter) && !PMS_PlayerState->IsDead())
					{
						float Distance = FVector::Dist(MonsterLocation, PlayerCharacter->GetActorLocation());
						if (Distance < MinDistance)
						{
							MinDistance = Distance;
							TargetCharacter = PlayerCharacter;
						}
					}
				}
			}
			if (IsValid(TargetCharacter))
			{
				UPMS_GamePlayUtilsFunctionLibrary::AddHatredActorWithSightCheck(TargetCharacter, (*It));
			}
		}
	}
}
```



## 出现问题：

### 无法隐藏全部无人机

如果设置为发出警报就隐藏无人机，但是场景中多个无人机无法隐藏其他无人机。并且进入场景任务触发区域激活无人机巡逻行为以及离开任务区域会关闭玩法，所以要通过任务Tag来绑定委托 当无人机触发警报时要先隐藏无人机，并SetActorTickEnabled(false);

等到玩家离开无人机区域后，要销毁无人机。

```c++
	UFUNCTION()
	void OnTriggerActivateDrone(FGameplayTag Topic, UObject* Payload);

	UFUNCTION()
	void OnTriggerDestoryDrone(FGameplayTag Topic, UObject* Payload);

	UFUNCTION()
	void OnTriggerHiddenDrone(FGameplayTag Topic, UObject* Payload);

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ToolTip = "激活无人机的tag"))
	FGameplayTag ActivateTriggerTag;
	
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ToolTip = "警报后无人机销毁的Tag"))
	FGameplayTag HideTriggerTag;

	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ToolTip = "离开区域无人机销毁的Tag"))

	FGameplayTag LeaveTriggerTag;
	FDispatchitoDynamicDelegate OnDroneActivatedDelegate;
	FDispatchitoEventBindingHandle DroneActivatedHandle;

	FDispatchitoDynamicDelegate OnHideDroneDelegate;
	FDispatchitoEventBindingHandle HideDroneHandle;

	FDispatchitoDynamicDelegate OnLeaveAreaDelegate;
	FDispatchitoEventBindingHandle LeaveAreaHandle;
```



无人机Tick中新增警告状态判断，用于判断警告后过了多少秒会消除警告UI和声音（TS中写的逻辑）。并且在OnSetFlightState中的客户端增加一个警告状态用于广播开始警报的事件。

```c++
		// Tick
        else if(FlightState == EFlightState::Alert)
		{
			AlertDuration = GetWorld()->GetTimeSeconds() - AlertStartTime;
			if(AlertDuration >= MaxAlertTime)
			{
				MulticastRemoveAlert();
				SetActorTickEnabled(false);
			}
		}		
      // OnSetFlightState
        else if (FlightState == EFlightState::Alert)
		{
			UPMS_GameDelegate_WorldSubsystem::BroadcastEvent(GetWorld(), &UPMS_GameDelegate_WorldSubsystem::GetOnStartAlert);
		}

```



BeginPlay中绑定Tag的触发委托

```c++
void APMS_Drone::BeginPlay()
{
	Super::BeginPlay();
	if (GetNetMode() != NM_Client)
	{
		if (ActivateTriggerTag.IsValid())
		{
			OnDroneActivatedDelegate.BindDynamic(this, &APMS_Drone::OnTriggerActivateDrone);
			DroneActivatedHandle = UDispatchitoBlueprintLibrary::SubscribeToTopic(this, ActivateTriggerTag, OnDroneActivatedDelegate);
		}
		if(HideTriggerTag.IsValid())
		{
			OnHideDroneDelegate.BindDynamic(this, &APMS_Drone::OnTriggerHiddenDrone);
			HideDroneHandle = UDispatchitoBlueprintLibrary::SubscribeToTopic(this, HideTriggerTag, OnHideDroneDelegate);
		}
		if(LeaveTriggerTag.IsValid())
		{
			OnLeaveAreaDelegate.BindDynamic(this, &APMS_Drone::OnTriggerDestoryDrone);
			LeaveAreaHandle = UDispatchitoBlueprintLibrary::SubscribeToTopic(this, LeaveTriggerTag, OnLeaveAreaDelegate);
		}
	}
}
void APMS_Drone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if(DroneActivatedHandle.IsValid())
	{
		UDispatchitoBlueprintLibrary::Unsubscribe(this, DroneActivatedHandle);
	}
	if(HideDroneHandle.IsValid())
	{
		UDispatchitoBlueprintLibrary::Unsubscribe(this, HideDroneHandle);
	}
	if(LeaveAreaHandle.IsValid())
	{
		UDispatchitoBlueprintLibrary::Unsubscribe(this, LeaveAreaHandle);
	}
}
void APMS_Drone::OnTriggerActivateDrone(FGameplayTag Topic, UObject* Payload)
{
	FlightState = EFlightState::Patrol;
	OnSetFlightState();
}

void APMS_Drone::OnTriggerHiddenDrone(FGameplayTag Topic, UObject* Payload)
{
	SetActorHiddenInGame(true);
}

void APMS_Drone::MulticastRemoveAlert_Implementation()
{
	UPMS_GameDelegate_WorldSubsystem::BroadcastEvent(GetWorld(), &UPMS_GameDelegate_WorldSubsystem::GetOnRemoveAlert);
}

void APMS_Drone::OnTriggerDestoryDrone(FGameplayTag Topic, UObject* Payload)
{
	Destroy();
	FlyMoveToActorTask->EndTask();
}
```



### 网络同步不生效

因为h2d场景的网络同步压力太大了 所以unreliable函数没办法执行了

```c++
UFUNCTION(NetMulticast, Reliable)
void MulticastRemoveAlert();
```

### 垃圾回收导致崩溃

对象成员 'FlyMoveToActorTask'可能会在垃圾收集期间被销毁，导致指针过时。所以需要更改成如下这样，确保垃圾回收时不会回收这些。

UPROPERTY避免垃圾回收了这些任务，TWeakObjectPtr是弱指针引用，为了在对象销毁后，指针会自动失效，避免了悬空指针

```c++
	TWeakObjectPtr<AAIController> AIController;
	TWeakObjectPtr<APMS_HeroCharacter> TargetActor = nullptr;
	UPROPERTY()
	TObjectPtr<UGameplayTask_FlyMoveTo> FlyMoveToActorTask;
	UPROPERTY()
	TObjectPtr<UGameplayTask_FlyMoveTo> FlyMoveToTask;
```

