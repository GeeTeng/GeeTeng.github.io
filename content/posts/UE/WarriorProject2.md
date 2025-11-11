---
title: "Warrior项目笔记 2"
date: 2025-11-10
tags: [UE5]
description: "Warrior项目笔记第2部分 武器装配、武器能力与连击系统"
showDate: true
math: true
chordsheet: true
---





## 整体框架概览：

 ![26](/images/UE/WarriorProject/26.png)



## BP Function Library 蓝图函数库

**蓝图函数库是静态函数的集合，提供与Gameplay对象无关的实用工具功能，**

比如UGameplayStatics和UKismetSystemLibrary

有时候你写游戏逻辑，会发现：

> “我想写一个通用的小函数，比如把某个 Actor 转成 Pawn 并拿到它的 AIController。”

但这个逻辑既不是某个角色特有的、也不是某个组件专属的。它只是一个到处都能用的“小工具函数”。这时候你就不该把它写在 `Actor`、`Component`、`GameInstance` 里，因为那样会让这个逻辑绑死在具体类型上。

👉 所以 Unreal 提供了一个专门的“函数容器”——`UBlueprintFunctionLibrary`。



## 角色扩展组件

PawnExtensionComponentBase

|__PawnCombatComponent

​     |__HeroCombatComponent

​     |__EnemyCombatComponent

`static_assert` 是 **编译期断言**，会在编译时检查条件是否成立，不成立就报编译错误。

创建一个PawnExtensionComponentBase，用于角色组件，统一访问Pawn、Controller等。

```c++
protected:
	template<class T>
	T* GetOwningPawn() const
	{
		// TPointerIsConvertibleFromTo 测试 T 是否可转换为 APawn
		static_assert(TPointerIsConvertibleFromTo<T,APawn>::Value, "'T' Template Parameter to GetPawn must be derived from APawn");
		return CastChecked<T>(GetOwner());
	}

	APawn* GetOwningPawn() const
	{
		return GetOwningPawn<APawn>();
	}

	template<class T>
	T* GetOwningController() const
	{
		static_assert(TPointerIsConvertibleFromTo<T,AController>::Value, "'T' Template Parameter to GetController must be derived from AController");
		return GetOwningPawn<APawn>()->GetController<T>();
	}
```

之后在WarriorHeroCharacter中定义一个能够获取HeroCombatComponent组件的函数。

```c++
FORCEINLINE UHeroCombatComponent* GetHeroCombatComponent() const { return HeroCombatComponent; }
```



## 注册生成的武器

PawnCombatComponent中定义一个map存储tag-weapon。

三个函数分别是注册生成的weapon到map中；通过tag获取角色携带的武器；获取角色当前装配的武器。

将会在蓝图中调用这些函数。当GA_Shared_SpawnWeapon中生成武器后调用RegisterSpawnedWeapon

```c++
public:
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	void RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister, AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon = false);

	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const;

	UPROPERTY(BlueprintReadWrite, Category = "Warrior|Combat")
	FGameplayTag CurrentEquippedWeaponTag;

	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon() const;
	
private:
	TMap<FGameplayTag, AWarriorWeaponBase*> CharacterCarriedWeaponMap;
	
```

```c++
// 把生成好的武器注册到角色的已持有武器表中，并可选地设为当前装备武器。
void UPawnCombatComponent::RegisterSpawnedWeapon(FGameplayTag InWeaponTagToRegister,
                                                 AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon)
{
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister), TEXT("A named %s has already been added as carried weapon"),*InWeaponTagToRegister.ToString());
	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);
	if (bRegisterAsEquippedWeapon)
	{
		CurrentEquippedWeaponTag = InWeaponTagToRegister;
	}
}

// 根据武器标签，返回对应的武器对象指针
AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTagToGet) const
{
	if (CharacterCarriedWeaponMap.Contains(InWeaponTagToGet))
	{
		if (AWarriorWeaponBase* const* FoundWeapon = CharacterCarriedWeaponMap.Find(InWeaponTagToGet))
		{
			return *FoundWeapon;
		} 
	}
	return nullptr;
}

// 获取当前装备的武器
AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	if (!CurrentEquippedWeaponTag.IsValid())
	{
		return nullptr;
	}
	return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}
```

之后就可以在Hero_SpawnAxe中配置注册的tag了。

![6](/images/UE/WarriorProject/6.jpg)



在WarriorGameAbility中添加两个新函数用于获取CombatComponent和ASC。

