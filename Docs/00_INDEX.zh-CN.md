# Dream of Padma 文档索引

- 英文原文：[00_INDEX.md](00_INDEX.md)

查找索引：搜索所需主题，不加载全部链接。日常开工只读 ProjectState、当前 TASK 与相关英文段落；历史与示例按需读取。

## 当前状态

- [项目状态](ProjectState.zh-CN.md)——已有行为及剩余验收
- [离线 Demo 路线](Production/Roadmap.zh-CN.md)与[双构建验收](Production/BuildMatrix.zh-CN.md)
- [第零章范围与教程规则](Rules/ChapterZero.zh-CN.md)
- [ADR-0010 Demo 内容、烘焙地图与暂缓编辑器](Decisions/ADR-0010-Offline-Demo-Content-and-Map.zh-CN.md)
- [TASK-053 基于 grill 的技术文档整理](Production/Tasks/TASK-053-Grill-Technical-Docs.zh-CN.md)
- [TASK-052 当前单击/输入/PIE 退出修复](Production/Tasks/TASK-052-Tutorial-Interaction-and-Motion-UI.zh-CN.md)
- [原生可玩指南](Content/NativePlayableDemo.zh-CN.md)、[地图制作](Content/WorldMapAuthoring.zh-CN.md)、[策略表现](Content/StrategyPresentation.zh-CN.md)
- [运行时命名](Architecture/RuntimeNaming.zh-CN.md)、[Agent 工作流](Agent/Workflow.zh-CN.md)、[任务模板](Production/TaskTemplate.zh-CN.md)
- [变更日志](Changelog.zh-CN.md)——早期 TASK-046–052 证据与历史表现决策

## 设计规则

- [第零章 Demo](Rules/ChapterZero.zh-CN.md)

- [时间与日历](Rules/Time.zh-CN.md)
- [卡牌合成](Rules/Synthesis.zh-CN.md)
- [战斗](Rules/Combat.zh-CN.md)
- [世界状态](Rules/WorldState.zh-CN.md)

## MVP 策划基线（已验收）

这些文档中的高层 MVP 范围和规则理解已于 2026-09-02 通过用户验收，作为实现基线。明确标记为“开放”“提案”“待确认”、延期或仅供试玩验证的内容仍未决定，程序实现时不得自行补全。

- [主策划案](Design/ZH/01_MainPlanner.md)
- [系统策划案](Design/ZH/02_SystemPlanner.md)
- [战斗/AI 策划案](Design/ZH/03_CombatAIPlanner.md)
- [关卡/内容策划案](Design/ZH/04_LevelContentPlanner.md)
- [数值策划案](Design/ZH/05_NumericalPlanner.md)

## 架构

- [模块图](Architecture/ModuleMap.zh-CN.md)
- [程序架构基线](Architecture/ProgramArchitecture.zh-CN.md)
- [数据驱动架构](Architecture/DataDrivenArchitecture.zh-CN.md)
- [运行时流程](Architecture/RuntimeFlow.zh-CN.md)
- [存档模式](Architecture/SaveSchema.zh-CN.md)
- [设计模式指导](Architecture/DesignPatterns.zh-CN.md)
- [架构 Agent 规则](Architecture/AGENTS.zh-CN.md)
- [模块 Agent 规则](Architecture/Modules/AGENTS.zh-CN.md)

### 逻辑模块程序文档

- [PadmaCore](Architecture/Modules/PadmaCore/README.zh-CN.md)
- [PadmaGame](Architecture/Modules/PadmaGame/README.zh-CN.md)
- [PadmaGameplay](Architecture/Modules/PadmaGameplay/README.zh-CN.md)
- [未来 ACT 开发 Agent 契约](Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.zh-CN.md)
- [PadmaWorld](Architecture/Modules/PadmaWorld/README.zh-CN.md)
- [PadmaUI](Architecture/Modules/PadmaUI/README.zh-CN.md)
- [PadmaEditor](Architecture/Modules/PadmaEditor/README.zh-CN.md)
- [PadmaTests](Architecture/Modules/PadmaTests/README.zh-CN.md)

## 生产与学习

