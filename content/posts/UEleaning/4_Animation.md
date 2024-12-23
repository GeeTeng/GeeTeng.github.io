#### 序列帧动画 

无需计算，直接显示图片逻辑简单。**缺点**：占用空间大，不适合复杂动画。

#### 模型动画

- **顶点动画** 

  直接存储每个顶点的动画信息 。**缺点**：数据量大，迭代缓慢。

  例子：水流、捏脸Morph Target Animation

- **骨骼动画（Skinned Animation）**
  通过改变骨骼的朝向和位置来生成动画
  **蒙皮**：将mesh顶点附着到骨骼上（不同顶点权重不一样每个顶点可以被多个骨骼控制）

  每个骨骼有一个变换矩阵，最终网格顶点的位置是多个骨骼变换矩阵的加权和。

  **蒙皮矩阵的计算（Skinning Matrix）:**
  $$
  M_{skin} =M_{bone}×M_{inverse}
  $$
  M_inverse是你绑定矩阵，用于骨骼从骨骼空间转换到模型空间的变换。

  **Quaternion NLerp**用于物体旋转的平滑过渡。

  NLerp和SLerp（球面线性插值）

  **NLerp** 比 **Slerp** 更简单，计算开销较小，因为它只进行线性插值，不涉及球面插值的复杂计算。

  当旋转角度较大时，使用Slerp。

- **基于物理的动画（Physics-based Animation）**

  - 布娃娃系统（Ragdoll）

  - 布料和流体模拟（Cloth and Fluid simulation）

  - 反向动力学（Inverse Kinematics(IK)）

---

#### （关节）Joint

![jointandbone](/images/Animation/jointandbone.png)

两个Joint定义一个骨骼

**Root Joint**在人的双腿之间

**Pelvis Joint**（骨盆关节） 蹲起

![bindpose](/images/Animation/bindpose.png)

**Joint Pose**有9个DOF（自由度） 放缩、平移、旋转

---

#### 万向节锁（Gimbal Lock）

在三维空间中使用欧拉角表示旋转时，当旋转顺序导致某两个旋转轴对齐时，会丧失一个自由度，导致旋转不再是完全独立的，从而限制了旋转的自由度。这个现象被称为 **万向节锁**。

如：大疆无人机、pocket3的镜头防抖动，就是使用陀螺仪和万向节锁来实现的。

引出**四元数**解决万向节锁

---

#### **骨骼动画资产类型**

- **Skeleton**

​	骨架资源，存储骨骼的层级、动画数据关联。

​	多个模型可以共享同一个骨架。

- **SkeletalMesh**

​	骨骼网格体，用于渲染。

​	包含蒙皮信息，顶点如何受骨骼影响。

- **PhysicsAsset**

​	定义骨骼与物理系统的交互。

​	用于 ragdoll、碰撞检测等。

---

#### 动画图表

- **EventGraph**

​	动画蓝图中逻辑部分，处理输入事件、动画状态等

- **AnimGraph**

​	动画蓝图中动画部分，定义动画的混合逻辑、运算节点。

---

#### 动画逻辑中的空间

##### **Mesh Space vs Local Space**

- **Mesh Space**

  相对于整个模型的空间。

  使用场景：射击，确保角色朝向不同方向时，枪口始终指向准星。

- **Local Space**

  相对于骨骼或父节点的局部空间。

  使用场景：装弹，动作只依赖局部骨骼的朝向，不受整体移动影响。

![spaces](/images/Animation/spaces.png)

---

#### 动画高级控制

- ##### **Blend Space**

​	定义不同动画之间的平滑混合（如跑步、行走、站立）。

​	**Aim Offset**：通过 Blend Space 实现瞄准时的身体姿态调整。

- ##### **Montage**

​	依赖具体的 **Anim Sequence**。

​	用于处理复杂的、可中断的动作序列（如攻击、技能）。

​	根骨动画：需要客户端和服务器同步播放。若服务器不播动画或时间差太多，客户端会被拉扯（如技能瞬移）。

---

#### **反向动力学（IK）**

动态调整骨骼的姿态，使角色的特定部分（如手、脚）精确触碰目标位置。

##### **IK 节点类型**

- **Two Bone IK**：

​	控制具有两个骨骼的链条（如大腿、膝盖、小腿）。

​	使用场景：脚精确接触地面，膝盖自然弯曲。

- **FABRIK（Forward And Backward Reaching Inverse Kinematics）**：

​	一种通用的 IK 算法，适合任意长的骨骼链。





adobe mixamo



课后作业 

实现敌人的基础移动动画蓝图 并尽可能优化表现

实现敌人的死亡动画 并尽可能优化表现 （多方向死亡动画）

实现近战击败敌人的功能和动画表现（选做）