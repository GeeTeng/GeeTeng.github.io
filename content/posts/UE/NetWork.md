---
title: "UE网络"
date: 2024-12-23
tags: [UE5]
showDate: true
math: true
chordsheet: true
---



## 网络模式

*NM_Standalone*模式：服务器在本地计算机运行，不接受来自远程计算机的客户端，适合单人游戏或者本地多人游戏。没有服务器和客户端的区别，逻辑都在本地执行。

*NM_DedicatedServer*模式（专用服务器）：专属服务器没有本地玩家，用于托管在受信任服务器上的多人游戏，比如在线射击游戏。服务器不会有本地玩家ULocalPlayer、APlayerController等一些客户端专有的类，也会抛弃掉客户端独有的逻辑，为了减少性能开销。总而言之，就是服务器只处理多人游戏逻辑和同步，不参与游戏，节省资源。

*NM_ListenServer*模式（监听服务器）：本机又是服务器又是客户端，主客户端拥有全部服务器权限，其他客户端可以连接这个Listen Server。举例子：星露谷物语、Minecraft这种游戏的联机模式。

*NM_Client*模式（纯客户端）：非服务器模式，本地计算机是专属或监听服务器的客户端，不会允许服务器端逻辑。

|                    | Playable | Has Authority | Clients Can Join |
| ------------------ | -------- | ------------- | ---------------- |
| NM_Standalone      | ✅        | ✅             | ❌                |
| NM_DedicatedServer | ✅        | ✅             | ✅                |
| NM_ListenServer    | ❌        | ✅             | ✅                |
| NM_Client          | ✅        | ❌             | ❌                |



## 局域网联机（LAN）

HostLANGame函数代表本机加载这个路径的地图，然后作为监听服务器等待其他玩家加入。

JoinLANGame函数代表客户端连接到这个ip地址的服务器中作为客户端加入。

```c++
void AMultiplayerGameGameMode::HostLANGame()
{
	// ?listen代表游戏作为监听服务器（Listen Server） 这个游戏实例既是服务器又是客户端
	GetWorld()->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
}

void AMultiplayerGameGameMode::JoinLANGame()
{
	APlayerController *PC = GetGameInstance()->GetFirstLocalPlayerController();
	if (PC)
	{
		PC->ClientTravel("192.168.31.130", TRAVEL_Absolute);
	}
}
```

在蓝图中调用该函数，然后将项目打包后运行在另一个局域网电脑中，就可以实现局域网联机了。



## Actor复制（Replication）机制

*创建和销毁*：服务器创建的Actor，客户端也能看到，并且销毁时同步消除。这一部分UE已经帮我们实现了，不需要我们做什么。

*移动复制*：同步Actor的位置、旋转、速度等移动信息，保持物理表现一致。只需要在蓝图上勾选Replication中的选项。

*变量复制*：同步服务器上的变量（比如血量、分数、状态）到客户端。

RPCs远程函数调用：1. Server（客户端向服务器请求，比如攻击） 2. Client（服务器通知客户端，比如受伤） 3. Multicast（广播，比如爆炸）



## 网络角色（Network Role）

`AActor::GetLocalRole`函数用来返回角色控制权限，返回值有：

- `ROLE_None`：无效，不参与网络同步。
- `ROLE_SimulatedProxy`：模拟代理，只做视觉表现不控制逻辑，比如客户端上看到的其他玩家。
- `ROLE_AutonomousProxy`：自主代理，拥有本地控制权，比如客户端自己的Pawn。
- `ROLE_Authority`：拥有最高控制权，服务器对所有的Actor来说都是Authority。

通过`HasAuthority()`来测试是否当前运行的是客户端或者服务端。

```c++
void AMyBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority()) // 相当于HasAuthority() == ROLE_Authority
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Server"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("Client"));
	}
}
```



## 变量同步

```c++
UPROPERTY(Replicated, BlueprintReadWrite)
float ReplicatedVar;
// 是UE中用来指定哪些变量要进行网络同步的函数，从AActor类继承来的。
void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
```

`DOREPLIFETIME`用于登记需要同步的变量

```c++
void AMyBox::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 宏函数 用来把变量注册为需要网络同步
    DOREPLIFETIME(AMyBox, ReplicatedVar);
}
```



