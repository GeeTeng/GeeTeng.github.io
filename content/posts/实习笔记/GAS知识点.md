---
title: "GAS知识点笔记"
date: 2025-06-27
tags: [实习笔记]
description: "GAS知识点"
showDate: true
math: true
chordsheet: true
---



# GAS部分知识点

ASC（Ability System Component）主要组件（谁放技能）

AS（Attribute Set）角色身上可以用float表示的属性，如生命值、体力值等（技能改变的属性）

GA（Gameplay Abilities）角色的技能，包括攻击、疾跑、施法、翻滚、使用道具等，但不包括基础移动和UI（技能逻辑）

GE（Gameplay Effects）用于修改属性，如增加移动速度等（技能的效果）

GC（Gameplay Cues）播放特效、音效等（技能的视效）

GameplayTag（技能涉及的条件）

GameplayTask（技能长时行动）

GameplayEvent（技能消息事件）

## ASC

在构造函数中创建ASC组件，其中ReplicationMode三种模式：Full、Mixed、Minimal

比如大招GE会被同步给队友，但是普通技能的冷却时间不会被同步。但是AI设置为Minimal，因为不需要别人知道他的技能冷却时间等。

其中Mixed模式必须要设置拥有者为控制器

![img](/images/实习笔记/03.png)

```c++
PMSAbilitySystemComponent = CreateDefaultSubobject<UPMS_AbilitySystemComponent>(CharacterASCName);
PMSAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

PMSAttributeSetBase = CreateDefaultSubobject<UPMS_AttributeSetBase>(CharacterAttributeSetName);
```



## Attribute

有两个值，BaseValue和CurrentValue，其中CurrentValue是BaseValue加上GE给的临时修改值后得到的，比如BaseValue生命值100，捡到药水瓶CurrentValue = 150

在PreAttributeChange中处理对CurrentValue的修改以及取值范围问题, 在PostGameplayEffectExecute中处理GE对BaseValue的修改

Meta Attribute是临时的、可被任意GE修改、不可同步的，将于任意Attribute交互的临时属性。 比如伤害值作为Meta Attribute占位符, 而不是使用GE直接修改生命值Attribute, 使用这种方法, 伤害值就可以在GameplayEffectExecutionCalculation中由buff和debuff修改, 并且可以在AttributeSet中进一步操作。

Set和Init的区别是在于Set只能设置basevalue， 而init可以设置currentvalue和basevalue

在PlayerCharacterBase的BeginPlay中，绑定委托、初始化属性、初始化GA

```c++
void APMS_CharacterBase::BeginPlay()
{
	Super::BeginPlay();
	BindAttributesDelegate();
	PMSAttributeSetBase->InitAttributes();
	FTimerHandle hld;
	GetWorld()->GetTimerManager().SetTimer(hld, this, &APMS_CharacterBase::InitAttributes, 0.1f);
}
void APMS_CharacterBase::BindAttributesDelegate()
{
	if (IsValid(PMSAbilitySystemComponent) && IsValid(PMSAttributeSetBase))
	{
		HealthChangedDelegateHandle = PMSAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PMSAttributeSetBase->GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);
        // ...
	}
}
void UPMS_AttributeSetBase::InitAttributes()
{
	InitHealth(100.f);
    //...
}
void APMS_CharacterBase::InitAttributes()
{
	if (HasAuthority() && InitAttributesGA)
	{
		PMSAbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(InitAttributesGA, 3, -1, this));
	}
	if (HasAuthority() && CharacterDieBaseGA)
	{
		PMSAbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(CharacterDieBaseGA, 1, -1, this));
	}
}
```

FGameplayAbilityActorInfo 是一个结构体，其中包含了例如OwnerActor（playerstate / pawn等）、AvatarActor（character）、ASC等。

FGameplayAbilitySpec 也是一个结构体，包含了能力、等级等



 