- [路线图](Production/Roadmap.zh-CN.md)
- [构建矩阵](Production/BuildMatrix.zh-CN.md)
- [第一个基础任务](Production/Tasks/TASK-001-Foundation.zh-CN.md)
- [Agent 工作流初始化任务](Production/Tasks/TASK-002-Agent-Workflow-Bootstrap.zh-CN.md)
- [第一个核心规则契约切片](Production/Tasks/TASK-003-Core-Contracts.zh-CN.md)
- [ACT 架构决策集成](Production/Tasks/TASK-004-ACT-Architecture-Decision.zh-CN.md)
- [确定性随机基础](Production/Tasks/TASK-005-Deterministic-Random-Foundation.zh-CN.md)
- [可玩原型垂直切片集群](Production/Tasks/TASK-006-Playable-Prototype-Cluster.zh-CN.md)
- [SLG 世界选择与切场](Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.zh-CN.md)
- [Encounter 角色与技能数据](Production/Tasks/TASK-008-Encounter-Character-and-Skill-Data.zh-CN.md)
- [出牌与角色召唤](Production/Tasks/TASK-009-Card-Play-and-Character-Summon.zh-CN.md)
- [可玩原型集成](Production/Tasks/TASK-010-Playable-Prototype-Integration.zh-CN.md)
- [学习课程](Learning/Curriculum.zh-CN.md)
- [开发学习闭环](Learning/Workflow.zh-CN.md)
- [GM 命令](Tools/GMCommands.zh-CN.md)

## 完整规则 MVP 任务簇

- [TASK-011](Production/Tasks/TASK-011-Full-MVP-Task-Cluster.zh-CN.md): 总体交付矩阵与依赖顺序
- [MVP 决策登记表](Production/MVPDecisionRegister.zh-CN.md)
- [PDF 卡牌、规则与画面来源提取](Production/MVPSourceExtraction.zh-CN.md)
- [TASK-012](Production/Tasks/TASK-012-Run-State-and-Contracts.zh-CN.md): 本局状态与共享契约
- [TASK-013](Production/Tasks/TASK-013-GAS-Infrastructure.zh-CN.md): 原生 GAS 基础设施
- [TASK-014](Production/Tasks/TASK-014-Character-Presentation.zh-CN.md): 角色渲染与动画
- [TASK-015](Production/Tasks/TASK-015-Combat-Calculation-Contracts.zh-CN.md): 战斗计算契约
- [TASK-016](Production/Tasks/TASK-016-Encounter-Timeline.zh-CN.md): Encounter 行动时间线
- [TASK-017](Production/Tasks/TASK-017-Encounter-GAS-Playable-Loop.zh-CN.md): 基于 GAS 的可玩 Encounter
- [TASK-018](Production/Tasks/TASK-018-Calendar-and-Phase-Scheduler.zh-CN.md): 日历与阶段调度
- [TASK-019](Production/Tasks/TASK-019-Resource-and-Global-Ledgers.zh-CN.md): 资源与全局数值台账
- [TASK-020](Production/Tasks/TASK-020-Card-Catalog-and-Lifecycle.zh-CN.md): 卡牌目录与生命周期
- [TASK-021](Production/Tasks/TASK-021-ABC-Sandbox-Operations.zh-CN.md): ABC 沙盒操作
- [TASK-022](Production/Tasks/TASK-022-Synthesis-Resolution.zh-CN.md): 合成结算
- [TASK-023](Production/Tasks/TASK-023-Synthesis-UI.zh-CN.md): 合成预览与界面
- [TASK-024](Production/Tasks/TASK-024-Sandbox-Graph-and-Movement.zh-CN.md): 沙盒节点图与移动
- [TASK-025](Production/Tasks/TASK-025-Node-Operations-and-Story.zh-CN.md): 节点操作与故事变化
- [TASK-026](Production/Tasks/TASK-026-Ruler-World-AI.zh-CN.md): 统治者世界 AI
- [TASK-027](Production/Tasks/TASK-027-Battle-Routing-and-Travel.zh-CN.md): 战斗路由与场景切换
- [TASK-028](Production/Tasks/TASK-028-Battle-Transactions.zh-CN.md): 战斗快照事务
- [TASK-029](Production/Tasks/TASK-029-Safe-Boundary-Save-Load.zh-CN.md): 安全边界存档与读档
- [TASK-030](Production/Tasks/TASK-030-ACT-GAS-Playable-Loop.zh-CN.md): 基于 GAS 的可玩 ACT
- [TASK-031](Production/Tasks/TASK-031-ACT-Repository-and-Bullet-Time.zh-CN.md): ACT 卡库与子弹时间
- [TASK-032](Production/Tasks/TASK-032-Run-Outcomes-and-Restart.zh-CN.md): 整局胜负与重开
- [TASK-033](Production/Tasks/TASK-033-First-Era-Content-Pack.zh-CN.md): 首个时代内容包
- [TASK-034](Production/Tasks/TASK-034-Full-MVP-Integration.zh-CN.md): 完整 MVP 集成与验收
- [TASK-035](Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md): ACT 角色卡与出战阵容

