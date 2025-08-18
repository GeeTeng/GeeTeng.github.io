---
title: "UI制作体力条、选择房间"
date: 2025-07-04
tags: [实习笔记]
description: "选择服务器、显示房间号、体力条"
showDate: true
math: true
chordsheet: true
---





## 选择房间进入不同服务器

创建一个通用的UMG_CheckBox，其中包含显示房间号的TextBlock和CheckBox。

在UIDefine.ts中声明一个Map用来存储房间号和端口的映射关系 这个文件的数据全局都可以访问到。

```typescript
export const RoomToPortMap: Map<string, string> = new Map<string, string>([
    ["房间1", "7770"],
    ["房间2", "7771"],
    ["房间3", "7772"],
]);
```

在LobbyMainSystem.ts（进入游戏前的大厅）的OnCreated中调用创建选择房间的CheckBox控件

声明一个Map用来存储CheckBox和房间号的映射，可以通过CheckBox对应的房间号查找到RoomToPortMap的房间对应的端口号。

```typescript
CheckBoxToRoom: Map<UE.CheckBox, string> = new Map();
OnCreated(InUEWidget: UE.BJ_UserWidgetBase): void {
    super.OnCreated(InUEWidget);
    // ...
    const Container = this.UEWidget.CheckBoxContainer as UE.VerticalBox;
    this.CreateRoomCheckBox(Container);
}
    protected UncheckOtherCheckBoxes(SelectCheckBox: UE.CheckBox): void {
    for (let [CheckBox] of this.CheckBoxToRoom) {
        if (CheckBox !== SelectCheckBox) {
            CheckBox.SetIsChecked(false);
        }
    }
}
protected GetSelectedRoomPort(): string | null {
    for (let [Checkbox, Room] of this.CheckBoxToRoom) {
        if (Checkbox.IsChecked()) {
            return RoomToPortMap.get(Room) || null;
        }
    }
    return null;
}
protected CreateRoomCheckBox(Container: UE.VerticalBox) {
    this.CheckBoxToRoom.clear();
    const CheckBoxClass = UE.Class.Load("/Game/UI/UMG/Lobby/UMG_CheckBox.UMG_CheckBox_C");
    let isFirst = true;
    RoomToPortMap.forEach((Port, Room) => {
        const CheckBox = UIUtil.CreateWidget(CheckBoxClass) as UE.UMG_CheckBox_C;
        CheckBox.TextBlock_Room.SetText(Room);
        // 默认第一个CheckBox选中
        if (isFirst) {
            isFirst = false;
            CheckBox.CheckBox_Room.SetIsChecked(true);
        } else {
            CheckBox.CheckBox_Room.SetIsChecked(false);
        }
        CheckBox.CheckBox_Room.OnCheckStateChanged.Add((isChecked: boolean) => {
            if (isChecked) {
                this.UncheckOtherCheckBoxes(CheckBox.CheckBox_Room);
            } else {
                let hasChecked = false;
                for (let [cb] of this.CheckBoxToRoom) {
                    if (cb.IsChecked()) {
                        hasChecked = true;
                        break;
                    }
                }
                if (!hasChecked) {
                    CheckBox.CheckBox_Room.SetIsChecked(true);
                }
            }
        });
        Container.AddChildToVerticalBox(CheckBox);
        this.CheckBoxToRoom.set(CheckBox.CheckBox_Room, Room);
    });
}

```



## 设置面板中显示玩家所在房间

在Debug面板的UMG中创建一个新的TextBlock用于显示所在房间

GameDebugSystem.ts中的OnCreated函数会调用当前文件的Init函数，Init函数中添加初始化房间号的函数InitCurrentRoomText

其中调用玩家的GetDSPort方法，用于获取当前连接的网络端口号，再从RoomToPortMap中查找对应的房间号。

```typescript
InitCurrentRoomText = () => {
    let Player = this.UEWidget?.GetOwningPlayer() as UE.PMS_ZombiePlayerController;
    if (Player !== undefined) {
        let ReturnPort = String(Player.GetDSPort());
        if (ReturnPort !== undefined) {
            for (let [Room, Port] of RoomToPortMap) {
                if (ReturnPort === Port) {
                    this.UEWidget.TextBlock_Room.SetText(Room);
                    break;
                } else {
                    this.UEWidget.TextBlock_Room.SetText("未加入房间");
                }
            }
        }
    }
};
```

