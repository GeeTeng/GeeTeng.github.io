---
title: "Warrior项目笔记3"
date: 2025-11-13
tags: [UE5]
description: "Warrior项目笔记第3部分 小怪制作 角色属性 自定义计算类的GE造成伤害"
showDate: true
math: true
chordsheet: true
---



### 怪物生成武器

创建新的cpp文件：

1. UEnemyCombatComponent : public UPawnCombatComponent
2. UWarriorEnemyGameplayAbility : public UWarriorGameplayAbility
3. AWarriorEnemyCharacter : public AWarriorBaseCharacter
4. UDataAsset_EnemyStartUpData : public UDataAsset_StartUpDataBase

并在编辑器中创建：

ABP_Enemy_Base

|__ABP_Guardian

BS_Guardian_Default

BP_EnemyCharacter_Base

|__BP_Gruntling_Base

​      |__BP_Gruntling_Guardian

在AWarriorEnemyCharacter中创建一个EnemyCombatComponent组件，设置怪物角色的基础属性。

在UWarriorEnemyGameplayAbility中和UWarriorHeroGameplayAbility一样，需要创建如下函数，暴露给蓝图，用于获取怪物角色和怪物的Combat组件。

```c++
UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
AWarriorEnemyCharacter* GetEnemyCharacterFromActorInfo();
UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
UEnemyCombatComponent* GetEnemyCombatComponentFromActorInfo();
private:
TWeakObjectPtr<AWarriorEnemyCharacter> CachedWarriorEnemyCharacter;
```

在UDataAsset_EnemyStartUpData中重写父类的`GiveToAbilitySystemComponent`。该函数会在怪物角色的PossessedBy中调用。

```c++
void UDataAsset_EnemyStartUpData::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,
    int32 ApplyLevel)
{
    Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);
    if (!EnemyCombatAbilities.IsEmpty())
    {
       for (const TSubclassOf<UWarriorEnemyGameplayAbility>& AbilityClass : EnemyCombatAbilities)
       {
          if (!AbilityClass) continue;
          FGameplayAbilitySpec AbilitySpec(AbilityClass);
          AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
          AbilitySpec.Level = ApplyLevel;
          InASCToGive->GiveAbility(AbilitySpec);
       }
    }
}
```

因为玩家只有一个，所以同步加载也无所谓，但是怪物有很多个，所以我们要使用异步加载。

创建Lambda表达式，在其中调用UDataAsset_EnemyStartUpData的`GiveToAbilitySystemComponent`。

```c++
void AWarriorEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitEnemyStartUpData();
}

void AWarriorEnemyCharacter::InitEnemyStartUpData()
{
	if (CharacterStartUpData.IsNull())
	{
		return;
	}
	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CharacterStartUpData.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[this]()
			{
				if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpData.Get())
				{
					LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
				}
			})
	);
}
```

添加新的GameplayTag_Enemy_Weapon，用于怪物出生时生成武器。

创建新的GA_Gruntling_SpawnWeapon继承GA_SpawnWeaponBase。在其中配置好参数，并且在DA_Guardian（继承UDataAsset_EnemyStartUpData ）中配置该GA。即可实现怪物出生即生成武器。



### 创建属性

创建6个属性分别是：CurrentHealth、MaxHealth、CurrentRage、MaxRage、AttackPower、DefensePower

并在构造函数中初始化它。

```c++
// AttributeSet 类里专门用来管理 Gameplay 属性（Attribute）的宏级封装工具
// 自动为某个 FGameplayAttributeData 属性生成访问、修改、初始化等接口
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UPROPERTY(BlueprintReadOnly, Category = "Health")
FGameplayAttributeData CurrentHealth;
ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, CurrentHealth)
    
UWarriorAttributeSet::UWarriorAttributeSet()
{
	InitCurrentHealth(1.f);
	InitMaxHealth(1.f);
	InitCurrentRage(1.f);
	InitMaxRage(1.f);
	InitAttackPower(1.f);
	InitDefensePower(1.f);
}
```

创建CT_HeroStats存储玩家出生时属性配置表

![28](/images/UE/WarriorProject/28.png)

之后创建GE_HeroStartUp填写要修改的属性。这是为了让玩家初始拥有最大血量、最大怒气值、攻击力和防御力。

![27](../../../static/images/UE/WarriorProject/27.png)

之后创建一个同样用作初始化玩家属性的GE_Hero_Static，设置**Magnitude Calculation Type**为**Attribute Based**，该类型是基于某个属性去做修改。

我们要在这里修改当前生命值和当前怒气值。所以**Attribute To Capture**设置为最大生命值和最大怒气值。

将AttributeSource设为Target。

![29](/images/UE/WarriorProject/29.png)

