---
title: "多人FPS游戏制作笔记"
date: 2025-05-23
tags: [实习笔记]
description: "多人FPS游戏初步搭建、网络同步、武器组件化、复活"
showDate: true
math: true
chordsheet: true
---

 

角色添加了四个骨骼，分别是第一人称视角的：LegMesh自己看自己的腿，FirstPersonMesh自己看自己的手，和其他玩家视角的全身模型：ThirdpersonMesh，以及ShadowMesh影子网格体

![img](/images/实习笔记/02.png)

动画完成了人物的走跑（八方向混合空间）、跳（起跳—在空中—落地）、射击部分。

## 射击实现

```c++
protected:
	void SetupInputComponent() override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputMappingContext> DefaultIMC;
 
void ADemoPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
 
	if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		FModifyContextOptions Options;
		Options.bForceImmediately = true;
		Subsystem->ClearAllMappings();
		if(DefaultIMC)
		{
			Subsystem->AddMappingContext(DefaultIMC, 0, Options);
		}
	}
}
```

在SetupPlayerInputComponent中调用自定义输入组件绑定输入。

```c++
void ADemoPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UDemoPlayerInputComponent* DemoInput = CastChecked<UDemoPlayerInputComponent>(PlayerInputComponent);
	if (DemoInput)
	{
		DemoInput->SetupInputBindings();
	}
}
void UDemoPlayerInputComponent::SetupInputBindings()
{
	if(IA_DemoFire)
	{
		BindAction(IA_DemoFire, ETriggerEvent::Started, this, &UDemoPlayerInputComponent::StartFire);
		BindAction(IA_DemoFire, ETriggerEvent::Completed, this, &UDemoPlayerInputComponent::EndFire);
	}
}
```

自定义输入组件中的开火函数用来调用ADemoPlayerCharacter的开火函数

```c++
void UDemoPlayerInputComponent::StartFire(const FInputActionValue& InputActionValue)
{
	ADemoPlayerCharacter* CharacterOwner = GetOwner<ADemoPlayerCharacter>();
	if(IsValid(CharacterOwner))
	{		CharacterOwner->OnStartFire();
	}
}
 
void UDemoPlayerInputComponent::EndFire(const FInputActionValue& InputActionValue)
{
	ADemoPlayerCharacter* CharacterOwner = GetOwner<ADemoPlayerCharacter>();
	if(IsValid(CharacterOwner))
	{
		CharacterOwner->OnEndFire();
	}
}
```

角色类中的开火逻辑 定时器开火不断调用武器类中的开火

```c++
void ADemoPlayerCharacter::OnStartFire()
{
	if(CurrentWeapon)
	{
		GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ADemoPlayerCharacter::FireWeapon, FireRate, true);
		bIsFire = true;
	}
}
 
void ADemoPlayerCharacter::OnEndFire()
{
	if(CurrentWeapon)
	{
		GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
		bIsFire = false;
	}
}
 
void ADemoPlayerCharacter::FireWeapon()
{
	if(CurrentWeapon)
	{
		CurrentWeapon->Fire();
	}
}
```

武器类的开火逻辑

其中AmmoCount是子弹数量设定为30， CanFire逻辑还没写，之后在里面判定枪械状态，比如说换弹时不能开枪。或者更复杂一些，比如被技能控制不能开火我认为都可以写在CanFire里。

使用自定义武器射线碰撞，因为默认的ECC_Visibility会将摄像机视野中的物体都检测，会命中很多没必要检测的对象，自定义通道提升效率，减少无意义的检测计算。

在项目设置中的Collision中添加新的射线通道，block（阻挡，射线击中就停止）和ignore（忽略，比如忽略队友）和overlap（重叠，触发器盒子，范围检测）

这些逻辑包括TakeDamege都在服务器上判断

```c++
void ADemoWeapon::Fire()
{
	if(CanFire() && AmmoCount > 0)
	{
		CurrentState = EWeaponState::WS_Firing;
		AmmoCount --;
		UE_LOG(LogTemp, Warning, TEXT("Firing Weapon, AmmoCount:%d"),AmmoCount);
		FireRaycast();
	}
	// TODO:恢复到Idle状态
}
void ADemoWeapon::FireRaycast()
{
	ADemoPlayerCharacter* PlayerCharacter = Cast<ADemoPlayerCharacter>(GetOwner());
	if(!PlayerCharacter) return;
	FVector Start = PlayerCharacter->FirstPersonCamera->GetComponentLocation();
	FVector ForwardVector = PlayerCharacter->FirstPersonCamera->GetForwardVector();
	FVector End = (ForwardVector * 1000.f) + Start;
 
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	// 改成自定义武器射线碰撞
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, (ECollisionChannel)TR_WeaponTrace, CollisionParams);
	if (bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if(HitActor)
		{
			ADemoPlayerCharacter* HitCharacter = Cast<ADemoPlayerCharacter>(HitActor);
			if(HitCharacter)
			{
				HitCharacter->TakeDamage(Damage);
			}
		}
	}
}
```