## RepNodifies（Replicated Using 机制）

当一个 `UPROPERTY` 变量被网络复制（Replicated）并且加了 `ReplicatedUsing = OnRep_XXX`，那么 **当它的值在客户端因为复制而发生变化时**，Unreal 就会自动调用你指定的 `OnRep_XXX` 函数。

```c++
UPROPERTY(ReplicatedUsing = OnRep_ReplicatedVar, BlueprintReadWrite)
float ReplicatedVar;

UFUNCTION(BlueprintCallable)
void OnRep_ReplicatedVar();
```

```c++
void AMyBox::BeginPlay()
{
	Super::BeginPlay();
	SetReplicates(true);
	SetReplicateMovement(true);
	ReplicatedVar = 100.0f;
	if (HasAuthority())
	{
		// 如果是服务器 两秒自动调用DecreseReplicatedVar这个函数（只执行一次）
		GetWorld()->GetTimerManager().SetTimer(TestTimer, this, &AMyBox::DecreseReplicatedVar, 2.0f, false);
	}
}
void AMyBox::OnRep_ReplicatedVar()
{
	if (HasAuthority())
	{
		FVector NewLocation = GetActorLocation() + FVector(0.0f, 200.0f, 0.0f);
		SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Server:: OnReplicatedVar"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("Client %d: OnReplicatedVar"),GPlayInEditorID));
	}
}
void AMyBox::DecreseReplicatedVar()
{
	if (HasAuthority())
	{	// 每次被调用值都-1
		ReplicatedVar -= 1.0f;
        // 手动调用
		OnRep_ReplicatedVar();
        // 如果值大于1，则每两秒继续调用自己（调用自己，自己继续调用自己 实现循环）
		if (ReplicatedVar > 1.0f)
		{
			GetWorld()->GetTimerManager().SetTimer(TestTimer, this, &AMyBox::DecreseReplicatedVar, 2.0f, false);
		}

	}
}
```



## Server RPCs

由于客户端不能随便影响服务器，所以客户端只能请求服务器做某事（用Server RPC），客户端不能直接控制服务器的逻辑和变量，而RPC是一种**受控通信机制**。

**RPC可靠性**——不要过度的使用可靠RPC。

```c++
// 可靠RPC
UFUNCTION(NetMulticast, Reliable)
void MulticastRPCFunction(float Arg);
// 用于非关键任务 比如移动 因为每帧都在传输 可以有一些丢失
UFUNCTION(Server, Unreliable)
void ServerRPCFunction(float Arg);
```

*使用方法*：

```c++
UFUNCTION(Server, Reliable, BlueprintCallable)
void ServerRPCFunction();
```

### RPC_Implementation(服务器逻辑)

```c++
void AMultiplayerGameCharacter::ServerRPCFunction_Implementation()
{
	if (HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Server: ServerRPCFunction_Implementation"));
	}
}
```

`ServerRPCFunction`是客户端调用的函数接口

`ServerRPCFunction_Implementation()` 是 **服务端真正执行的函数内容**。

- 服务端接收到请求后，才会执行这个函数体
- 所以逻辑写在 `_Implementation()` 中，只有服务端会运行它

使用RPC请求生成球的例子：

```c++
UPROPERTY(EditAnywhere)
UStaticMesh* SphereMesh;
```

```c++
void AMultiplayerGameCharacter::ServerRPCFunction_Implementation()
{
	if (HasAuthority())
	{
		if(!SphereMesh)
		{
			return;
		}
		// 生成一个静态网格体StaticMeshActor
		AStaticMeshActor *StaticMeshActor = GetWorld()->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass());
		if (StaticMeshActor)
		{
			// 设置StaticMeshActor可复制 可移动
			StaticMeshActor->SetReplicates(true);
			StaticMeshActor->SetReplicateMovement(true);
			StaticMeshActor->SetMobility(EComponentMobility::Movable);
			// 设置生成位置是玩家朝向前
			FVector SpawnLocation = GetActorLocation() + GetActorRotation().Vector() * 100.f + GetActorUpVector() * 50.0f;
			StaticMeshActor->SetActorLocation(SpawnLocation);
			UStaticMeshComponent *StaticMeshComponent = StaticMeshActor->GetStaticMeshComponent();
			if (StaticMeshComponent)
			{
				StaticMeshComponent->SetIsReplicated(true);
				StaticMeshComponent->SetSimulatePhysics(true);
				if (SphereMesh)
				{
					StaticMeshComponent->SetStaticMesh(SphereMesh);
				}
			}
		}
	}
}
```

