---
title: "UMG"
date: 2024-12-13
tags: [UE5]
description: "作业：实现登录注册界面，密码加密，设置界面（按键修改、语言设置、分辨率更改）"
showDate: true
math: true
chordsheet: true
---



可直接跳转到文末查看最终效果图

---

#### 创建主页面

新建HUD类-MainUIHUD和GameMode-MainMenuGameMode，并新建BP_UIGameMode蓝图类。

```c++
// 将该HUD绑定到该GameMode上
AMainMenuGameMode::AMainMenuGameMode() {
	HUDClass = AMainUIHUD::StaticClass();
}
```

在MainUIHUD中加载主界面，新建UserWidget类MainUIHUD和继承其的蓝图（往后每一个界面都是这样的创建方式）

```c++
class UMainUserWidget;
protected:
	UPROPERTY()
	UMainUserWidget* MainUserWidget;
```

```c++
#include "MenuLevel/MainUserWidget.h"
void AMainUIHUD::BeginPlay()
{
	Super::BeginPlay();
	// 显示UI 加载蓝图类
	TSubclassOf<UMainUserWidget> WidgetClass = LoadClass<UMainUserWidget>(nullptr,TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/FPSGame/UI/UMG/UMG_MainUI.UMG_MainUI_C'"));
	if (WidgetClass)
	{
		// CreatWidget()
		MainUserWidget = CreateWidget<UMainUserWidget>(GetOwningPlayerController(), WidgetClass);
		if (MainUserWidget)
		{
			MainUserWidget->AddToViewport();
		}
	}
    // 显示鼠标
	GetOwningPlayerController()->bShowMouseCursor = true;
}
```

创建主页面（图片文字为后加上的）

![UMG_MainUI](/images/UMG/UMG_MainUI.png)

MainUserWidget

```c++
protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidgetAnim), Transient)
	UWidgetAnimation* MenuUIAni;

```

```c++
void UMainUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UMainUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
    // 制作了一个简单的动画 - 进入游戏按钮的透明度改变，并且按钮点击有音效
	PlayAnimation(MenuUIAni);
}
```

按钮的背景图是自己制作的 - 新建一个Material，提取UV坐标的R，[0, 1] * [1, 0] = > 中间最大值为0.25 两边为0的渐变色

![BtnMaterial](/images/UMG/BtnMaterial.png)

为了做设置面板，要新建OptionUserWidget，设置面板分为两个面板基础设置和按键设置

- **基础设置**用来更改语言、分辨率、音量等
- **按键设置**用来更改游戏操作按键

该面板有选择按钮、关闭按钮，加入一个重要的组件就是WidgetSwitcher能实现切换面板，而面板内容需要新建两个新的widget类分别为Widget_KeyOption按键设置面板内容和Widget_NormalOption基础设置面板内容。

BackgoundBlur设置模糊且Visible（防止点击到下一层）

![UMG_Option](/images/UMG/UMG_Option.png)

---

#### 切换面板

选择面板的蓝图，通过SetActiveWidgetIndex来切换面板

![UMG_Option01](/images/UMG/UMG_Option01.png)

---

#### 更改屏幕分辨率

Widget_NormalOption蓝图，新增了一个纯函数Get Window Mode（仅依赖传入的参数，不会改变外部状态）。

![Widget_NormalOption](/images/UMG/Widget_NormalOption.png)

新增了一个EWindowMode类型的输出命名为Mode

![GetWindowMode](/images/UMG/GetWindowMode.png)

---

#### 更改语言

打开Tool-Localization Dashboard

勾选想要翻译的部分，C++中增加的FText和项目中有的文字。

![LocalizationDashboard]/images/UMG/LocalizationDashboard.png)

向下滑，由于游戏的语言是中文，所以默认是中文语言，新增一个语言（English），然后点击Gather Text，此时会看到中文是100%的进度，而英文是0%进度，所以点击右侧第一个按钮Edit translations for this culture，去自己增加对应的英文翻译。

![GatherText](/images/UMG/GatherText.png)