## Agent 自动化

- 项目自定义 Agent 配置：`.codex/config.toml` 和 `.codex/agents/*.toml`
- 任务执行流程：`.agents/skills/padma-task-runner/SKILL.md`
- 独立 Review 流程：`.agents/skills/padma-review/SKILL.md`
- 基于证据的学习流程：`.agents/skills/padma-learning-loop/SKILL.md`

## 资源

- [资源命名](Content/Naming.zh-CN.md)

## 决策记录

- [ADR-0001 项目结构](Decisions/ADR-0001-ProjectStructure.zh-CN.md)
- [ADR-0002 Encounter 优先与 ACT 准备](Decisions/ADR-0002-Encounter-First-ACT-Preparation.zh-CN.md)
- [ADR-0003 固定可玩原型垂直切片](Decisions/ADR-0003-Playable-Prototype-Vertical-Slice.zh-CN.md)
- [ADR-0004 Encounter 与 ACT 分离的 GAS 体系](Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md)

策划基线的双语同步规则见 [Design/AGENTS.zh-CN.md](Design/AGENTS.zh-CN.md)。

- [ADR-0005 地图定义、表现与存档分层](Decisions/ADR-0005-World-Map-Storage.zh-CN.md)


- [TASK-036 ACT 配置定义](Production/Tasks/TASK-036-ACT-Authoring-Definitions.zh-CN.md)
- [ACT 配置指南](Content/ACTAuthoring.zh-CN.md)

## 地图与全局配置

- [画面设计](Design/ZH/06_MapAndPreparation.md)
- [TASK-037 地块查看与局部场景](Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.zh-CN.md)
- [TASK-038 全局配置与增益界面](Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.zh-CN.md)
- [TASK-039 剧情关系与增益状态](Production/Tasks/TASK-039-Story-Relationships-and-Bonus-State.zh-CN.md)

## UE 模型配置预览

- [TASK-040 模型资产与场景绑定](Production/Tasks/TASK-040-Model-Assets-and-Scene-Bindings.zh-CN.md)
- [模型配置指南](Content/ModelAuthoring.zh-CN.md)

## 当前版本测试

- [TASK-052 教程交互与动态 UI](Production/Tasks/TASK-052-Tutorial-Interaction-and-Motion-UI.zh-CN.md)

- [UE 与 HTML 测试方案](Production/CurrentBuildTestPlan.zh-CN.md)：启动入口、现有功能用例、已知差异与结果记录
- [TASK-041 测试方案交付](Production/Tasks/TASK-041-Current-Build-Test-Plan.zh-CN.md)

## HTML 规则原型优先

- [可玩 HTML 入口](../Prototypes/HTML/index.html)
- [归处／剧情／合成试玩与 UE 承接建议](../Prototypes/HTML/README.zh-CN.md)
- [TASK-042 可玩原型](Production/Tasks/TASK-042-HTML-Playable-Prototype.zh-CN.md)
- [TASK-043 Encounter 战斗动态反馈](Production/Tasks/TASK-043-Encounter-Visual-Feedback.zh-CN.md)
- [TASK-044 选目标、卡牌施放与角色详情](Production/Tasks/TASK-044-Encounter-Targeting-and-Card-Casting.zh-CN.md)

- [TASK-045 精简 Agent 工作流](Production/Tasks/TASK-045-Lean-Agent-Workflow.zh-CN.md)
