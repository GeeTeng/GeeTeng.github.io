---
title: "论文笔记"
date: 2025-01-09
tags: [Paper]
description: "记录读过的论文 更新中"
showDate: true
math: true
chordsheet: true
---



## 一种基于瞬时风险识别的自动驾驶车辆协同进化变道轨迹规划方法

 [A-co-evolutionary-lane-changing-trajectory-planning-metho_2023_Accident-Anal.pdf](/images/Paper/A-co-evolutionary-lane-changing-trajectory-planning-metho_2023_Accident-Anal.pdf) 

提出一种CLTP方法（协同变道轨迹规划），使用HighD数据集，采用深度学习神经网络捕捉变道车辆周围车辆移动的轨迹动态。考虑主体车辆与周围车辆的安全交互，基于故障树分析方法建立变道事件时空风险识别的数学模型。以变道风险最小化为目标，基于加速和减速超车规则以及梯形加速法，设计了变道过程中的纵向和横向位移方案。最后使用遗传算法获取纵向和横向位移的运动参数，形成理想的变道轨迹。

```
为什么提出CLTP方法？

现有的关于 LTP 的研究通常假设本车周围的车辆保持恒定速度，并且仅考虑与周围车辆初始状态相对应的一条变道轨迹。然而周围车辆速度是在变化的，所以动态变道轨迹规划（DLTP）解决了这个问题，根据周围车辆的实时数据为每个时间单位设计新的行驶轨迹，但是DLTP的研究很少，且没有考虑系统反应时间并且假设了目标车辆的初始加速度为0，与实际情况不符合。现有的DLTP模型仍然存在局限性，只考虑了PV（前车）和LV（后车）对目标车道的影响，而没有考虑到原来车道的FV（前车）和RV（后车）也会影响，所以提出基于周边车辆未来运动信 息的新型协同进化变道轨迹规划（CLTP）。

CLTP方法可以做到
1.探索了周边车辆在近未来的运动状态演变趋势，预测瞬时风险，并设计理想的变道轨迹，以克服静态或在线数据的滞后性。
2.评估时空风险，设计了考虑变道过程中本车与周边车辆安全交互的虚拟变道轨迹曲线，解决了其他方法中存在的传统风险识别缺陷。
3.设计了变道的起始和结束点确定模块，为安全变道前纵向位置的动态调整策略提供支持，克服了以往研究中静态
```

目前流行的LTP方法有：

- 几何曲线法：常用的几何曲线包括多项式曲线、圆弧曲线、B 样条曲线、梯形曲线、螺旋曲线等。
- 图搜索法：A*、dijkstra等。
- 人工势场法：假设车辆在具有目标点和障碍物的势场中行驶。目标点被视为引力场的中心，吸引主体车辆靠近，而障碍物则被定义为斥力场的中心，以防止车辆碰撞。

**方法论：**

本方法更适用于相邻车辆为人工驾驶车辆。可通过基于停车距离指数（SDI）的纵向位置调整策略获取变道的起始和结束点位置。基于梯形加速度（TA）方法和遗传算法（GA）设计变道的横向和纵向轨迹，在给定曲率、速度和瞬时风险的约束下，降低变道方案的整体风险系数。

使用GRU（基于门控循环单元）来预测周边车辆的轨迹。然后基于GRU提供的周围车辆未来轨迹，使用SDI指数来判断是否在一段时间内可能发生碰撞。

![0](/images/Paper/0.png)

使用故障树分析方法（FTA）进行风险识别，将瞬时风险系数分为2种情况：

1.越过车道线之前，收到周围4个车辆的影响，目标车辆前后和原车道前后。

2.越过车道线之后，仅收到目标车道前后车辆的影响。

生成轨迹：确定起始点和终点，应在目标车辆的牵扯和候车临界安全间距（SSD）的交集范围内，使用TA进行横向轨迹规划，优化纵向轨迹规划。

最后使用均方根误差（RMSE）、平均绝对误差（MAE）和平均绝对百分 比误差（MAPE）被选作衡量标准，表明GRU比LSTM、ANN要优。