## 委托_生命值和子弹数量更新

*委托*：是一种**观察者模式**，也被称为代理，主要用于监听事件或变量的变化。监听者将需要响应的函数绑定到委托对象上，使得委托在触发时调用所绑定的函数。

在UE中，按照委托函数个数分为单播、多播，按照是否可暴露给蓝图分为静态和动态。所以一共**四种类型**。

射击子弹数量同步到UI上使用动态多播委托

动态多播委托在执行时需要实时在类中按照给定的函数名字查找对应的函数，因此执行速度慢，**维护了一个由动态单播委托组成的TArray数组，依托动态单播委托实现。**

只有动态多播可以被蓝图绑定，需要加标记*BlueprintAssignable*。

在武器类中声一个带有两个参数的动态多播委托，传递当前子弹数量和最大子弹数量。

FOnAmmoChanged是一个委托类型，可以将多个处理函数绑定到委托上。

```c++
/** 子弹变化 **/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, AmmoCount, int32, MaxAmmo);
UPROPERTY(BlueprintAssignable, Category = "Events")
FOnAmmoChanged OnAmmoChanged; // 创建委托实例
/** 生命值变化 **/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChanged, int32, Health);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged;
```

在武器类的射击函数中添加OnAmmoChanged.Broadcast(AmmoCount, MaxAmmo); // 用来广播委托

在角色类的TakeDamage受伤函数中添加OnHealthChanged.Broadcast(Health);

在玩家控制器类BeginPlay中绘制UserWidget。

```c++
void ADemoPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController())
	{
		if(GameplayUserWidgetClass)
		{
			GameplayUserWidget = CreateWidget<UDemoGameplayUserWidget>(this, GameplayUserWidgetClass);
			if(GameplayUserWidget)
			{
				GameplayUserWidget->AddToViewport();
			}
		}
	}
}
```

在角色控制器类中实现可以调用UserWidget类更新UI的函数，还有一进游戏的初始化UI函数

```c++
void ADemoPlayerController::OnAmmoChanged(int32 AmmoCount, int32 MaxAmmo)
{
	if(GameplayUserWidget)
	{
		GameplayUserWidget->UpdateAmmoDisplay(AmmoCount, MaxAmmo);
	}
}
 
void ADemoPlayerController::OnHealthChanged(int32 Health)
{
	if (GameplayUserWidget)
	{
		GameplayUserWidget->UpdateHealthDisplay(Health);
	}
}
void ADemoPlayerController::InitGameplayUserWidget()
{
	ADemoPlayerCharacter* PlayerCharacter = Cast<ADemoPlayerCharacter>(GetPawn());
	GameplayUserWidget->UpdateAmmoDisplay(PlayerCharacter->CurrentWeapon->AmmoCount, PlayerCharacter->CurrentWeapon->MaxAmmo);
	GameplayUserWidget->UpdateHealthDisplay(PlayerCharacter->Health);
}
```

UserWidget中更新子弹数量函数

```c++
void UDemoGameplayUserWidget::UpdateAmmoDisplay(int32 AmmoCount, int32 MaxAmmo)
{
	if(TB_AmmoCount)
	{
		TB_AmmoCount->SetText(FText::AsNumber(AmmoCount));
	}
	if(TB_MaxAmmo)
	{
		TB_MaxAmmo->SetText(FText::AsNumber(MaxAmmo));
	}
}
```

由于把绑定委托写在角色类BeginPlay中会导致时序问题，所以重写PawnClientRestart函数，确保获得到了控制器之后再进行绑定。

ChangeAmmo和ChangeHealth是绑定到委托上的两个函数。