翻译完之后点击CountWords和Compile Text就可以看到是百分百的状态。随着不断地增加字段，也需要更新这个操作。

![Widget_NormalOption01](/images/UMG/Widget_NormalOption01.png)

---

#### 背景音乐

下载一个比较贴切FPS游戏的背景音乐，然后加载到资产中，新建一个SoundCue、一个混音器和一个音效类。SoundCue中右键Wave Player，添加背景音乐。左侧面板设置音效类。音效类添加passive sound mix modifiers设置混音器。

在Wiget_NormalOption中设置Slider的值变化与音效相关，并且需要在主面板的蓝图中Event Construct中Play Sound 2D选择背景音乐，这样才会一进入游戏就会播放。

![SoundBGM](/images/UMG/SoundBGM.png)

至此基础设置面板结束，开始制作按键设置面板，预览图效果如下。

![UMG_Normal](/images/UMG/UMG_Normal.gif)

---

#### 更改操作键

新建C++类-UserWidget - KeyInfoWidget，并新建蓝图继承它，用来按键集合

新增数据表格Data Table 选择KeyInfoHeader，在这之前需要有一些InputAction。

默认跳跃空格 移动WASD 开火左键

![DT_KeyInfo](/images/UMG/DT_KeyInfo.png)

KeyInfoWidget文件

```c++
void UKeyInfoWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	// 绑定按键拾取器
	KeySelector->OnKeySelected.AddDynamic(this, &UKeyInfoWidget::OnKeySelected);
}

void UKeyInfoWidget::OnKeySelected(FInputChord SelectedKey)
{
	// 如果修改的键和当前键一样 就无需修改
	if (SelectedKey.Key == CurrentKey)
	{
		return;
	}
	// 检查是否有别的行为键有被用
	// 获取父容器
	if (UScrollBox* Box = Cast<UScrollBox>(GetParent()))
	{
		// 取容器中所有内容
		for (int32 i = 0; i < Box->GetChildrenCount(); ++i)
		{
			if (UKeyInfoWidget* KeyInfoWidget = Cast<UKeyInfoWidget>(Box->GetChildAt(i)))
			{
				if (KeyInfoWidget->CurrentKey == SelectedKey.Key)
				{
					// 将按键改回原来按键 不让它改
					KeySelector->SetSelectedKey(CurrentKey);
					return;
				}
			}
		}
	}

	if (KeyInfoHeader)
	{
		KeyInfoHeader->Key = SelectedKey.Key;
	}

}

// 初始化面板 获取datatable里面的字段
void UKeyInfoWidget::InitPanel(FKeyInfoHeader* OutKeyInfoHeader)
{
	if (OutKeyInfoHeader)
	{
		KeyDescribe->SetText(OutKeyInfoHeader->KeyDescribe);
		KeySelector->SetSelectedKey(OutKeyInfoHeader->Key);
		CurrentKey = OutKeyInfoHeader->Key;
		KeyInfoHeader = OutKeyInfoHeader;
	}
}

void UKeyInfoWidget::ResetKey()
{
	if (KeyInfoHeader)
	{
		// 如果没修改过
		if (CurrentKey == KeyInfoHeader->DefaultKey)
		{
			return;
		}
		KeyInfoHeader->Key = KeyInfoHeader->DefaultKey;
		CurrentKey = KeyInfoHeader->DefaultKey;
		KeySelector->SetSelectedKey(CurrentKey);
	}
}

```

KeyOptionWidget文件