之后再UDataAsset_StartUpDataBase中定义一个初始化GE数组，用于初始化施加GE。

```c++
UPROPERTY(EditDefaultsOnly, Category = "StartUpData")
TArray<TSubclassOf<UGameplayEffect>> StartUpGameplayEffects;
```

在`GiveToAbilitySystemComponent`中添加如下逻辑，即可完成玩家初始化生命值等属性。

```c++
void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,
                                                              int32 ApplyLevel)
{
    check(InASCToGive);
    GrantAbilities(ActivateOnGivenAbilities, InASCToGive, ApplyLevel);
    GrantAbilities(ReactiveAbilities, InASCToGive, ApplyLevel);
    if (!StartUpGameplayEffects.IsEmpty())
    {
       for (const TSubclassOf<UGameplayEffect>& EffectClass : StartUpGameplayEffects)
       {
          if (!EffectClass) continue;
          UGameplayEffect* EffectCDO = EffectClass->GetDefaultObject<UGameplayEffect>();
          InASCToGive->ApplyGameplayEffectToSelf(
             EffectCDO,
             ApplyLevel,
             InASCToGive->MakeEffectContext()
             );
       }
    }
}
```



### 怪物属性与GE

与角色同理，但是怪物的GE没有怒气值。创建一个新的CurveTable配置好怪物的属性。并且在怪物的DA中也配置好初始的GE。

为了在调试中可以看到怪物属性，将项目文件夹中Config的DefaultGame.ini进行修改。

```
[/Script/GameplayAbilities.AbilitySystemGlobals]
bUseDebugTargetFromHud = true
```



### 武器碰撞检测

武器不能一直开着碰撞，应该在动画中添加通知**ANS_ToggleWeaponCollision**，使得其在动画播放到合适的时候禁用、开启武器碰撞。

但是多个角色发起很多次攻击，就要获取很多次武器（通过PawnCombatComponent获取武器），这样多次查找很浪费性能。

因此我们需要创建一个接口**UPawnCombatInterface**，当中只有一个获取PawnCombatComponent的纯虚函数不做实现，然后让AWarriorBaseCharacter添加这个接口，重新该函数返回nullptr。

而子类AWarriorEnemyCharacter和AWarriorHeroCharacter返回其Combat组件。

```c++
virtual UPawnCombatComponent* GetPawnCombatComponent() const = 0;
```

```c++
// ~ Begin IPawnCombatInterface Interface
virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
// ~ End IPawnCombatInterface Interface
```

为了在蓝图中可以获取到，新增两个函数：

```c++
static UPawnCombatComponent* NativeGetPawnCombatComponentFromActor(AActor* InActor);

UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary", meta = (DisplayName = "GetPawnCombatComponentFromActor", ExpandEnumAsExecs = "OutValidType"))
static UPawnCombatComponent* BP_GetPawnCombatComponentFromActor(AActor* InActor, EWarriorValidType& OutValidType);

UPawnCombatComponent* UWarriorFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor);
	if (IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}
	return nullptr;
}
UPawnCombatComponent* UWarriorFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor,
	EWarriorValidType& OutValidType)
{
	UPawnCombatComponent* CombatComponent = NativeGetPawnCombatComponentFromActor(InActor);
	OutValidType = CombatComponent? EWarriorValidType::Valid : EWarriorValidType::Invalid;
	return CombatComponent;
}
```

在PawnCombatComponent中添加一个`ToggleWeaponCollision`函数，用于根据布尔值启用 / 禁用武器的碰撞体，会在ANS_ToggleWeaponCollision调用。

```c++
void UPawnCombatComponent::ToggleWeaponCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
    if (ToggleDamageType == EToggleDamageType::CurrentEquippedWeapon)
    {
       AWarriorWeaponBase* WeaponToToggle = GetCharacterCurrentEquippedWeapon();
       check(WeaponToToggle);
       if (bShouldEnable)
       {
          WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
       }
       else
       {
          WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
       }
    }
}
```

之后将ANS_ToggleWeaponCollision添加到动画中。

在WarriorWeaponBase的BeginPlay中绑定BeginOverlap和EndOverlap函数。

`OnComponentBeginOverlap`和`OnComponentEndOverlap`是一个动态多播委托，使用`AddUniqueDynamic`可以防止重复绑定。

```c++
void AWarriorWeaponBase::BeginPlay()
{
    Super::BeginPlay();
    WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxBeginOverlap);
    WeaponCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxEndOverlap);
}
```

> 为什么不能在构造函数中绑定？

当构造函数执行时候C++正在构建CDO，这个时候对象只是“模板”，还未注册到世界中，蓝图在生成派生类时，会复制CDO中的属性结构，然后自己再重新生成实例。因此绑定会丢失。