```c++
UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;

UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponentFromActorInfo() const;
UPawnCombatComponent* UWarriorGameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponent>();
}

UWarriorAbilitySystemComponent* UWarriorGameplayAbility::GetWarriorAbilitySystemComponentFromActorInfo() const
{
	return Cast<UWarriorAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}
```

然后创建WarriorHeroGameplayAbility去继承WarriorGameAbility

```c++
public:
UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
AWarriorHeroCharacter* GetHeroCharacterFromActorInfo();

UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
AWarriorHeroController* GetHeroControllerFromActorInfo();

UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
UHeroCombatComponent* GetHeroCombatComponentFromActorInfo();

private:
TWeakObjectPtr<AWarriorHeroCharacter> CachedWarriorHeroCharacter;
TWeakObjectPtr<AWarriorHeroController> CachedWarriorHeroController;

AWarriorHeroCharacter* UWarriorHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
	if (!CachedWarriorHeroCharacter.IsValid())
	{
		CachedWarriorHeroCharacter = Cast<AWarriorHeroCharacter>(CurrentActorInfo->AvatarActor);
	}
	return CachedWarriorHeroCharacter.IsValid() ? CachedWarriorHeroCharacter.Get() : nullptr;
}

AWarriorHeroController* UWarriorHeroGameplayAbility::GetHeroControllerFromActorInfo()
{
	if (!CachedWarriorHeroController.IsValid())
	{
		CachedWarriorHeroController = Cast<AWarriorHeroController>(CurrentActorInfo->PlayerController);
	}
	return CachedWarriorHeroController.IsValid() ? CachedWarriorHeroController.Get() : nullptr;
}

UHeroCombatComponent* UWarriorHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
	return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}
```

创建一个GA蓝图WarriorHeroGameplayAbility，在当中可以调用这些函数去获取组件、controller、character。

## 装配武器

绑定装配武器的输入

在WarriorGameplayTags中添加新Tag

```c++
WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_EquipAxe);
WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_UnEquipAxe);
```

DataAsset_InputConfig中定义

```c++
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
TArray<FWarriorInputActionConfig> AbilityInputActions;
```

在WarriorInputComponent中定义`BindAbilityInputAction`来绑定能力输入。

为每个技能Action（`FWarriorInputActionConfig`）分别绑定：

- `ETriggerEvent::Started` → 调用 `InputPressedFunc`
- `ETriggerEvent::Completed` → 调用 `InputReleasedFunc`

```c++
template <class UserObject, typename CallbackFunc>
inline void UWarriorInputComponent::BindAbilityInputAction(const UDataAsset_InputConfig* InInputConfig, UserObject* ContextObject,
    CallbackFunc InputPressedFunc, CallbackFunc InputReleasedFunc)
{
    checkf(InInputConfig,TEXT("InputConfigDataAsset is null"));
    for (const FWarriorInputActionConfig& AbilityInputActionConfig : InInputConfig->AbilityInputActions)
    {
       if (!AbilityInputActionConfig.IsValid()) continue;
       BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Started, ContextObject, InputPressedFunc, AbilityInputActionConfig.InputTag);
       BindAction(AbilityInputActionConfig.InputAction, ETriggerEvent::Completed, ContextObject, InputReleasedFunc, AbilityInputActionConfig.InputTag);
    }
}
```



在DataAsset_HeroStartUpData中定义结构体`FWarriorHeroAbilitySet`。该结构体包含角色初始化所需的技能。

```c++
USTRUCT(BlueprintType)
struct FWarriorHeroAbilitySet
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UWarriorGameplayAbility> AbilityToGrant;
	bool IsValid() const;
};
```

在该文件中重写父类DataAsset_StartUpDataBase的`GiveToAbilitySystemComponent`，这个函数在角色类中的PossessedBy中被调用。

```c++
void UDataAsset_HeroStartUpData::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASCToGive,
    int32 ApplyLevel)
{
    Super::GiveToAbilitySystemComponent(InASCToGive, ApplyLevel);
    for (const FWarriorHeroAbilitySet& AbilitySet : HeroStartUpAbilitySets)
    {
       if (!AbilitySet.IsValid()) continue;
       FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
       AbilitySpec.SourceObject = InASCToGive->GetAvatarActor();
       AbilitySpec.Level = ApplyLevel;
       AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);
       InASCToGive->GiveAbility(AbilitySpec);
    }
}
```

在WarriorAbilitySystemComponent中定义两个函数，用于绑定输入的回调函数。

```c++
void OnAbilityInputPressed(const FGameplayTag& InInputTag);
void OnAbilityInputReleased(const FGameplayTag& InInputTag);

void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if (!InInputTag.IsValid())
	{
		return;
	}
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)) continue;
		TryActivateAbility(AbilitySpec.Handle);
	}
}
```

