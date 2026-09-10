# TASK-039 剧情关系与增益状态

- 英文原文：TASK-039-Story-Relationships-and-Bonus-State.md
- 文档 ID：TASK-039
- 版本：0.1
- 状态：Backlog
- 上级：[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)
- Primary Role：Core / Game relationship state
- Primary Agent：Ready 时指定唯一写入者；尚未启动
- 分支／worktree：预检时指定 Local 分支；确需隔离时才用 worktree

## 目标

实现最小用户配好的关系成长与增益来源记录，通过既有资源／卡牌／战斗契约提供校验后的效果来源，并向全局配置提供只读视图。

## 输入与就绪条件

阅读 AGENTS.md、Docs/00_INDEX.md、Docs/ProjectState.md、前置契约、[画面设计](../../Design/ZH/06_MapAndPreparation.md)、[MVP 决策](../MVPDecisionRegister.zh-CN.md)、ADR-0005、相关模块 README 和目录指令。英文是规范源，中文同步。

- 必要前置：[TASK-012](TASK-012-Run-State-and-Contracts.zh-CN.md), [TASK-015](TASK-015-Combat-Calculation-Contracts.zh-CN.md), [TASK-018](TASK-018-Calendar-and-Phase-Scheduler.zh-CN.md), [TASK-019](TASK-019-Resource-and-Global-Ledgers.zh-CN.md), [TASK-020](TASK-020-Card-Catalog-and-Lifecycle.zh-CN.md), [TASK-025](TASK-025-Node-Operations-and-Story.zh-CN.md)。
- 决策门禁：D03, D19, D27, D28。未回答项不是默认值。
- 用户要求此能力；当前契约只规划它。编码前完成依赖、记录确切批准的行为／内容／数值、审查公共结构并进入 Ready。每个未接受数值都需逐项确认。
- Core 独立于表现；使用稳定身份和强类型命令／结果／视图边界。不宣称运行成功。

## 范围与允许路径

- `Source/DreamOfPadma/Public/MVP/Core/Relationships/`
- `Source/DreamOfPadma/Private/MVP/Core/Relationships/`
- `Source/DreamOfPadma/Public/MVP/Game/Relationships/`
- `Source/DreamOfPadma/Private/MVP/Game/Relationships/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-039/`
- `Content/Padma/MVP/Definitions/Relationships/`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Docs/Architecture/Modules/PadmaCore/README.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-039-Story-Relationships-and-Bonus-State.md`
- `Docs/Production/Tasks/TASK-039-Story-Relationships-and-Bonus-State.zh-CN.md`

Coordinator 单独拥有共享 index／state／任务矩阵更新。Ready 前明确加入所需规范／ADR／公共契约路径，不推定宽泛租约。排除其他功能代码、配置／构建文件、.agents／.codex、生成目录和第三方内容。

## 范围外

完整关系网络、虚构 NPC／内容／数值配置、自动转换成战斗角色、UI 实现、Core 直接依赖 GAS／Actor，以及修改其他功能执行器。

## 验收标准

- [ ] 实现前 D28 记录具体剧情角色、触发事件、增益／目标、阈值与每个数值字段、授予／移除／叠加／时机及全局／本局规则。样例值不能关闭门禁。
- [ ] 独立使用 StoryCharacterId、BonusDefinitionId／版本、进度／解锁状态和来源事件身份；不复用合成相性、回合制卡身份或 ACT 角色身份。
- [ ] 通过校验的应用服务接收 TASK-025 剧情结果事件；同一来源事件重放不得重复增加成长或效果，失败保持原状态。
- [ ] 只读资格／效果来源视图说明来源、目标／类别、模式／范围、时钟、授予条件和生效／失效原因。批准的效果消费者拥有执行；关系状态不成为另一套万能效果管理器。
- [ ] 通过 TASK-012 中立贡献端口支持 D28 批准的目标；如果选择 B 卡／类别或合成，使用其所属卡牌／合成修正消费者。不得要求全部关系增益走 ACT GAS，也不继承启示的不可叠加规则。
- [ ] 明确分类全局与本局状态。本局参与完整战斗快照／恢复；独立全局成长不被局部战斗默认撤销。保存权威来源／版本，幂等重建临时效果。
- [ ] 用服务样例验证一条已批准的剧情 → 关系 → 增益 → 实际计算路径；TASK-034 接入真实消费者，本任务不拥有它们的源码。
- [ ] 双语模块／任务报告、范围内验证及独立只读 Review 通过。

## 验证

使用批准值的阈值／资格边界、重复事件幂等、拒绝不支持目标、来源移除、全局／本局归属、快照／读档往返，以及重建效果后的计算一致性。

原生修改编译 Editor 目标；通过 Scripts/RunTests.ps1 运行 TASK039 窄自动化并核对实际用例结果。运行 Scripts/AuditDocs.ps1、适用的 ValidateProject.ps1 -Strict 和 git diff --check。记录可复现人工观察，不要求宽泛 UI 自动化套件。TASK-034 负责真实功能组合，集成失败交回功能所有者。

## 归属、学习与恢复

唯一 Primary 写入者，一条串行 UE 构建／Editor 通道；地图、资产、结构和模块 README 配对采用独占租约。有限 Architect／Reviewer 可以只读检查，不增加第二写入者。命令／事件与状态归属在场景重建和存档边界中保持，不使用 Actor 指针。

学习目标：跟踪剧情事件到持久关系来源与临时增益，解释为什么重新打开 UI 不能再应用一次。 用户证据：Not started。实现交接提供一次预测和实际观察；Agent 产出不证明用户学习。

检查归属后仅回退本任务经 Review 的修改。不得删除生成／无关数据，未经明确授权不得合并／推送。

## 完成报告

- 状态：Backlog；本次为规划补充，未开始实现。
- 代码／资产、运行测试、用户观察及独立实现 Review：待完成。
- 英中配对同时创建。实现交接提供 Changelog 草稿、确切变更路径、证据和残余风险。

## 明确的生产交接

TASK-012 在本任务实现前冻结中立来源／修正值与消费者登记。本任务产生关系来源／资格服务，019／020／022／015 负责选定数值应用，034 注入服务。本任务不回头改造那些执行器，也不二次应用同一标量。