总结：将风险评估与深度学习相结合，通过风险评估系数来优化生成轨迹，调整换道路径和时机。



## 驾驶员对车辆轨迹预测的影响

 [Driver-influence-on-vehicle-trajectory-predi_2021_Accident-Analysis---Preven.pdf](/images/Paper/Driver-influence-on-vehicle-trajectory-predi_2021_Accident-Analysis---Preven.pdf) 

文章将驾驶员特征（如高风险和低风险分类，采用K均值聚类分析）和驾驶表现（如风险行为和非风险行为）纳入了轨迹预测系统中，使用卷积LSTM进行轨迹预测，使用SHRP2数据集，然后将预测结果与实际的驾驶轨迹进行了对比分析。通过这种对比，研究发现驾驶员的特征对轨迹预测系统的准确性有显著影响：

- 低风险（LR）驾驶员在非风险行为（NRB）情境下，轨迹预测误差最低，表明系统在这些情境下的预测更为准确。
- 高风险（HR）驾驶员在风险行为（RB）情境下，轨迹预测误差最高，显示出驾驶员特征确实对轨迹预测的效果产生了重要影响。

![1](/images/Paper/1.png)

作者在简介中着重介绍了近年来关于驾驶员对轨迹预测有影响的研究论文，皆证实了驾驶员的特征（性格、年龄）对轨迹预测有显著的影响，提出了当今对于这一方面的研究仍十分有限。所以本文假设车辆轨迹预测误差与驾驶员特征和驾驶表现存在关联。

本研究的重点是研究驾驶员概况、驾驶表现和其他影响因素（如交通密度、次要任务、性别和年龄组）对轨迹预测系统性能的影响。方法评估基于**驾驶员概况和驾驶相关因素如何影响轨迹预测系统性能**。

评估方法：

- 端到端方法：使用从SHRP2中提取的一组驾驶样本训练预测模型，然后评估系统性能以进行进一步分析。每个驾驶场景的实际值与预测值之间的差异被报告为预测误差。之后，该预测误差用于评估驾驶员概况、驾驶表现以及驾驶员受损、交通密度和次要任务等影响因素对轨迹预测系统准确性的相关性和影响。
- 通过统计问卷将驾驶员概况分类为**低风险和高风险**，将驾驶表现分类为**风险行为和非风险行为**集群，这些信息连同第一步的预测误差进行第三步的分析。
- 分析预测误差和参数之间的关系。**使用随机森林 (RF) 评估其他影响系统性能的因素的影响**。将 RF 拟合到数据中，根据因素与回归问题的相关性对其进行排序，以减少高维数据集中的模型输入数量，从而提高计算效率。

对于驾驶员特征（高风险 / 低风险）：

高风险（HR）驾驶员在道路上与其他车辆保持的车头时距更短，且速度和加速度均高于低风险（LR）驾驶员。与低风险驾驶员相比，高风险驾驶员的横向速度和横向运动中的最大加速度更为显著，这表明高风险驾驶员更倾向于进行变道操作。

对于驾驶表现（风险行为 / 非风险行为）：

预测结果表明从事危险行为的驾驶员会增加轨迹预测的误差，驾驶表现与轨迹预测存在关联。

研究的目的是探索驾驶员概况和驾驶表现对轨迹预测系统性能的影响。调查显示，**基于自我报告问卷的驾驶员概况不会影响系统的性能**。然而，驾驶表现会影响轨迹预测性能。特别是，在表现出风险较高的概况（即驾驶距离太近或从事危险行为）的驾驶员中，轨迹预测比表现出更安全行为的驾驶员表现出更多的错误。这凸显了一个重要问题。也就是说，**轨迹性能预测对于表现出更多危险驾驶倾向的驾驶员来说效果较差**，未来的研究还需要考虑在车辆轨迹预测系统中使用驾驶行为。使用具有相同分布的分类数据并分析特定模式将导致设计个性化的轨迹预测系统，从而提高系统性能和道路安全。