同样的在WarriorHeroCharacter里也有调用刚才WarriorAbilitySystemComponent定义的函数

```c++
void AWarriorHeroCharacter::Input_AbilityInputPressed(FGameplayTag InInputTag)
{
    WarriorAbilitySystemComponent->OnAbilityInputPressed(InInputTag);
}

void AWarriorHeroCharacter::Input_AbilityInputReleased(FGameplayTag InInputTag)
{
    WarriorAbilitySystemComponent->OnAbilityInputReleased(InInputTag);
}
```

这两个函数目的是为了调用WarriorInputComponent中定义的`BindAbilityInputAction`

```c++
WarriorInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &ThisClass::Input_AbilityInputPressed, &ThisClass::Input_AbilityInputReleased);
```

### 整体流程

![8](/images/UE/WarriorProject/8.png)

赋予能力：DataAsset_HeroStartUpData中重写父类的`GiveToAbilitySystemComponent`，在其中调用WarriorAbilitySystemComponent的GiveAbility去赋予能力。



激活能力：

在角色类的SetupPlayerInputComponent中调用WarriorInputComponent的`BindAbilityInputAction`，并传入按下和释放按键的回调函数。

DA_InputConfig中有配置好Input Tag和InputAction，也就是`FWarriorInputActionConfig`结构体。

![7](/images/UE/WarriorProject/7.png)

`BindAbilityInputAction`负责把输入系统和GAS连接起来，通过传入InputTag使当按下输入时，调用带 InputTag 的函数，也就是角色类的回调函数`Input_AbilityInputPressed`、`Input_AbilityInputReleased`。

这两个回调函数会调用WarriorAbilitySystemComponent的`OnAbilityInputPressed`、`OnAbilityInputReleased`函数。

其中ASC系统中`OnAbilityInputPressed`会通过`TryActivateAbility`来实现GA。



### 装配动画蒙太奇

首先创建动画通知蓝图类，添加一个重写函数`ReceivedNotify`。

![9](/images/UE/WarriorProject/9.png)

再装配武器的动画蒙太奇中添加该动画通知，并在Details面板中设置EventTag为新建的Player.Event.Equip.Axe。

在GA_Hero_EquipAxe中播放蒙太奇并在接收到GameplayEvent时使武器附着在右手上。

![10](/images/UE/WarriorProject/10.png)

![11](/images/UE/WarriorProject/11.png)



### 动画层

创建动画层接口ALI_Hero，之后再ABP_Hero中的类设置配置继承接口层为这个动画接口。

由于ABP_Hero继承C++中的WarriorCharacterAnimInstance，在该文件中有`AWarriorHeroCharacter* OwningHeroCharacter`;因此可以通过属性存取获得到OwningCharacter的Combat组件，也就可以得到当前武器的Tag。因此可以来混合拿武器和不拿武器的动画。

![12](/images/UE/WarriorProject/12.png)

之后创建MasterLayer_Hero继承WarriorHeroLinkedAnimLayer，同样在类设置配置继承接口层为ALI_Hero。

在WarriorHeroLinkedAnimLayer中创建该函数，用于获取AnimInstance。

```c++
UWarriorHeroAnimInstance* UWarriorHeroLinkedAnimLayer::GetHeroAnimInstance() const
{
    return Cast<UWarriorHeroAnimInstance>(GetOwningComponent()->GetAnimInstance()) ;
}
```

![13](/images/UE/WarriorProject/13.png)

之后在创建一个继承MasterLayer_Hero的AnimLayer_HeroAxe，用于播放持有武器的混合动画。在类默认中配置DefaultLocomotionBlendSpace为新建的BlendSpace1D持有武器的混合空间动画。

接下来需要将这些层贯穿起来。

创建新的cpp文件类型为None用于自定义结构体。首先定义一个`FWarriorHeroWeaponData`

```c++
class UWarriorHeroLinkedAnimLayer;

USTRUCT(BlueprintType)
struct FWarriorHeroWeaponData
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UWarriorHeroLinkedAnimLayer> WeaponAnimLayerToLink;
};
```

之后要在武器类中获取这个结构体，这样暴露在蓝图中可以配置结构体的`WarriorHeroLinkedAnimLayer`。

```c++
UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
FWarriorHeroWeaponData HeroWeaponData;
```

![14](/images/UE/WarriorProject/14.png)

然后在HeroCombatComponent中定义一个通过Tag获取角色持有武器的函数。与父类函数相同，只不过要做一个强转而已。