```c++
void ADemoPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	ADemoPlayerController* PC = Cast<ADemoPlayerController>(GetController());
	if(PC)
	{
		PC->InitInputSystem();
		if(IsLocallyControlled())
		{
			PC->InitGameplayUserWidget(); // 调用角色控制器类中初始化UI
			OnHealthChanged.AddDynamic(this, &ADemoPlayerCharacter::ChangeHealth);
			CurrentWeapon->OnAmmoChanged.AddDynamic(this, &ADemoPlayerCharacter::ChangeAmmo);
		}
	}
}
void ADemoPlayerCharacter::ChangeHealth(int32 DelegateHealth)
{
	ADemoPlayerController* PC = Cast<ADemoPlayerController>(GetController());
	if(PC)
	{
		PC->OnHealthChanged(DelegateHealth);
	}
}
void ADemoPlayerCharacter::ChangeAmmo(int32 AmmoCount, int32 MaxAmmo)
{
	ADemoPlayerController* PC = Cast<ADemoPlayerController>(GetController());
	if(PC)
	{
		PC->OnAmmoChanged(AmmoCount, MaxAmmo);
	}
}
```

 

## 网络同步_多人游戏

### Server

生命值和子弹数量需要同步给服务器，因为客户端不能自己修改，需要服务器来操作。

以子弹数量为例：

```c++
UPROPERTY(ReplicatedUsing = On_RepAmmoCount, BlueprintReadWrite, Category = "Weapon")
	int32 AmmoCount = 25;
UFUNCTION()
void On_RepAmmoCount();
 
void ADemoWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
 
	DOREPLIFETIME(ADemoWeapon, AmmoCount);
}
void ADemoWeapon::On_RepAmmoCount()
{
	OnAmmoChanged.Broadcast(AmmoCount, MaxAmmo);
}
```

其次同步开火事件，让服务器处理开火逻辑。

```c++
// 开火定时器调用FireWeapon开火逻辑
void StartFire();
UFUNCTION(Server, Unreliable, WithValidation)
void C2SStartFire();
	
// 结束开火
void EndFire();
UFUNCTION(Server, Reliable, WithValidation)
void C2SEndFire();
 
void ADemoWeapon::StartFire()
{
	if(HasAuthority())
	{
		if(CanFire())
		{
			CurrentState = EWeaponState::WS_Firing;
			FireWeapon();
			GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ADemoWeapon::FireWeapon, FireRate, true);
		}
	}
	else
	{
		C2SStartFire();
	}
}
 
void ADemoWeapon::C2SStartFire_Implementation()
{
	StartFire();
}
 
bool ADemoWeapon::C2SStartFire_Validate()
{
	return CanFire();
}
void ADemoWeapon::EndFire()
{
	if(HasAuthority())
	{
		if(CanFire())
		{
			CurrentState = EWeaponState::WS_Idle;
			GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
		}
	}
	else
	{
		C2SEndFire();
	}
}
 
void ADemoWeapon::C2SEndFire_Implementation()
{
	EndFire();
}
```



### Muticast播放开火动画

将播放蒙太奇动画封装成一个函数，方便之后其他操作播放不同的动画。

每次射击时调用Multicast_PlayMontageAnimation，同时播放第一人称动画（仅主控端自己可见）和第三人称（其他人可见）。

```c++
UFUNCTION(NetMulticast, Unreliable)
void Multicast_PlayMontageAnimation(UAnimMontage* FPMontage, UAnimMontage* TPMontage);
void ADemoPlayerCharacter::Multicast_PlayMontageAnimation_Implementation(UAnimMontage* FPMontage, UAnimMontage* TpMontage)
{
	if(IsLocallyControlled())
	{
		if(FirstPersonMesh && FPMontage)
		{
			UAnimInstance* AnimInstance = FirstPersonMesh->GetAnimInstance();
			if(AnimInstance)
			{
				AnimInstance->Montage_Play(FPMontage, 1.0f);
			}
		}
	}
	else
	{
		if(ThirdPersonMesh && TpMontage)
		{
			UAnimInstance* AnimInstance = ThirdPersonMesh->GetAnimInstance();
			if(AnimInstance)
			{
				AnimInstance->Montage_Play(TpMontage, 1.0f);
			}
		}	
	}
}
```



### 武器和角色同步

武器只有一把，在自己视角中是附加到第一人称手臂上，在其他玩家视角是附加到全身模型角色的手上，在网络同步中更新了位置。