在蓝图中客户端可以调用ServerRPCFunction来让服务器执行它的逻辑。

### RPC_Validation（验证数据）

当你定义了一个`ServerRPCFunction`的函数时，UE会自动为你生成两个对应的函数定义，分别是`ServerRPCFunction_Implementation`，这是你真正**在服务器上执行逻辑** 和`ServerRPCFunctionArgs_Validate`，这是一个可选的函数，用来在客户端调用服务器 RPC 时，进行**参数验证**。只有当返回 `true` 时，才会继续执行 `_Implementation`。

```c++
	UFUNCTION(Server, Reliable,WithValidation, BlueprintCallable)
	void ServerRPCFunctionArgs(int Args);
```

```c++
void AMultiplayerGameCharacter::ServerRPCFunctionArgs_Implementation(int Args)
{
	if (HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, FString::Printf(TEXT("Args: %d"), Args));
	}
}
// 如果返回true才会执行服务端的逻辑，返回false就直接退回游戏了
bool AMultiplayerGameCharacter::ServerRPCFunctionArgs_Validate(int Args)
{
	if (Args >= 0 && Args <= 100)
	{
		return true;
	}
	return false;
}
```

### RPCMulticast(多播)

服务端客户端都会执行这个逻辑

```c++
UFUNCTION(NetMulticast, Reliable)
void MulticastRPCExplode();
```

```c++
void AMyBox::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(TestTimer, this, &AMyBox::MulticastRPCExplode, 2.0f, false);
	}
}
void AMyBox::MulticastRPCExplode_Implementation()
{
	if (HasAuthority())
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("Server: MulticastRPCExplode_Implementation"));
		// 多播RPC只应该从服务器调用 然后将在所有客户端和服务器上执行
		GetWorld()->GetTimerManager().SetTimer(TestTimer, this, &AMyBox::MulticastRPCExplode, 2.0f, false);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("Client: MulticastRPCExplode_Implementation"));
	}
}
```

使用多播生成爆炸效果例子：

在多播RPC中插入如下

```c++
// 如果不是专用服务器的话就生成粒子特效 因为专用服务器不需要图形化
if (!IsRunningDedicatedServer())
{
    // 创建粒子特效 没有旋转 自动释放
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
}
```

## Ownership（所有权）

在多人游戏中，每个客户端只能操纵自己的Actor，不能操控别人的Actor，因此UE使用Ownership来判断这个客户端**是否被允许**向服务器请求改变这个Actor的状态。

Client Connection --> APlayerController --> APawn --> AItem

```c++
FActorSpawnParameters SpawnParameters;
// 将当前对象（this）设为将要生成的 Actor 的 Owner（所有者）
SpawnParameters.Owner = this;
```

![01](/images/UE/Network/01.png)



## ClientRPCs

如果玩家在游戏中受伤，玩家客户端会显示红屏效果，但是不希望其他玩家去看到这个红屏效果。

```c++
UFUNCTION(Client, Reliable, BlueprintCallable)
void ClientRPCFunction();
```

他只能在所在的客户端上调用，其他客户端无法看到。

总结：

**尽可能少的使用RPC，用RepNodifies来代替**，因为RPC可能会在网络上产生大量额外的流量，尤其是多播RPC。

**如果使用RPC，尽可能不可靠**。因为如果每一帧的tick或者玩家疯狂的发送rpc，会迅速导致网络队列溢出。

避免调用没有意义的RPC，比如从客户端调用客户端RPC，或者在服务器调用一个客户端RPC，但是Actor没有在客户端或者是在服务器上的。或者在客户端调用多播RPC和在服务器调用服务器RPC，都是无用的。

只需要使用服务器RPC来使服务器和客户端通信。

比如说发射子弹这个事情所有人都要看到就是多播，受伤就是服务器发给客户端告诉你受伤了，攻击别人就是客户端请求服务器如果合法就执行伤害生成子弹。
