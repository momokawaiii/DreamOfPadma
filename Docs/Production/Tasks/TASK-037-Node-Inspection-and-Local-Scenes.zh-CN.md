# TASK-037 地块查看与局部场景

- 英文原文：TASK-037-Node-Inspection-and-Local-Scenes.md
- 文档 ID：TASK-037
- 版本：0.1
- 状态：Backlog
- 上级：[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)
- Primary Role：World presentation / Game composition
- Primary Agent：Ready 时指定唯一写入者；尚未启动
- 分支／worktree：预检时指定 Local 分支；确需隔离时才用 worktree

## 目标

从同一份按观察者过滤的世界视图提供地块悬浮信息和含模型的局部场景，支持校验后的进入／返回及人物／建筑详情选择。这补齐查看表现，不代替移动或战斗规则。

## 输入与就绪条件

阅读 AGENTS.md、Docs/00_INDEX.md、Docs/ProjectState.md、前置契约、[画面设计](../../Design/ZH/06_MapAndPreparation.md)、[MVP 决策](../MVPDecisionRegister.zh-CN.md)、ADR-0005、相关模块 README 和目录指令。英文是规范源，中文同步。

- 必要前置：[TASK-012](TASK-012-Run-State-and-Contracts.zh-CN.md), [TASK-014](TASK-014-Character-Presentation.zh-CN.md), [TASK-024](TASK-024-Sandbox-Graph-and-Movement.zh-CN.md), [TASK-025](TASK-025-Node-Operations-and-Story.zh-CN.md), [TASK-040](TASK-040-Model-Assets-and-Scene-Bindings.zh-CN.md)。
- 决策门禁：D02, D12, D21, D24, D25。未回答项不是默认值。
- 用户要求此能力；当前契约只规划它。编码前完成依赖、记录确切批准的行为／内容／数值、审查公共结构并进入 Ready。每个未接受数值都需逐项确认。
- Core 独立于表现；使用稳定身份和强类型命令／结果／视图边界。不宣称运行成功。

## 范围与允许路径

- `Source/DreamOfPadma/Public/MVP/UI/NodeInspection/`
- `Source/DreamOfPadma/Private/MVP/UI/NodeInspection/`
- `Source/DreamOfPadma/Public/MVP/Game/NodeVisit/`
- `Source/DreamOfPadma/Private/MVP/Game/NodeVisit/`
- `Source/DreamOfPadma/Public/MVP/World/NodePresentation/`
- `Source/DreamOfPadma/Private/MVP/World/NodePresentation/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-037/`
- `Content/Padma/MVP/World/LocalNodes/`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.md`
- `Docs/Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.zh-CN.md`

Coordinator 单独拥有共享 index／state／任务矩阵更新。Ready 前明确加入所需规范／ADR／公共契约路径，不推定宽泛租约。排除其他功能代码、配置／构建文件、.agents／.codex、生成目录和第三方内容。

## 范围外

新的移动费用／可见性规则、战斗结算／事务、完整 NPC 关系、全局配置内部实现、ACT／FPS 战斗、World Partition／PCG，以及强制每节点独立地图。

## 验收标准

- [ ] 悬浮及键盘／触控查看展示获准的地形、节点类型、控制方、部署、A/B 附着、设施／状态和路线条件；明确表达不可用／未知信息。
- [ ] 查询服务按 D25 首版全公开政策提供敌我名称、数量、位置、卡牌和模型构造信息。空数据仍区别于缺失数据，不引入情报隐藏机制。
- [ ] 世界与局部表现绑定同一 NodeId、卡实例 ID、EntityId 和状态版本。进入／重建不能复制实体或重置损伤／设施。
- [ ] 场景定义使用带类型软引用和明确摆放，检查缺失地图／模型与过期或不匹配的加载完成，不留下部分状态。
- [ ] 点击只查看地块全部信息，右侧独立“移动到此处”按钮向移动服务提交选中卡实例 ID，采用 D29 成组校验。查看本身不发送扣费／开战命令；查看返回独立于战斗退出／回滚。
- [ ] 可以选择人物／建筑，查看只读卡牌／状态／技能／操作。TASK-014 提供兼容表现组件，战斗动画／执行仍由 TASK-017／030 负责。
- [ ] 返回恢复地图选择／镜头上下文。加载失败保留最后有效世界状态，经地块操作服务产生的合法变化继续反映。
- [ ] 为 TASK-038 暴露导航宿主／意图；TASK-034 绑定真实全局配置页面，不形成反向实现依赖。
- [ ] 双语模块／任务报告、范围内验证及独立只读 Review 通过。

## 验证

观察者过滤（含模型构建）、未知与空的区别、同 ID 重复进入、过期／错误节点加载、缺失资产、上下文恢复，以及已批准的移动／扣费分工。PIE 观察悬浮 → 场景 → 实体 → 返回。

原生修改编译 Editor 目标；通过 Scripts/RunTests.ps1 运行 TASK037 窄自动化并核对实际用例结果。运行 Scripts/AuditDocs.ps1、适用的 ValidateProject.ps1 -Strict 和 git diff --check。记录可复现人工观察，不要求宽泛 UI 自动化套件。TASK-034 负责真实功能组合，集成失败交回功能所有者。

## 归属、学习与恢复

唯一 Primary 写入者，一条串行 UE 构建／Editor 通道；地图、资产、结构和模块 README 配对采用独占租约。有限 Architect／Reviewer 可以只读检查，不增加第二写入者。命令／事件与状态归属在场景重建和存档边界中保持，不使用 Actor 指针。

学习目标：跟踪 NodeId 与 EntityId 在地图、局部 Actor 和存档中的关系，解释为何重建画面不能重建一份所有权。 用户证据：Not started。实现交接提供一次预测和实际观察；Agent 产出不证明用户学习。

检查归属后仅回退本任务经 Review 的修改。不得删除生成／无关数据，未经明确授权不得合并／推送。

## 完成报告

- 状态：Backlog；本次为规划补充，未开始实现。
- 代码／资产、运行测试、用户观察及独立实现 Review：待完成。
- 英中配对同时创建。实现交接提供 Changelog 草稿、确切变更路径、证据和残余风险。

## 2026-09-09 已确认模型／移动交接

D24／D25 已接受：点击显示地块全部信息，右侧放置“移动到此处”，通过移动服务提交选中卡实例 ID。显示完整敌方内容。D29 规定 A／附着 B 成组，UI 不能直接移动 Actor。经真实世界视图和 TASK-020 正式定义消费 TASK-040 表现，由 034 组合；预览绑定需换成权威投影。查看进出不创建战斗快照。