定义两个委托用来监听武器击中和离开敌人的事件。并在beginoverlap和endoverlap中进行执行。

```c++
DECLARE_DELEGATE_OneParam(FOnTargetInteractedDelegate, AActor*);	
FOnTargetInteractedDelegate OnWeaponHitTarget;
FOnTargetInteractedDelegate OnWeaponPulledFromTarget;
void AWarriorWeaponBase::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* WeaponOwningPawn = GetInstigator<APawn>();
	checkf(WeaponOwningPawn, TEXT("Forget to assign an instigator as the owning pawn of the weapon: %s"), *GetName());
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (WeaponOwningPawn != HitPawn)
		{
			OnWeaponHitTarget.ExecuteIfBound(OtherActor);
		}
	}
}

void AWarriorWeaponBase::OnCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* WeaponOwningPawn = GetInstigator<APawn>();
	checkf(WeaponOwningPawn, TEXT("Forget to assign an instigator as the owning pawn of the weapon: %s"), *GetName());
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (WeaponOwningPawn != HitPawn)
		{
			OnWeaponPulledFromTarget.ExecuteIfBound(OtherActor);
		}
	}
}
```

在`UPawnCombatComponent::RegisterSpawnedWeapon`中进行绑定委托关联的函数OnHitTargetActor、OnWeaponPulledFromTargetActor。

```c++
InWeaponToRegister->OnWeaponHitTarget.BindUObject(this, &ThisClass::OnHitTargetActor);
InWeaponToRegister->OnWeaponPulledFromTarget.BindUObject(this, &ThisClass::OnWeaponPulledFromTargetActor);
```

这两个函数在其子类UHeroCombatComponent要做重写。需要重新定义一个`WarriorGameplayTags::Shared_Event_MeleeHit`，也就是该函数会在攻击到敌人时会发送一个GameplayEvent给自己。

```c++
void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
{
    if (OverlappedActors.Contains(HitActor))
    {
       return;
    }
    OverlappedActors.AddUnique(HitActor);
    FGameplayEventData Data;
    Data.Instigator = GetOwningPawn();
    Data.Target = HitActor;
    UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
       GetOwningPawn(),
       WarriorGameplayTags::Shared_Event_MeleeHit,
       Data
       );
}
```

在GA中可以接受这个event执行逻辑。



### 应用伤害

应用伤害流程

GA—— Make Gameplay Effect Spec Handle —— Apply Handle To Target —— Gameplay Effect Execution Calculation —— Attribute Set —— Notify UI

在WarriorHeroGameplayAbility中定义一个新的函数用于把一次攻击的上下文信息——（是谁打的、什么攻击、伤害多少）打包成一个 GameplayEffectSpec，供 GAS 系统分发处理。

创建一个新的tag——`Shared_SetByCaller_BaseDamage`

```c++
// EffectClass要生成的GE(比如GA_Damage) InWeaponBaseDamage伤害值 InCurrentAttackTypeTag攻击类型标签（比如轻击重击） InCurrentComboCount连击次数
FGameplayEffectSpecHandle UWarriorHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(
    TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag,
    int32 InCurrentComboCount)
{
    check(EffectClass);
    // ContextHandle 记录 GE的来源信息（这个能力发起的、来源对象、施加效果者）
    FGameplayEffectContextHandle ContextHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
    ContextHandle.SetAbility(this);
    ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
    ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
    // 创建GE EffectClass 是函数传入的 这个能力等级 刚才构建的上下文信息 ContextHandle
    FGameplayEffectSpecHandle EffectSpecHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
       EffectClass,
       GetAbilityLevel(),
       ContextHandle
    );
    // 动态传入伤害数值
    EffectSpecHandle.Data->SetSetByCallerMagnitude(
       WarriorGameplayTags::Shared_SetByCaller_BaseDamage,
       InWeaponBaseDamage
    );
    // 如果攻击标签有效 则再加入参数 攻击类别（轻击重击）连击次数
    if (InCurrentAttackTypeTag.IsValid())
    {
       EffectSpecHandle.Data->SetSetByCallerMagnitude(
          InCurrentAttackTypeTag,
          InCurrentComboCount
       );
    }
    return EffectSpecHandle;
}
```

接下来就可以在蓝图中调用该函数了，为了将输入传入这个函数，我们需要有一个1GE蓝图类、2还有一个能获取武器伤害数值的函数、3当前攻击的类别和4连击次数。

1. 因此首先需要创建一个GE类，我们只需要给他配置一个计算类，这个计算类是我们创建的C++新类**UGEExecCalc_DamageTaken**继承自UGameplayEffectExecutionCalculation。

![32](/images/UE/WarriorProject/32.png)