```c++
void UKeyOptionWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	KeyMappingData = LoadObject<UDataTable>(this, TEXT("/Script/Engine.DataTable'/Game/FPSGame/Data/KeyInfoData.KeyInfoData'"));
	if (KeyMappingData)
	{
		TSubclassOf<UKeyInfoWidget> KeyInfoClass = LoadClass<UKeyInfoWidget>(nullptr, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/FPSGame/UI/UMG/Widget/Widget_KeyInfo.Widget_KeyInfo_C'"));

		TArray<FKeyInfoHeader*> KeyInfos;

		KeyMappingData->GetAllRows<FKeyInfoHeader>(TEXT("Load KeyMapper Error"), KeyInfos);
		for (auto Key : KeyInfos)
		{
			UKeyInfoWidget* KeyInfoWidget = CreateWidget<UKeyInfoWidget>(GetOwningPlayer(), KeyInfoClass);
			KeyInfoWidget->InitPanel(Key);
			KeyScollBox->AddChild(KeyInfoWidget);
		}
	}
}

// 重置所有按键
void UKeyOptionWidget::ResetAllKey()
{
    // 遍历全部KeyScollBox中的按键调用KeyInfo中的重置按键到DefaultKey的方法
	for (int32 i = 0; i < KeyScollBox->GetChildrenCount(); ++i)
	{
		if (UKeyInfoWidget* KeyInfoWidget = Cast<UKeyInfoWidget>(KeyScollBox->GetChildAt(i)))
		{
			KeyInfoWidget->ResetKey();
		}
	}
}

```

最终效果图如下

![KeyInfo](/images/UMG/KeyInfo.gif)

---

#### 登录

新建LoginUserWidget类（登陆注册界面）

在MainUIHUD中绑定该界面，由于要绑定的界面太多了，可以写一个模板函数去复用这个操作。

```c++
template<typename T>
void AMainUIHUD::MakeUserWidget(T*& Widget, const TCHAR* Path)
{
	if (!Widget)
	{
		TSubclassOf<T> WidgetClass = LoadClass<T>(nullptr, Path);
		Widget = CreateWidget<T>(GetOwningPlayerController(), WidgetClass);
	}
}
```

或者用宏

```c++
#define MAKEUSERWIDFETOBJ(UserWidgetClass, UserWidgetObj, Path) if (!UserWidgetObj)\
	{\
		TSubclassOf<UserWidgetClass> BPClass = LoadClass<UserWidgetClass>(nullptr, Path);\
		UserWidgetObj = CreateWidget<UserWidgetClass>(GetOwningPlayerController(), BPClass);\
	}
#undef MAKEUSERWIDFETOBJ
```

![UMG_Login](/images/UMG/UMG_Login.png)

```c++
protected:
	UFUNCTION(BlueprintCallable)
	void LoginGame();

protected:
	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* AccountTextBox;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* PasswordTextBox;

	// 是否保存账号密码
	UPROPERTY(meta = (BindWidget))
	UCheckBox* SaveCheckBox;
```

强制绑定需要修改蓝图中的名称 一一对应上。

![UMGLogin01](/images/UMG/UMGLogin01.png)

新建一个C++类继承SaveGame - LoginSaveGame

![SaveGame](/images/UMG/SaveGame.png)

```c++
// 需要存储的字段
UPROPERTY()
FString Account;
UPROPERTY()
FString Password;
```

LoginUserWidget文件

```c++
protected:
	UFUNCTION(BlueprintCallable)
	void LoginGame();

	virtual void NativeOnInitialized() override;

protected:
	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* AccountTextBox;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* PasswordTextBox;

	// 是否保存账号密码
	UPROPERTY(meta = (BindWidget))
	UCheckBox* SaveCheckBox;

	UPROPERTY()
	ULoginSaveGame* LoginSaveGame;
```



```c++
void ULoginUserWidget::LoginGame()
{
	// 如果勾选保存
	if (SaveCheckBox->IsChecked())
	{
		// 保存文档
		// 如果之前没存过 就创建一个新的存档对象
		if (!LoginSaveGame)
		{
			LoginSaveGame = Cast<ULoginSaveGame>(UGameplayStatics::CreateSaveGameObject(ULoginSaveGame::StaticClass()));
		}
		LoginSaveGame->Account = AccountTextBox->GetText().ToString();
		LoginSaveGame->Password = PasswordTextBox->GetText().ToString();
		// 存档 (存和读用一个名称)
		UGameplayStatics::SaveGameToSlot(LoginSaveGame, TEXT("LoginSaveGame"), 0);
	}
	else
	{
		// 如果没有勾选保存就删掉存档
		UGameplayStatics::DeleteGameInSlot(TEXT("LoginSaveGame"), 0);
	}
}

void ULoginUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	// 如果存在存档
	if (UGameplayStatics::DoesSaveGameExist(TEXT("LoginSaveGame"), 0))
	{
		// 加载存档 设置账号密码字段 以及 checkbox状态
		LoginSaveGame = Cast<ULoginSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("LoginSaveGame"), 0));
		if (LoginSaveGame)
		{
			AccountTextBox->SetText(FText::FromString(LoginSaveGame->Account));
			PasswordTextBox->SetText(FText::FromString(LoginSaveGame->Password));
			SaveCheckBox->SetCheckedState(ECheckBoxState::Checked);
		}
	}
}
```