```c++
AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
    return Cast<AWarriorHeroWeapon>(GetCharacterCarriedWeaponByTag(InWeaponTag));
}
```

在GA中添加如下的逻辑，通过LinkAnimClassLayers来实现关联动画层。这样就可以完成了武器装配后动画的切换了。

![15](/images/UE/WarriorProject/15.png)



## 卸下武器

声明GameplayTag

```c++
WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Equip_Axe);
WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_UnEquip_Axe);
```

将tag设置到GA中

![16](/images/UE/WarriorProject/16.png)

之后创建收回武器的蒙太奇动画，添加动画通知并设置EventTag。在GA蓝图中像装配武器那样处理接收到event之后的逻辑。

创建一个InputAction用于收回武器的输入触发，并配置在DA_InputConfig中，同样需要一个GameplayTag。

```c++
WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_UnEquipAxe);
```

![17](/images/UE/WarriorProject/17.png)



思考一个问题：玩家卸下武器的能力应该要在玩家出生时就拥有，还是在装配武器后才拥有。

显而易见，应该在装配武器后才会拥有卸下武器的能力，因此我们需要在HeroWeaponData中（那个结构体，已有AnimLayerToLink成员）添加一些新的成员。

HeroWeaponData

|__AnimLayerToLink（切换持有武器的动画）

|__DefaultWeaponAbility(获取武器能力，比如卸下武器、轻击、重击)

|__InputMappingContext（更改按键绑定）

将FWarriorHeroAbilitySet结构体从DataAsset_HeroStartUpData移动到WarriorStructType中。之后定义新的成员分别是`WeaponInputMappingContext`和`DefaultWeaponAbilities`

```c++
USTRUCT(BlueprintType)
struct FWarriorHeroWeaponData
{
    GENERATED_BODY()
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<UWarriorHeroLinkedAnimLayer> WeaponAnimLayerToLink;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    UInputMappingContext* WeaponInputMappingContext;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
    TArray<FWarriorHeroAbilitySet> DefaultWeaponAbilities;
};
```

创建一个新的IMC，其中有武器能力的输入，在BP_HeroAxe中修改HeroWeaponData。

![18](/images/UE/WarriorProject/18.png)

在WarriorAbilitySystemComponent中增加一个函数用于赋予武器能力。

在`GiveAbility`后GAS会返回一个`FGameplayAbilitySpecHandle`，`OutGrantedAbilitySpecHandles` 用来**收集并保存每个被授予的能力的句柄**，方便你之后按句柄精确地移除、查找或管理这些能力。

```c++
void UWarriorAbilitySystemComponent::GrantHeroWeaponAbilities(
    const TArray<FWarriorHeroAbilitySet>& InDefaultWeaponAbilities, int32 ApplyLevel,
    TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandles)
{
    if (InDefaultWeaponAbilities.IsEmpty())
    {
       return;
    }
    for (const FWarriorHeroAbilitySet& AbilitySet : InDefaultWeaponAbilities)
    {
       if (!AbilitySet.IsValid()) continue;
       FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
       AbilitySpec.SourceObject = GetAvatarActor();
       AbilitySpec.Level = ApplyLevel;
       AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);
       OutGrantedAbilitySpecHandles.AddUnique(GiveAbility(AbilitySpec));
    }
}
```

在GA_Hero_EquipAxe中创建新函数`HandleEquipWeapon`用来替代之前写的逻辑。

![19](/images/UE/WarriorProject/19.png)

在WarriorHeroWeapon中添加两个函数。装配武器蓝图中在`GrantHeroWeaponAbilities`后面调用`AssignGrantedAbilitySpecHandles`。



```c++
void AWarriorHeroWeapon::AssignGrantedAbilitySpecHandles(const TArray<FGameplayAbilitySpecHandle>& InSpecHandles)
{
    GrantAbilitySpecHandles = InSpecHandles;
}

TArray<FGameplayAbilitySpecHandle> AWarriorHeroWeapon::GetGrantedAbilitySpecHandles() const
{
    return GrantAbilitySpecHandles;
}
```

在WarriorAbilitySystemComponent中添加一个新函数，用于移除武器赋予的能力。

```c++
void UWarriorAbilitySystemComponent::RemoveGrantedHeroWeaponAbilities(
    TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
    if (InSpecHandlesToRemove.IsEmpty())
    {
       return;
    }
    for (const FGameplayAbilitySpecHandle& SpecHandle : InSpecHandlesToRemove)
    {
       if (SpecHandle.IsValid())
       {
          ClearAbility(SpecHandle);
       }
    }
    InSpecHandlesToRemove.Empty();
}
```