## 体力条显示

在UMG_HUDInfo中新增一个ProgressBar叫做ProgressBar_Energy用于显示体力的进度条，同时创建两个SlateBrush变量分别是Brush_LowEnergy和Brush_NormalEnergy。

Brush_LowEnergy是当体力值低于百分之30时，赋值给this.UEWidget!.ProgressBar_Energy.WidgetStyle.BackgroundImage，可以将进度条的背景更改为红色。Brush_NormalEnergy是体力值高于百分之30时的正常背景颜色。

在PMS_CharacterBase.h中将BindAttributesDelegate和UnBindAttributesDelegate更改为虚函数可被重写。PMS_CharacterBase是怪物AI和玩家的基类，由于玩家有Energy属性怪物没有，所以要在PMS_HeroCharacter中绑定体力改变的委托。

在PMS_GameDelegate_WorldSubsystem.h中声明体力变化的委托

```c++
UPROPERTY(BlueprintAssignable)
FPawnFloatValueChange OnLocalPlayerCurEnergyChange;
UPROPERTY(BlueprintAssignable)
FPawnFloatValueChange OnLocalPlayerMaxEnergyChange;
FPawnFloatValueChange& GetOnLocalPlayerCurEnergyChange() { return OnLocalPlayerCurEnergyChange; }
FPawnFloatValueChange& GetOnLocalPlayerMaxEnergyChange() { return OnLocalPlayerMaxEnergyChange; }
```

```c++
public:
	virtual void NotifyControllerChanged() override;	
	UFUNCTION(BlueprintCallable)
	float GetEnergy();	
	UFUNCTION(BlueprintCallable)
	float GetMaxEnergy();
protected:
	virtual void InitDefaultGameAbilities() override;	
	virtual void BindAttributesDelegate() override;
	virtual void UnBindAttributesDelegate() override;
private:
	void EnergyChanged(const FOnAttributeChangeData& Data);
	void MaxEnergyChanged(const FOnAttributeChangeData& Data);
public:
	UPROPERTY(BlueprintAssignable, Category = Character)
	FOnCharacterFloatValueChange OnEnergyChanged;	
	UPROPERTY(BlueprintAssignable, Category = Character)
	FOnCharacterFloatValueChange OnMaxEnergyChanged;
protected:
	FDelegateHandle EnergyChangedDelegateHandle;
	FDelegateHandle MaxEnergyChangedDelegateHandle;
```



然后在TS脚本的GameHUDInfoSystem.ts中声明TargetEnergy代表当前真实的体力值，DisplayEnergy代表UI表现的体力值，UI表现的体力值要去追赶真实体力值。设置这两个变量是为了做进度条平滑的移动，防止一下子减少一截。

因为体力值可能不是100，可能是任何数字，所以要去获取角色实际的体力值。

```typescript
CurEnergy: number = 100;
EnergyBlinkTimer: number = 0; // 体力值为0的闪烁定时器
isEnergyBlinkState: boolean = false; // 切换闪烁时的背景状态
private TargetEnergy: number = 100; 
private DisplayEnergy: number = 100; // 之后会将实际体力值赋值给它们

OnCreated(InUEWidget: UE.BJ_UserWidgetBase): void {
    super.OnCreated(InUEWidget);
    this.UEWidget!.SetVisibility(UE.ESlateVisibility.Hidden);
    this.ReadyToNotifyStart();
    let localPlayer = this.UEWidget!.GetOwningPlayer() as UE.PMS_ZombiePlayerController;
    let SelfCharacter = localPlayer.Pawn as UE.PMS_ZombiePlayerCharacter;
    if (SelfCharacter !== undefined) {
        this.CurEnergy = SelfCharacter.GetEnergy();
        this.MaxEnergy = SelfCharacter.GetMaxEnergy();
        const EnergyPercent: number = Math.min(Math.max(0, this.CurEnergy / this.MaxEnergy), 1);
        this.SetProgressBarEnergyPercent(EnergyPercent);
    }
}
```

在TSTick中每帧计算当前要增加或减少的步长CurStep。

其中RecoverEnergySpeed和CostEnergySpeed是需要在UMG中配置的消耗、恢复能量速度。