```c++
// 同步枪械位置到其他客户端的第三人称模型身上视角
UFUNCTION(NetMulticast, Unreliable)
void MulticastWeaponState(FVector NewLocation, FRotator NewRotation);
	
void UpdateCharacterVisibility();
 
void ADemoPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	UpdateCharacterVisibility(); // 更新可见性
	if (DefaultWeaponClass)
	{
		CurrentWeapon = GetWorld()->SpawnActor<ADemoWeapon>(DefaultWeaponClass);
		if (CurrentWeapon && FirstPersonMesh)
		{
			CurrentWeapon->SetOwner(this);
			if(IsLocallyControlled())
			{
				CurrentWeapon->AttachToComponent(FirstPersonMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("GridPoint"));
			}
			else
			{
				MulticastWeaponState(CurrentWeapon->GetActorLocation(), CurrentWeapon->GetActorRotation());
			}
 
		}
	}
}
 
void ADemoPlayerCharacter::UpdateCharacterVisibility()
{
	if(IsLocallyControlled())
	{
		FirstPersonMesh->SetVisibility(true);
		ThirdPersonMesh->SetVisibility(false);
		LegMesh->SetVisibility(true);
	}
	else
	{
		FirstPersonMesh->SetVisibility(false);
		ThirdPersonMesh->SetVisibility(true);
		LegMesh->SetVisibility(false);
	}
}
 
void ADemoPlayerCharacter::MulticastWeaponState_Implementation(FVector NewLocation, FRotator NewRotation)
{
	if(CurrentWeapon)
	{
		CurrentWeapon->SetActorLocation(NewLocation);
		CurrentWeapon->SetActorRotation(NewRotation);
		CurrentWeapon->AttachToComponent(ThirdPersonMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("TPGridPoint"));
	}
}
```

 

## 武器组件化

将武器的功能抽象成一个个独立可复用的模块，然后组合到武器实例上。比如射线枪和后坐力还有换弹都可以抽象成一个个功能，然后自由组合。

组件继承UActorComponent，我写了两个组件分别是**子弹管理组件**和**开火组件**。

### **子弹管理组件**

子弹管理组件定义了子弹数量和初始化子弹数量、判断子弹数量是否满足开火条件、子弹减少等方法。并将之前在武器类中子弹数量变化的代理和子弹数量的网络同步转移到当前子弹管理组件脚本中。

```c++
void UAmmoManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UAmmoManagerComponent, AmmoCount);
}
 
UAmmoManagerComponent::UAmmoManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}
 
void UAmmoManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	AmmoCount = MaxAmmo;
}
 
void UAmmoManagerComponent::InitializeAmmo(int32 InAmmoCount, int32 InMaxAmmo)
{
	MaxAmmo = InMaxAmmo;
	AmmoCount = InAmmoCount;
}
 
void UAmmoManagerComponent::ExpendAmmoCount()
{
	-- AmmoCount;
}
 
bool UAmmoManagerComponent::EnoughAmmoToFire()
{
	return AmmoCount > 0;
}
 
void UAmmoManagerComponent::SetMaxAmmo(int32 NewMaxAmmo)
{
	MaxAmmo = NewMaxAmmo;
	AmmoCount = FMath::Clamp(AmmoCount, 0, MaxAmmo);
	OnRep_AmmoCount();
}
 
void UAmmoManagerComponent::OnRep_AmmoCount()
{
	OnAmmoChanged.Broadcast(AmmoCount, MaxAmmo);
}
```



### **开火组件**

开火组件，创建了一个FireBase的基类开火组件，定义了一个OwnerWeapon用于获取当前开火的武器。

```c++
void UDemoFireBase::Initialize(ADemoWeapon* InWeapon)
{
	OwnerWeapon = InWeapon;
}
```

在武器类中的BeginPlay去查找是否存在开火组件，如果存在就调用这个函数。

```c++
void ADemoWeapon::BeginPlay()
{
	Super::BeginPlay();
 
	FireComponent = FindComponentByClass<UDemoFireBase>();
	if(FireComponent)
	{
		FireComponent->Initialize(this);
	}
}
```

投掷类开火和射线开火都是FireBase的派生类，重写开火函数实现多态，游戏运行时会走当前开火组件的逻辑。

 

#### **射线枪开火组件**