---

#### 注册

新建C++类 - RegisterUserWidget 和 蓝图类 - UMG_Register

同样在MainUIHUD中绑定该页面

```C++
void AMainUIHUD::ShowRegisterUI()
{
	MakeUserWidget<URegisterUserWidget>(RegisterUserWidget, TEXT("/Script/UMGEditor.WidgetBlueprint'/Game/FPSGame/UI/UMG/UMG_Register.UMG_Register_C'"));
	// 如果存在 且 不在视口中 则让它出现在视口中
	if (RegisterUserWidget && !RegisterUserWidget->IsInViewport())
	{
		RegisterUserWidget->AddToViewport();
	}
}
```

这里做一个邮箱发送验证码的伪功能，而不是真正的发送邮件。

![UMG_Register](/images/UMG/UMG_Register.png)

该页面是通过登录页面中的注册账号按钮跳转过来，所以要在UMG_Login蓝图类中让它显示出来。

![UMG_Login02](/images/UMG/UMG_Login02.png)

RegisterUserWidget文件 - 发送邮件倒计时逻辑

```c++
protected:
	UFUNCTION(BlueprintCallable)
	// 点击发送 触发发送邮件
	void SendMail();
	// 回调函数
	void OnColdDownCB();
	// 更新按扭文字
	void UpdateButtonText();

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* SendButtonText;

	// 冷却中不允许点击
	UPROPERTY(meta = (BindWidget))
	UButton* SendButton;


	int32 ColdDownTime;

	// 定时器句柄
	FTimerHandle ColdDownTimeHandle;
```



```c++
// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuLevel/RegisterUserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void URegisterUserWidget::SendMail()
{
	// 发送时 就得停用按钮
	SendButton->SetIsEnabled(false);
	ColdDownTime = 5;

	// 启动定时器 执行一次就结束了
	// 获取世界对象定时器管理器FTimeManager 设置一个新的定时器
	// 第一个参数是一个定时器句柄FTimerHandle 用于唯一标识这个定时器
	GetWorld()->GetTimerManager().SetTimer(ColdDownTimeHandle, this, &URegisterUserWidget::OnColdDownCB, 1);
	UpdateButtonText();
}

// 定时器触发时的回调函数
void URegisterUserWidget::OnColdDownCB()
{
	if (--ColdDownTime <= 0)
	{
		// 如果计时结束 发送按钮激活
		SendButton->SetIsEnabled(true);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(ColdDownTimeHandle, this, &URegisterUserWidget::OnColdDownCB, 1);
	}
	UpdateButtonText();
}

// 设置发送按钮的文本 在计时时的变化
void URegisterUserWidget::UpdateButtonText()
{
	if (ColdDownTime <= 0)
	{
		SendButtonText->SetText(NSLOCTEXT("Register", "k1", "发送"));
	}
	else
	{
		// 格式化文本 将数字转换成字符
		SendButtonText->SetText(FText::Format(NSLOCTEXT("Register", "k1", "发送({0})"), FText::AsNumber(ColdDownTime)));
	}

}

```

注册账号逻辑