总结：可以使用驾驶员特征进行换道意图识别（个性化行为分析），可以提高换道意图预测的准确性。



## Abstract

> 10.1016/j.compeleceng.2024.110053
>
> Group vehicle trajectory prediction model based on multi-graph fusion

Group vehicle trajectory prediction **is crucial for** autonomous driving systems to make safe and reliable decisions. **However**, existing models are deficient in integrating vehicle history information to capture dynamic interaction features at different time steps **and** exhibit limitations in handling vehicle history states with varying speeds. **To address these issues**, this paper **proposes** a multi-graph fusion group vehicle trajectory prediction model **that aims to** integrate spatio-temporal features(融合时空特征) to accurately predict the future trajectories of groups of vehicles. The model **employs** a graph attention mechanism to model the correlations between vehicles' dynamic information and combines the learned dynamic associations with prior knowledge to extract richer interaction features. **Furthermore**, the model **utilizes** multi-scale spatio-temporal graph convolution（多尺度时空图卷积） to adaptively extract spatio-temporal features of vehicles with different speeds. **In addition**, this paper designs an omni-dimensional dynamic decoder to parse the spatio-temporal features through an omni-dimensional attention mechanism, **thereby** improving the accuracy and effectiveness of the predicted trajectories. **Finally**, experimental results demonstrate that the proposed model achieves higher prediction accuracy with fewer parameters on the NGSIM(Next Generation Simulation) and highD datasets.

---

> 10.1109/tiv.2024.3381990
>
> Graph Representation Learning in the ITS: Car-Following Informed Spatiotemporal Network for Vehicle Trajectory Predictions

Multimodal synchronization（多模态同步） has become the research highlight of the ITS, **where** complex driving scenarios, various types of vehicles and diverse data sources **are crucial constituents**. **As**（由于） real-time microscopic traffic characteristics can be vividly represented by graph data, we **strive to** achieve accurate trajectory predictions via graph-structured series for the stability and the efficiency of transportation. **Although** the existing data-driven algorithms have achieved fabulous accuracy in various simulation tasks, **there are limitations in** the distribution and the number of vehicles under investigation. With car-following patterns applied as physical information, we derive the adjacency matrix and design graph filters to explore the spatial dependence between vehicles via the graph-represented multi-lane traffic. The multi-head attention layer is attached to the spatiotemporal convolutional network as an extension. The rationality and the superiority of our model are validated on two calibrated datasets. Through error comparisons, we discuss the role of changeable hyper-parameters to deduce the optimal model for one-step and multi-step predictions. **Novel ideas are shared in this paper to** simplify the complexity of trajectory prediction in the synchronized transportation system.

---