2. 在HeroCombatComponent中添加两个函数分别是返回玩家武器和武器中配置的WeaponBaseDamage（一个Curve Table，稍后会说创建它）得到这个表中武器等级对应的伤害。

   ```c++
   AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCurrentEquippedWeapon() const
   {
       return Cast<AWarriorHeroWeapon>(GetCharacterCurrentEquippedWeapon());
   }
   float UHeroCombatComponent::GetHeroCurrentEquippedWeaponDamageAtLevel(float InLevel) const
   {
       return GetHeroCurrentEquippedWeapon()->HeroWeaponData.WeaponBaseDamage.GetValueAtLevel(InLevel);
   }
   ```

   在WarriorStructTypes中的FWarriorHeroWeaponData中添加新成员，是一个Curve Table用于配置武器对应等级能造成的伤害。

   ```
   UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
   FScalableFloat WeaponBaseDamage;
   ```

   之后在编辑器中创建CT_HeroWeaponStats并在武器类中配置这个表。

之后在WarriorGameplayAbility中创建两个函数，它们都是用来施加GE到目标的，但是分别用于C++和蓝图。

需要添加一个新的enum结构`EWarriorSuccessType`在WarriorEnumTypes中。

```c++
FActiveGameplayEffectHandle NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle);

UFUNCTION(BlueprintCallable, Category = "Warrior|Ability", meta = (DisplayName = "Apply Gameplay Effect Spec Handle To Target Actor", ExpandEnumAsExecs = "OutSuccessType"))
FActiveGameplayEffectHandle BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle, EWarriorSuccessType& OutSuccessType);

FActiveGameplayEffectHandle UWarriorGameplayAbility::NativeApplyEffectSpecHandleToTarget(AActor* TargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	check(TargetASC && InSpecHandle.IsValid());
	return GetWarriorAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*InSpecHandle.Data,
		TargetASC
	);
}

FActiveGameplayEffectHandle UWarriorGameplayAbility::BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor,
	const FGameplayEffectSpecHandle& InSpecHandle, EWarriorSuccessType& OutSuccessType)
{
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = NativeApplyEffectSpecHandleToTarget(TargetActor, InSpecHandle);
	OutSuccessType = ActiveGameplayEffectHandle.WasSuccessfullyApplied() ? EWarriorSuccessType::Successful : EWarriorSuccessType::Failed;
	return ActiveGameplayEffectHandle;
}

```

![33](/images/UE/WarriorProject/33.png)

值得注意的是**UsedComboCount**这个变量是CurrentLightAttackComboCount提升的新变量，目的是为了在sequence先执行伤害施加时不要立马连击次数++了，所以要延迟一下变量的更新。

接下来我们需要去处理伤害数值的计算，这个伤害是融合了连击次数、武器基础伤害（武器等级）、攻击类型以及角色的攻击力。

首先在伤害计算类里，注册并声明要从 AttributeSet 中读取的属性。这里有两种方法读取属性。

1. 宏`DECLARE_ATTRIBUTE_CAPTUREDEF` 版

   ```c++
   struct FWarriorDamageCapture
   {
   	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower)
   	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower)
   	FWarriorDamageCapture()
   	{
   		// 从施加者的UWarriorAttributeSet获取AttackPower，每次都取最新值（false）
   		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, AttackPower, Source, false)
   		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DefensePower, Target, false)
   	}
   };
   // 为了RelevantAttributesToCapture能够全局访问这个定义 做了一个单例静态实例
   static const FWarriorDamageCapture& GetWarriorDamageCapture()
   {
       static FWarriorDamageCapture WarriorDamageCapture;
       return WarriorDamageCapture;
   }
   UGEExecCalc_DamageTaken::UGEExecCalc_DamageTaken()
   {
   	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().AttackPowerDef);
   	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DefensePowerDef);
   }
   ```

   这个宏是GameplayAbilities 提供的简化封装，用来自动生成：

   ```
   FGameplayEffectAttributeCaptureDefinition AttackPowerDef;
   FGameplayEffectAttributeCaptureDefinition AttackPowerProperty;
   ```

2. 手动创建 `FGameplayEffectAttributeCaptureDefinition`

   ```c++
   FProperty* AttackPowerProperty = FindFieldChecked<FProperty>(
       UWarriorAttributeSet::StaticClass(),
       GET_MEMBER_NAME_CHECKED(UWarriorAttributeSet, AttackPower)
   );
   FGameplayEffectAttributeCaptureDefinition AttackPowerCaptureDefinition(
       AttackPowerProperty,
       EGameplayEffectAttributeCaptureSource::Source,
       false
   );
   RelevantAttributesToCapture.Add(GetWarriorDamageCapture().AttackPowerDef);
   ```

   相当于上面方法的宏展开版，但是很麻烦。