```c++
void UDemoFireRaycast::Fire()
{
	Super::Fire();
	if(!OwnerWeapon || !OwnerWeapon->CanFire()) return;
	FireRaycast();
}
void UDemoFireRaycast::FireRaycast()
{
	ADemoPlayerCharacter* PlayerCharacter = Cast<ADemoPlayerCharacter>(OwnerWeapon->GetOwner());
	if(PlayerCharacter)
	{
		FVector Start = PlayerCharacter->FirstPersonCamera->GetComponentLocation();
		FVector End = Start + PlayerCharacter->FirstPersonCamera->GetForwardVector() * 1000.f;
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(PlayerCharacter);
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, (ECollisionChannel)TR_WeaponTrace, Params);
		if(bHit)
		{
			AActor* HitActor = HitResult.GetActor();
			if(HitActor)
			{
				ADemoPlayerCharacter* HitCharacter = Cast<ADemoPlayerCharacter>(HitActor);
				if(HitCharacter)
				{
					HitCharacter->TakeDamage(Damage); // Damage也是射线组件定义的伤害
				}
			}
		}
	}
}
```



#### **投掷类榴弹开火组件**

```c++
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<ADemoGrenade> ProjectileClass;
	float LaunchSpeed = 1000.f;
	FName MuzzleSocketName = "FireMuzzleSocket";
 
void UDemoProjectileFire::Fire()
{
	Super::Fire();
	if(!ProjectileClass) return;
	if(OwnerWeapon)
	{
		USkeletalMeshComponent* MeshComponent = OwnerWeapon->FindComponentByClass<USkeletalMeshComponent>(); // 获取武器网格体为了获取枪口位置
		if(MeshComponent)
		{
			FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
			FRotator MuzzleRotation = MeshComponent->GetSocketRotation(MuzzleSocketName);
			ADemoGrenade* SpawnGrenade = GetWorld()->SpawnActor<ADemoGrenade>(ProjectileClass, MuzzleLocation, MuzzleRotation); // 在枪口位置生成手榴弹 
			if(SpawnGrenade)
			{
				UProjectileMovementComponent* ProjectileMovement = SpawnGrenade->FindComponentByClass<UProjectileMovementComponent>(); 
                            // 通过ProjectileMovementComponent来实现榴弹的发射有速度
				if(ProjectileMovement)
				{
					FVector LaunchDirection = MuzzleRotation.Vector();
					ProjectileMovement->Velocity = LaunchDirection * LaunchSpeed;
					ProjectileMovement->Activate();
					ProjectileMovement->OnProjectileStop.AddDynamic(SpawnGrenade, &ADemoGrenade::OnGrenadeStop); // UE自带的监听手榴弹停止的委托 当手榴弹停止运动时爆炸、伤害
				}
			}
		}
	}
}
```



**手榴弹类**

```c++
ADemoGrenade::ADemoGrenade()
{
 	// 初始化手榴弹组件 根组件是球形碰撞体（设置为BlockAll）挂载了GrenadeMesh（设置为NoCollsion），还有创建了ProjectileMovementComponent 并设置它的弹射 摩擦力等参数
	PrimaryActorTick.bCanEverTick = true;
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetSphereRadius(5);
	RootComponent = SphereCollision;
	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
	if(GrenadeMesh)
	{
		GrenadeMesh->SetupAttachment(SphereCollision);
	}
	
	ProjectileMoveComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	if(ProjectileMoveComp)
	{
		ProjectileMoveComp->bShouldBounce = true;
		ProjectileMoveComp->Bounciness = 0.4;
		ProjectileMoveComp->Friction = 0.6;
		ProjectileMoveComp->BounceVelocityStopSimulatingThreshold = 0.1;
		ProjectileMoveComp->InitialSpeed = 1000.f;
		ProjectileMoveComp->MaxSpeed = 1000.f;
	}
}
// 手榴弹停止运动事件触发的函数
void ADemoGrenade::OnGrenadeStop(const FHitResult& HitResult)
{
	FVector ExplosionCenter = HitResult.ImpactPoint; // 将投掷中心点传入ApplyExplosionDamage
	MulticastPlayExplosionEffect(ExplosionCenter); // 多播RPC让大家看到爆炸
	ApplyExplosionDamage(ExplosionCenter, ExplosionRadius);
}
 
void ADemoGrenade::ApplyExplosionDamage(FVector ExplosionCenter, float Radius)
{
	TArray<FOverlapResult> OverlapResults;
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(Radius);
       // 检测球形范围内有无物体
	bool bHit = GetWorld()->OverlapMultiByChannel(OverlapResults, ExplosionCenter, FQuat::Identity, (ECollisionChannel)TR_WeaponTrace, CollisionShape);
	if(bHit)
	{
              // 遍历所有被击中的玩家 让他们受伤
		for(const FOverlapResult& Result : OverlapResults)
		{
			AActor* HitActor = Result.GetActor();
			if(HitActor)
			{
				ADemoPlayerCharacter* HitCharacter = Cast<ADemoPlayerCharacter>(HitActor);
				if(HitCharacter)
				{
					HitCharacter->TakeDamage(Damage);
				}
			}
		}
	}
}
void ADemoGrenade::MulticastPlayExplosionEffect_Implementation(FVector ExplosionCenter)
{
	if(ExplosionEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionEffect,ExplosionCenter, FRotator::ZeroRotator);
	}
}
```



