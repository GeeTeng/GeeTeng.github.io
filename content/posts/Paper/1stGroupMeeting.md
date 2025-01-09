---
title: "论文笔记"
date: 2025-01-09
tags: [Paper]
description: "记录读过的论文 更新中"
showDate: true
math: true
chordsheet: true
---



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

 [A-co-evolutionary-lane-changing-trajectory-planning-metho_2023_Accident-Anal.pdf](/images/Paper/A-co-evolutionary-lane-changing-trajectory-planning-metho_2023_Accident-Anal.pdf) 

提出一种CLTP方法（协同变道轨迹规划），使用HighD数据集，

> 为什么提出CLTP方法？
>
> 因为现有的模型都是在默认为周围车辆是手动驾驶车辆，而没有考虑假如周围是自动驾驶车辆时，道路中换道将具有随机性和不确定性。
>
> 所以CLTP方法可以做到根据预测车辆轨迹推导出车辆的时间和空间风险，以评估换道安全性。为帮助车辆做好安全换道的准备，可以利用基于停车距离指数（SDI）的纵向位置调整策略来获取起止点位置。基于梯形加速度（TA）方法和遗传算法（GA）设计换道横向和纵向轨迹，旨在给定曲率、速度和瞬时风险的约束下，降低换道方案的总体风险系数。

- 使用深度学习神经网络（GRU）来捕获变道车辆周围车辆移动的轨迹运动学，其次使用**故障树分析方法**建立变道事件时空风险识别的数学模型，目标是获得未来的交通环境信息，将变道风险最小化。
  - GRU是一种改进的循环神经网络（RNN）模型，通过引入重置门、更新门和输入调制门来解决长期记忆和反向传播中的梯度消失问题，可以对历史数据的状态流和输入流进行细微的修改。因此，与长短期记忆（LSTM）和RNN相比，GRU更容易训练，训练效率和性能也更好。


- 基于加减速超车规则和梯形加速度方法，设计了变道过程中的纵向和横向位移方案。最后利用遗传算法获取纵向和横向位移的运动参数，形成理想的变道轨迹。

优点：考虑到了SV（目标车辆）换道过程中原来车道的FV、RV对其的影响，预测瞬时风险克服静态或在线数据的滞后性，设计了换道起点和终点确定模块，以提供在安全换道前纵向位置的动态调整策略，克服了以往研究中静态起点和终点灵活性差的问题。

总结：提供了一种新思路，算法与深度学习相结合，***RRT（快速随机树）/MPC（模型预测控制）车辆路径规划和深度学习预测车辆换道意图这部分可以提供换道的可能性以及换道时机的信息***，生成最优路径。

---

 [Driver-influence-on-vehicle-trajectory-predi_2021_Accident-Analysis---Preven.pdf](/images/Paper/Driver-influence-on-vehicle-trajectory-predi_2021_Accident-Analysis---Preven.pdf) 

文章将驾驶员特征（如高风险和低风险分类）和驾驶表现（如风险行为和非风险行为）纳入了轨迹预测系统中，使用卷积LSTM进行轨迹预测，使用SHRP2数据集，然后将预测结果与实际的驾驶轨迹进行了对比分析。通过这种对比，研究发现驾驶员的特征对轨迹预测系统的准确性有显著影响：

- 低风险（LR）驾驶员在非风险行为（NRB）情境下，轨迹预测误差最低，表明系统在这些情境下的预测更为准确。
- 高风险（HR）驾驶员在风险行为（RB）情境下，轨迹预测误差最高，显示出驾驶员特征确实对轨迹预测的效果产生了重要影响。

![01](/images/Paper/01.png)

本研究的重点是研究驾驶员概况、驾驶表现和其他影响因素（如交通密度、次要任务、性别和年龄组）对轨迹预测系统性能的影响。方法评估基于驾驶员概况和驾驶相关因素如何影响轨迹预测系统性能。将驾驶员概况分类为低风险和高风险，将驾驶表现分类为风险行为和非风险行为集群，以及对其他影响因素的重要分析。第一步，使用从SHRP2中提取的一组驾驶样本训练预测模型，然后评估系统性能以进行进一步分析。每个驾驶场景的实际值与预测值之间的差异被报告为预测误差。之后，该预测误差用于评估驾驶员概况、驾驶表现以及驾驶员受损、交通密度和次要任务等影响因素对轨迹预测系统准确性的相关性和影响。

使用随机森林 (RF) 评估其他影响系统性能的因素的影响。将 RF 拟合到数据中，根据因素与回归问题的相关性对其进行排序，以减少高维数据集中的模型输入数量，从而提高计算效率。

研究的目的是探索驾驶员概况和驾驶表现对轨迹预测系统性能的影响。本文根据驾驶员概况和驾驶表现，探讨了根据个体差异对车辆轨迹预测性能的影响。调查显示，基于自我报告问卷的驾驶员概况不会影响系统的性能。然而，驾驶表现会影响轨迹预测性能。特别是，在表现出风险较高的概况（即驾驶距离太近或从事危险行为）的驾驶员中，轨迹预测比表现出更安全行为的驾驶员表现出更多的错误。这凸显了一个重要问题。也就是说，轨迹性能预测对于表现出更多危险驾驶倾向的驾驶员来说效果较差，未来的研究还需要考虑在车辆轨迹预测系统中使用驾驶行为。使用具有相同分布的分类数据并分析特定模式将导致设计个性化的轨迹预测系统，从而提高系统性能和道路安全。

总结：确定将换道意图加入轨迹预测，和该文方法一致。可以使用驾驶员特征进行换道意图识别（个性化行为分析），可以提高换道意图预测的准确性。