```c++
void URegisterUserWidget::RegisterUser()
{
	FString Account = AccountTextBox->GetText().ToString();
	// MD5加密
	FString Password = EncryptPassword(PasswordTextBox->GetText().ToString());

	ULoginSaveGame* LoadedSaveGame = Cast<ULoginSaveGame>(UGameplayStatics::LoadGameFromSlot(Account, 1));
	// 账号已存在 不能注册
	if (LoadedSaveGame)
	{
		ShowFeedBkFail();
	}
	// 账号不存在 可以注册
	else
	{
		ShowFeedBkSuccess();

		ULoginSaveGame* SaveGameInstance = Cast<ULoginSaveGame>(UGameplayStatics::CreateSaveGameObject(ULoginSaveGame::StaticClass()));
		if (SaveGameInstance)
		{
			SaveGameInstance->Account = Account;
			SaveGameInstance->Password = Password;

			UGameplayStatics::SaveGameToSlot(SaveGameInstance, Account, 1);
		}
	}
}

// 加密
FString  URegisterUserWidget::EncryptPassword(const FString& Password)
{
	// 转化成UTF-8字符数组
	FTCHARToUTF8 UTF8Password(*Password);
	const uint8* Data = reinterpret_cast<const uint8*>(UTF8Password.Get());
	int32 Length = UTF8Password.Length();

	// MD5哈希
	FMD5 MD5;
	MD5.Update(Data, Length);
	uint8 Digest[16];
	MD5.Final(Digest);

	FString EncryptedPassword;
	for (uint8 Byte : Digest)
	{
		EncryptedPassword += FString::Printf(TEXT("%02x"), Byte);
	}

	return EncryptedPassword;
}
```

修改登录逻辑

改为从savegame中获取已注册的账号。如果没有注册则返回“账号不存在 请注册”，如果注册了但是密码输入错误则返回“账号或密码错误”，如果账号存在且输入正确则登陆成功，同时也要加密登陆输入的密码。

```c++
void ULoginUserWidget::LoginGame()
{
	FString Account = AccountTextBox->GetText().ToString();
	FString Password = EncryptPassword(PasswordTextBox->GetText().ToString());
	ULoginSaveGame* LoadedSaveGame = Cast<ULoginSaveGame>(UGameplayStatics::LoadGameFromSlot(Account, 1));
	if (LoadedSaveGame)
	{
		// 如果登录账号已经注册过 且 密码匹配 则登陆成功
		if (LoadedSaveGame->Account == Account && LoadedSaveGame->Password == Password)
		{
			// 如果勾选保存
			if (SaveCheckBox->IsChecked())
			{
				// 保存文档
				// 如果之前没存过 就创建一个新的存档对象
				if (!LoginSaveGame)
				{
					LoginSaveGame = Cast<ULoginSaveGame>(UGameplayStatics::CreateSaveGameObject(ULoginSaveGame::StaticClass()));
				}
				LoginSaveGame->Account = AccountTextBox->GetText().ToString();
				LoginSaveGame->Password = PasswordTextBox->GetText().ToString();
				// 存档 (存和读用一个名称)
				UGameplayStatics::SaveGameToSlot(LoginSaveGame, TEXT("LoginSaveGame"), 0);
			}
			else
			{
				// 如果没有勾选保存就删掉存档
				UGameplayStatics::DeleteGameInSlot(TEXT("LoginSaveGame"), 0);
			}
			// 进入游戏 打开游戏关卡
			UGameplayStatics::OpenLevel(this, FName(TEXT("FirstPersonMap")));
		}
		// 否则账号密码错误
		else
		{
			ShowFeedBkError();
		}
	}
	// 没有账号 提示注册
	else
	{
		ShowFeedBkFail();
	}
}
```

可以看到存储下来的密码都是加密过的。

![RegisterSaveGame](/images/UMG/RegisterSaveGame.png)

#### 场景加载

下载了插件Async Loading Screen

然后在ProjectSetting中有一个AsyncLoadingScreen，在当中可以设置场景跳转的时候加载什么图片，修改颜色等。同时也可以设置开场动画。

我在网上随便找了一个CG动画作为游戏的开场动画。

最终效果图如下

![Login](/images/UMG/Final.gif)

---

关于射击操作面板，我还没有实现，想完善好玩法之后再增加这一部分内容。