比如说0.2秒减少1点能量，速度是0.2。让EnergyStep去除以速度，是为了保证能量变化的幅度不会因为速率不同而出现问题。

出现问题的原因是因为let CurStep = this.EnergyStep * InDeltaTime;

这句代码表示，InDeltaTime是1/60秒，当前CurStep是一秒钟变化的步长。所以一定要除以速率。

```typescript
TSTick = (MyGeometry: UE.Geometry, InDeltaTime: number) => {
    // ...
    let CurStep = this.EnergyStep * InDeltaTime;
    if (Math.sign(this.EnergyStep) > 0 && this.DisplayEnergy > this.TargetEnergy) {
        this.DisplayEnergy = this.TargetEnergy;
        if (this.EnergyStep !== 0) {
            this.EnergyStep = 0;
        }
    } else if (Math.sign(this.EnergyStep) < 0 && this.DisplayEnergy < this.TargetEnergy) {
        this.DisplayEnergy = this.TargetEnergy;
        if (this.EnergyStep !== 0) {
            this.EnergyStep = 0;
        }
    } else {
        this.DisplayEnergy += CurStep;
    }
    let percent = Math.min(Math.max(0, this.DisplayEnergy / this.MaxEnergy), 1);
    this.SetProgressBarEnergyPercent(percent);
};

// 绑定C++中的委托 体力值改变时修改TargetEnergy值
@D.UEGameDelegateBind("OnLocalPlayerCurEnergyChange")
OnCurEnergyChange(Pawn: $Nullable<UE.Pawn>, OldValue: number, NewValue: number) {
    if (Pawn === null) {
        return;
    }
    if (!Pawn.IsLocallyControlled()) {
        return;
    }
    if (this.TargetEnergy !== NewValue) {
        this.EnergyStep = NewValue - this.TargetEnergy;
        if (this.EnergyStep > 0 && this.UEWidget!.RecoverEnergySpeed !== 0) {
            this.EnergyStep = this.EnergyStep / this.UEWidget!.RecoverEnergySpeed;
        } else if (this.EnergyStep < 0 && this.UEWidget!.CostEnergySpeed !== 0) {
            this.EnergyStep = this.EnergyStep / this.UEWidget!.CostEnergySpeed;
        }
        this.TargetEnergy = NewValue;
    }
}
SetProgressBarEnergyPercent(Percent: number) {
    if (Percent === 1.0) {
        this.UEWidget!.ProgressBar_Energy.SetVisibility(UE.ESlateVisibility.Hidden);
    } else {
        this.UEWidget!.ProgressBar_Energy.SetVisibility(UE.ESlateVisibility.Visible);
        if (Percent === 0) {
            if (!this.EnergyBlinkTimer) {
                this.EnergyBlinkTimer = Timer.StartTimer(
                    () => {
                        if (!this.isEnergyBlinkState) {
                            this.UEWidget!.ProgressBar_Energy.WidgetStyle.BackgroundImage =
                                this.UEWidget!.Brush_NormalEnergy;
                        } else {
                            this.UEWidget!.ProgressBar_Energy.WidgetStyle.BackgroundImage =
                                this.UEWidget!.Brush_LowEnergy;
                        }
                        this.isEnergyBlinkState = !this.isEnergyBlinkState;
                    },
                    0.3,
                    true
                );
            }
        } else {
            if (this.EnergyBlinkTimer) {
                Timer.StopTimer(this.EnergyBlinkTimer);
                this.EnergyBlinkTimer = 0;
            }
            if (Percent < 0.3) {
                this.UEWidget!.ProgressBar_Energy.WidgetStyle.BackgroundImage = this.UEWidget!.Brush_LowEnergy;
                this.UEWidget!.ProgressBar_Energy.WidgetStyle.FillImage = this.UEWidget!.FillBrush_LowEnergy;
            } else {
                this.UEWidget!.ProgressBar_Energy.WidgetStyle.BackgroundImage = this.UEWidget!.Brush_NormalEnergy;
                this.UEWidget!.ProgressBar_Energy.WidgetStyle.FillImage = this.UEWidget!.FillBrush_NormalEnergy;
            }
        }
    }
    this.UEWidget!.ProgressBar_Energy.SetPercent(Percent);
}

```