## **玩家复活**

玩家重生功能已经可以正常使用了。

在角色类中声明动态多播委托

```c++
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerDeath, ADemoPlayerCharacter*, DeadCharacter);
FOnPlayerDeath OnPlayerDeath;
```

玩家受伤TakeDamage函数中如果生命值<=0时，调用PlayerDead()函数。

```c++
void ADemoPlayerCharacter::TakeDamage(int32 DamageAmount)
{
	if(bIsDead) return;
 
	if (HasAuthority())  // 确保只有服务器才会处理伤害
	{
		Health -= DamageAmount;
		OnHealthChanged.Broadcast(Health);
 
		if (Health <= 0)
		{
			Health = 0;
			bIsDead = true;
			// 第一人称死亡蒙太奇没找到合适的
			PlayerDead();
		}
	}
}
void ADemoPlayerCharacter::PlayerDead()
{
	Multicast_PlayMontageAnimation(FPDeadMontage, TPDeadMontage); // 播放死亡动画
	GetCharacterMovement()->DisableMovement(); 
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetActorEnableCollision(false);
	OnPlayerDeath.Broadcast(this); // 重生 参数传入当前玩家
}
```

在GameMode中重写UE内置的RestartPlayer函数

```c++
void ADemoGameMode::RestartPlayer(AController* NewPlayer)
{
	if(NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
	{
		return;
	}
	TArray<AActor*> PlayerStarts;
   // 存储世界中所有的玩家出生点到TArray数组中
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
	if(PlayerStarts.Num() == 0)
	{
		return;
	}
   // 获取随机出生点 调用RestartPlayerAtPlayerStart，传入这个随机出生点
	APlayerStart* RandomStart = Cast<APlayerStart>(PlayerStarts[FMath::RandRange(0, PlayerStarts.Num() - 1)]);
	if(RandomStart)
	{
		RestartPlayerAtPlayerStart(NewPlayer, RandomStart);
		ADemoPlayerCharacter* PlayerCharacter = Cast<ADemoPlayerCharacter>(NewPlayer->GetPawn());
		if(PlayerCharacter)
		{
          // 每次出生都绑定玩家的死亡委托 恢复死亡前的状态
			PlayerCharacter->OnPlayerDeath.AddDynamic(this, &ADemoGameMode::ChangeDeath);
			PlayerCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			PlayerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			PlayerCharacter->SetActorEnableCollision(true);
		}
	}
}
```



## 出现的问题：

1.     **已解决** 角色混合空间移动缺少左右动画，导致左右腿移动时重叠。—— 骨骼没适配

2.     **已解决** 多人游戏中玩家奔跑只能自己看到 没办法被其他玩家看到 —— MaxWalkSpeed需要网络同步

3.     武器同步问题

beginplay中武器生成之后没办法正确同步，导致要么有客户端无法发射/有时候又正常，要么能发射但看不见手雷。——时序问题

```c++
void ADemoPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	MulticastUpdateVisibility();
	if (DefaultWeaponClass)
	{
		CurrentWeapon = GetWorld()->SpawnActor<ADemoWeapon>(DefaultWeaponClass);
		if (HasAuthority() && DefaultWeaponClass)
		{
			CurrentWeapon->SetOwner(this);
		}
	}
}
void ADemoPlayerCharacter::OnRep_CurrentWeapon()
{
	if(CurrentWeapon)
	{
		if(IsLocallyControlled())
		{
			CurrentWeapon->AttachToComponent(FirstPersonMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("GridPoint"));
		}
		else
		{
			CurrentWeapon->AttachToComponent(ThirdPersonMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("TPGridPoint"));
		}
	}
}
```