> [10.1109/tiv.2024.3406507](http://dx.doi.org/10.1109/tiv.2024.3406507)
>
> Dynamic Spatio-temporal Graph Neural Network for Surrounding-aware Trajectory Prediction of Autonomous Vehicles

Trajectory prediction **is a critical aspect of** understanding and estimating the motion of dynamic systems, **including** robotics and autonomous vehicles (AVs). **For** safe and efficient driving behavior, an AV should predict its own motion and the motions of surrounding vehicles in the upcoming time steps. **To achieve this**, understanding the interaction among vehicles is crucial for accurate trajectory prediction. **In this research, we implement** a dynamic Spatio-temporal graph convolutional network to predict the trajectory distribution of vehicles in a traffic scene. We perform the graph convolutional network (GCN) operation on directed graphs to capture the spatial dependencies among vehicles in each traffic scene. To accurately replicate the interaction among vehicles, **we propose** a novel weighted adjacency matrix derived by the strategic positions of vehicles (angular encoding) and the reciprocal of distances among vehicles in a traffic scene. **Additionally**, we employ the temporal convolution network (TCN) to learn the temporal dependencies of a trajectory sequence and decode the future driving status using historic trajectories. We test the model with a naturalistic trajectory dataset (HighD) and conduct performance evaluation. The findings reveal that the proposed model could significantly improve accuracy compared to existing state-of-the-art models. **Meanwhile**, we conduct transfer learning to test the generalizability of our model on low data availability scenario using NGSIM (US-101) dataset. The results show that the relearned model perform comparability well and depicts competing performance in comparison to the state-of-the-art methods.

---

> [10.1109/tiv.2024.3376074](http://dx.doi.org/10.1109/tiv.2024.3376074)
>
> A Cognitive-Based Trajectory Prediction Approach for Autonomous Driving

In autonomous vehicle (AV) technology, the ability to accurately predict the movements of surrounding vehicles **is paramount for**（对于...至关重要） ensuring safety and operational efficiency. Incorporating human decision-making insights **enables** AVs to more effectively anticipate the potential actions of other vehicles, **significantly improving** prediction accuracy and responsiveness in dynamic environments. **This paper introduces** the Human-Like Trajectory Prediction (HLTP) model, which adopts a teacher-student knowledge distillation framework inspired by human cognitive processes. The HLTP model incorporates a sophisticated teacher-student knowledge distillation framework. The “teacher” model, equipped with an adaptive visual sector, mimics the visual processing of the human brain, particularly the functions of the occipital and temporal lobes. The “student” model focuses on real-time interaction and decision-making, drawing parallels to prefrontal and parietal cortex functions. This approach allows for dynamic adaptation to changing driving scenarios, capturing essential perceptual cues for accurate prediction. Evaluated using the Macao Connected and Autonomous Driving (MoCAD) dataset, **along with** the NGSIM and HighD benchmarks, HLTP demonstrates superior performance compared to existing models, **particularly in** challenging environments with incomplete data.

---

> 10.1109/tiv.2024.3404658
>
> A Lane-Changing Trajectory Planning Algorithm Based on Lane-Change Impact Prediction

**Current research on** lane-changing trajectory planning typically focuses on the benefits of the Subject Vehicle (SV), **including** factors such as lane-changing efficiency, comfort, and safety. **However**, this can result in disturbances to the normal driving of the following vehicles. **To address this issue**, **this paper proposes** a lane-changing trajectory planning algorithm that takes into account the impact of lane changes. Based on the trajectories of the following vehicles, a gap distance-based evaluation scheme **is first proposed** to judge whether a vehicle is affected by the lane change of SV. An ordered probit model is **then** developed to predict the number of vehicles affected, with their efficiency and comfort benefits incorporated into the cost function for SV's lane-changing trajectory planning. Safety evaluation of the planned trajectories is **further** performed using an improved predictive risk field that can comprehensively quantify the risk posed by the surrounding vehicles. **Finally**, verification is conducted using the Simulation of Urban Mobility (SUMO) platform, and **the results show** that the proposed algorithm generates optimal lane-changing trajectories that maximize the overall benefits for multiple relevant vehicles when compared with other existing algorithms.

---

> A Self-Trajectory Prediction Approach for Autonomous Vehicles Using Distributed Decouple LSTM
>
> 10.1109/tii.2024.3352231

Vehicle trajectory prediction **plays a crucial role** in ensuring the driving safety of autonomous vehicles in complex traffic scenes. To accurately predict the trajectory of autonomous vehicles, in this article, we propose a distributed decouple long short-term memory (LSTM) self-trajectory prediction method for autonomous driving. The proposed new recurrent network includes a decouple-LSTM unit and corresponding distributed network architecture. To characterize the closed-loop dynamics of autonomous vehicles, a decouple gate and a control gate are proposed to build the decouple-LSTM unit. The data are processed in different ways according to whether the data participates in the recurrent. The decouple gate filters the data participating in the recurrent, while the control gate handles the data outside the recurrent. By leveraging the decouple-LSTM unit, a distributed network architecture is established, which corresponds with the general vehicle motion control architecture, which effectively models the vehicle motion processes. The proposed method is trained using an actual vehicle dataset and validated through vehicle experiments. The prediction horizon ranges from 0.5 to 3 s. When the prediction horizon is set to 3 s, compared with the LSTM method, the mean square error of the proposed method decreases by 98.0%. Results show that the proposed method significantly improves vehicle trajectory prediction accuracy.

---

> 10.1109/tfuzz.2024.3360946
>
> EMSIN: Enhanced Multi-Stream Interaction Network for Vehicle Trajectory Prediction

（提出问题）Predicting the future trajectories of dynamic traffic actors is the **Gordian knot**（高尔丁死结”一词用作一切疑难问题的代称） for autonomous vehicles to achieve collision-free driving. （现状）Most existing works suffer from a gap in characterizing the evolving interactions of scenario components and ensuring the physical feasibility of predictions, particularly in highly heterogeneous scenarios. （提出解决方案）**Therefore**, we propose an Enhanced Multi-Stream Interaction Network (EMSIN), which is devoted to providing accurate trajectory predictions. EMSIN highlights several threads of high-level time-varying interactions, including agent-traffic semantic, self-trend, and agentagent dependencies. A novelly-designed trend-aware mechanism is developed to capture the self-trend interactions from different representation subspaces sufficiently. （又提出来）**To** model the spatial information of traffic agents and extract their evolutions, we present a dynamic adaptive graph convolutional network that extends previously predefined graph paradigms. The adaptive and dynamic graphs in EMSIN are created using learnable node embeddings, allowing the model to discern interaction strengths without additional attention modules. **Finally**, all highlevel feature spaces elaborating multi-stream interactions are fused to generate possible agent actions with corresponding confidence values.（结果、数据集） **Comprehensive experiments** conducted on both L5kit and nuScenes datasets demonstrate that EMSIN surpasses its counterparts, boasting smaller prediction errors and faster inference times. **This study also introduces** a fuzzy-based metric to probe the physical feasibility of predicted trajectories, providing valuable insights into appraising the performance of various prediction models from the perspective of fuzziness.

---

> 10.1016/j.eswa.2023.121412
>
> Intention-convolution and hybrid-attention network for vehicle trajectory prediction

Trajectory prediction **aims to** estimate the future location of the vehicle based on its historical motion state, which **is essential for** driving decision-making and local motion planning of smart vehicles. **However**, affected by the multiple complex interaction in the traffic scene, predicting future trajectory accurately **is a challenging task**. （现状）The majority of existing methods **only focus on** modeling the inter-vehicle interaction, **while ignoring** the influence of road alignment and driver’s lane-change intention, making the poor performance of models, especially for long-term prediction or when the vehicle maneuvers laterally. （提出解决方案）**To overcome the deficiencies**, this paper proposes Intention-convolution and Hybrid-Attention Network (IH-Net) for reliable trajectory prediction. （咋实现的）**Specifically**, we analyze the correlation of lane-change behavior and the motion state of the vehicle, and then the Intention-convolutional Social Pooling module (I-CS) is introduced to extract complete interaction including the driver’s lane-change intention and inter-vehicle interaction. **In addition**, we construct a novel Hybrid Attention Mechanism (H-AM) to explore the trajectory periodicity formed under the restriction of road alignment, **as well as** the impacts of different features on trajectory prediction, which is used to improve the model’s long-term prediction capacity. （结果、数据集）The model’s prediction accuracy with [RMSE](https://www.sciencedirect.com/topics/engineering/root-mean-squared-error) loss function is tested on two public datasets NGSIM and highD, and the results demonstrate that IH-Net remarkably outperforms the state-of-art methods in long-term prediction.

---

> 10.1016/j.ymssp.2023.110854
>
> Lane change trajectory prediction considering driving style uncertainty for autonomous vehicles

Lane change trajectory prediction **is crucial for** autonomous vehicles (AVs) to assess their own driving safety in advance. （提出问题）**However**, there are significant uncertainties in the implementation of such prediction, including different behaviors caused by agent–agent interaction and driving styles of drivers. （稍微解释了一下又一个转折）**While** prototype trajectories can serve as a means to represent typical motion patterns and enhance trajectory prediction performance, their utilization in modeling motion patterns tends to overlook the influence of agent–agent interactions and vehicle dynamics. （提出解决方案）**This paper proposes** a fusion algorithm that considers driving style and vehicle dynamics to address these uncertainties. The algorithm involves a long short-term memory (LSTM) [lane change behavior](https://www.sciencedirect.com/topics/engineering/lane-change-behavior) recognition model that mines key features of agent–agent interaction through the [attention mechanism](https://www.sciencedirect.com/topics/computer-science/attention-machine-learning). The [Gaussian](https://www.sciencedirect.com/topics/physics-and-astronomy/gaussian-distribution) process (GP) motion modeling trajectory prediction (GPMM-TP) algorithm considers the driving style of the prototype trajectory based on the behavior recognition results. **To further improve** short-term and long-term prediction, the interactive multi-model (IMM) algorithm is used to assign probability weights to the [GP model](https://www.sciencedirect.com/topics/computer-science/gaussian-process-model) and the [Extended Kalman Filter](https://www.sciencedirect.com/topics/engineering/extended-kalman-filter) (EKF) model based on prediction accuracy, taking into account the driving styles and the vehicle dynamics. （结果说明有效、数据集）**The proposed algorithm provides a promising approach** to improving the accuracy of lane change trajectory prediction for AVs, and its effectiveness is demonstrated using the HighD dataset.

---

> 10.1109/tim.2024.3400338
>
> Hybrid Kalman Recurrent Neural Network for Vehicle Trajectory Prediction

Accurately forecasting the motion of surrounding vehicles **is a crucial prerequisite**（重要前提） for achieving safe autonomous driving (AD). （现在的方法）Methods for trajectory prediction encompass both physics- and learning-based approaches. （提出问题）**Despite** the significant advancements made by learning-based methods in enhancing performance, ensuring that predicted trajectories are realistic, interpretable, and physically feasible remains a challenging problem.（提出解决方案） **In this study**, we propose a physics-based deep learning (DL) framework founded on an encoder–decoder architecture, modeling the historical motion of traffic agents effectively coupled with the surrounding environment information through attention mechanisms. **Our model** incorporates the vehicle dynamic model (VDM) to couple with vehicle motion and utilizes the Kalman filter to fuse scene context information for multistep accurate and feasible multimodal trajectory prediction. **Our method** leverages the strengths of both learning- and physics-based models. （结果、数据集）The extensive experiment results on the Lyft l5 dataset demonstrate that our model outperforms various baseline approaches in terms of metrics including prediction errors, feasibility, and explainability.

---



Lane change intention recognition is crucial for autonomous vehicles (AVs) to identify potential safety hazards and improve traffic safety. However, in complex driving environments such as highways, recognizing lane change (LC) intentions becomes even more challenging due to the failure to account for the interactions between multiple vehicles. To address these challenges, this paper proposes a fusion algorithm that integrates multi-vehicle collaborative prediction methods and vehicle dynamics. A deep learning neural network captures surrounding vehicles' trajectory dynamics to accurately recognize LC intentions. Furthermore, we introduce a Fault Tree Analysis (FTA) method to mitigate lane change risks, thereby enhancing prediction accuracy. The NGSIM dataset, which provides real-world driving data, is used to validate the proposed framework. The results demonstrate that this approach effectively improves the safety of the lane-changing process.

换道意图识别对于自动驾驶车辆（AVs）识别潜在安全风险和提升交通安全至关重要。然而，在复杂的驾驶环境中，如高速公路，由于未能考虑到多车之间的相互影响，换道意图的识别变得更加具有挑战性。为了解决这些问题，本文提出了一种融合算法，通过结合多车协同预测方法和车辆动力学来应对这些不确定性。采用深度学习神经网络来捕捉周围车辆的轨迹动态，从而识别驾驶员的换道意图。此外，我们还引入了故障树分析（FTA）方法来规避换道风险，进一步提高预测的准确性。使用提供真实行驶数据的NGSIM数据集对所提框架进行了验证。结果表明，该方法能够有效提高换道过程的安全性。
