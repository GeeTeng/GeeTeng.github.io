1.序列帧动画
2.模型动画
顶点动画 数据量大 迭代缓慢 =》 骨骼动画

骨骼
通过改变骨骼的朝向和位置来生成动画
蒙皮
将mesh顶点附着到骨骼上（权重不一样）
每个顶点可以被多个骨骼控制

资产类型
skeleton骨架资源关联动画数据
skeletalmesh骨骼网格体 主要的渲染资源
physicsasset 物理资产 关联骨骼和物理世界的交互



eventGraph

AnimGraph



shooting 要在mesh space 而不是localspace 因为射击时要向准星方向 而不是人物朝向

装弹要在localspace 而不是mesh space  因为角色移动的时候朝向要向移动的方向装弹



blend space

- aim offset



Montage 依赖animsequence

根骨动画需要客户端和服务器同时播，如果服务器不播动画或者时间差太多 客户端会被拉扯





动画混合节点

ApplyAdditive

ApplyMeshSpaceAdditive

Blend

BlendBoneByChannel

BlendMulti

BlendPosesByBool /byint

layeredBlendPerBone

MakeDynamicAdditive





IK

TWO BONE IK

FABRIK





adobe mixamo



课后作业 实现敌人的基础移动动画蓝图 并尽可能优化表现

实现敌人的死亡动画 并尽可能优化表现 （多方向死亡动画）

实现近战击败敌人的功能和动画表现（选做）