卸下武器的GA中写如下逻辑

![25](/images/UE/WarriorProject/25.png)



## 武器攻击

同样需要声明GameplayTag。

```c++
WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_LightAttack_Axe);
WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_HeavyAttack_Axe);

WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_Light_Axe);
WARRIOR_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Player_Ability_Attack_Heavy_Axe);
```

创建新的GA：GA_Hero_LightAttackMaster和GA_Hero_LightAttack_Axe（继承前一个master）。配置好标签，如下图所示，并且将Equip和UnEquip的Block也添加Attack的标签。

![20](/images/UE/WarriorProject/20.png)

创建新的IA_LightAttack_Axe，并添加到DA_InputConfig和IMC_Axe中。

在BP_HeroAxe的WeaponData中添加新的元素。

**能力实例化策略**

1. 在每一次执行时实例化 （但是每次都会被重置默认值）
2. 在每个角色实例化 （只会在第一次生成，之后只需要每次激活重用生成的实例）
3. 不实例化（必须完全在C++中实现）

在轻击这里使用第二种策略，也就是只实例化一次，之后复用。

### 连击

在GA_Hero_LightAttackMaster创建一个Map用来存储连击次数与动画的映射。

在每次EndAbility后设置一个定时器，如果在定时器的时间内没有进行下一次攻击，则会调用自定义事件——ResetAttackComboCount（将连击次数重置为1）。如果在规定时间内进行了连击，则会进入到激活能力的流程中清除定时器（也就不会执行自定义Event）。

根据连击次数调用蒙太奇动画，如果连击次数达到Map的容量，则清空连击次数（调用自定义Event），否则连击数++。

![21](/images/UE/WarriorProject/21.png)

之后创建连击动画的蒙太奇动画，并更改插槽为FullBody，在ABP_Hero中添加该插槽。之后再配置GA中的Map。就可以实现连击了。

![22](/images/UE/WarriorProject/22.png)

### 重击

重击与轻击相同逻辑，不再赘述。重击会有2次连击效果。

### JumpToFinsher

在本节中会创建一个蓝图函数库的C++类。

由于蓝图只能使用执行引脚，不能用bool返回节点控制流，因此使用`ExpandEnumAsExecs`让枚举展开成多个执行引脚。`DisplayName`指定蓝图节点上显示的名字（而不是函数名）。

```c++
UENUM()
enum class EWarriorConfirmType :uint8
{
	Yes,
	No
};

UCLASS()
class WARRIOR_API UWarriorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static UWarriorAbilitySystemComponent* NativeGetWarriorASCFromActor(AActor* InActor);
	
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);

	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	static void RemoveGameplayFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);
	// C++调用
	static bool NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck);
	// 蓝图调用
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary", meta = (DisplayName = "Does Actor Have Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck, EWarriorConfirmType& OutConfirmType);
};

UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
{
    check(InActor);
    return CastChecked<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

void UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
    UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
    if (!ASC->HasMatchingGameplayTag(TagToAdd))
    {
       ASC->AddLooseGameplayTag(TagToAdd); // 不会触发激活逻辑 添加临时状态
    }
}

void UWarriorFunctionLibrary::RemoveGameplayFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
    UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
    if (ASC->HasMatchingGameplayTag(TagToRemove))
    {
       ASC->RemoveLooseGameplayTag(TagToRemove);
    }
}

bool UWarriorFunctionLibrary::NativeDoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck)
{
    UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
    return ASC->HasMatchingGameplayTag(TagToCheck);
}

void UWarriorFunctionLibrary::BP_DoesActorHaveTag(AActor* InActor, FGameplayTag TagToCheck,
    EWarriorConfirmType& OutConfirmType)
{
    OutConfirmType = NativeDoesActorHaveTag(InActor, TagToCheck) ? EWarriorConfirmType::Yes : EWarriorConfirmType::No;
}
```

JumpToFinsher是一个新的临时Tag，目的是当轻击次数达到最后一次之前，如果使用重击，则直接跳跃到重击的最后一个连击效果。

例子：轻击有4个，重击有2个。当轻击到第三下时使用重击会直接跳到重击的第二个效果。

```c++
UE_DEFINE_GAMEPLAY_TAG(Player_Status_JumpToFinisher, "Player.Status.JumpToFinisher");
```

![23](/images/UE/WarriorProject/23.png)

在EndAbility后也需要调用RemoveGameplayFromActorIfFound。

![24](/images/UE/WarriorProject/24.png)