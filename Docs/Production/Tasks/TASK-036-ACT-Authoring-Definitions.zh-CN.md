# TASK-036 ACT 配置定义

- 英文原文：TASK-036-ACT-Authoring-Definitions.md
- 文档 ID：TASK-036
- 版本：0.1
- 状态：Verified
- 上级：完整规则 MVP／用户澄清 D23，2026-09-08
- Primary Role / Agent：Gameplay 数据／root，唯一写入者
- 分支/worktree：Local codex/mvp-demo-foundations；无 worktree
- 授权：用户明确要求在 ACT 内容设计前提供模型和 ACT 技能配表软引用。Ready 预检已完成；本任务是静态配置，不是 TASK-030 战斗或 TASK-035 阵容状态。

## 目标与决策

创建可编辑 ACT 角色／武器／技能资产和独立强类型 ACT 技能表。校验缺失内容、引用、身份、绑定与骨架，不代填玩法数值、不启动 GAS。用户可以创建资产、填写导入模型与技能定义，并取得可操作的错误提示。

已确定：ACT 与回合制角色卡分离；基础技能身份／槽位只在后续公共 provider 边界共享。A→F／B→D／C→R 不用于分类 ACT 角色。特性／地形限制是显式配置数据；执行时点、阵容政策及数值仍属 D20／D23。内容版本是结构元数据，不是玩法数值。

## 首先阅读

AGENTS.md；Docs/00_INDEX.md；Docs/ProjectState.md；Source/AGENTS.md；Content/AGENTS.md；Docs/Architecture/DataDrivenArchitecture.md；Docs/Architecture/ModuleMap.md；Docs/Architecture/Modules/PadmaGameplay/README.md；Docs/Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md；ADR-0004；MVPDecisionRegister D23。

## 允许路径

- `Source/DreamOfPadma/Public/MVP/Gameplay/ACT/Authoring/`
- `Source/DreamOfPadma/Private/MVP/Gameplay/ACT/Authoring/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-036/`
- `Content/Padma/MVP/Definitions/ACTCharacterCards/`
- `Scripts/Editor/AuthorACTTemplates.py`
- `Docs/Content/ACTAuthoring.md`
- `Docs/Content/ACTAuthoring.zh-CN.md`
- `Docs/Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md`
- `Docs/Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md`
- `Docs/Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-030-ACT-GAS-Playable-Loop.md`
- `Docs/Production/Tasks/TASK-030-ACT-GAS-Playable-Loop.zh-CN.md`
- `Docs/Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.md`
- `Docs/Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md`
- `Docs/Production/Tasks/TASK-036-ACT-Authoring-Definitions.md`
- `Docs/Production/Tasks/TASK-036-ACT-Authoring-Definitions.zh-CN.md`

Coordinator 的 index／state／Changelog 更新属于单独串行集成文档。无其他写入者拥有此结构。TASK-008 资产和切场代码仍属自身切片，UE 构建通道保持串行。

## 排除范围

GAS／插件／构建依赖、ASC 生命周期、战斗执行、序列、伤害／费用／冷却默认值、ACT 输入／UI、阵容所有权／可用性、存档、C 卡自动转换与第三方素材。资产可解析不代表 ACT 角色可游玩。

## 依赖与交接

没有运行时前置：仅使用现有 Engine 数据资产和软引用 API。TASK-030 消费静态定义并提供受支持的 GAS 执行器和执行数据。TASK-035 消费角色／武器身份及表现，其 RosterData 只含阵容视图／适配器，不重复定义配置资产。ACTCharacterCards 静态内容从 TASK-035 转交本任务。TASK-030 的 ACT/Data 与 Definitions 保留运行执行／效果配置职责。

## 委派

只读 Architect 检查结构／归属，Reviewer 检查代码／数据／测试。root 是唯一写入者，不授权可写子 Agent。

## 验收

- [x] 独立角色、武器和技能定义类提供稳定 ID、内容版本、显示字段与强类型软引用。
- [x] 强类型 ACT 表连接技能身份、定义与逻辑激活绑定；错误行类型和缺失／重复绑定失败。
- [x] 无需改 C++ 即可填写角色模型／动画／技能引用与武器表现；缺失内容保持无效草稿。
- [x] 通过注入的只读目录校验加载引用类型、重复定义 ID、技能身份一致性、骨架兼容及外部执行器／地形 ID。
- [x] 特性限制不暗中拒绝整个角色，不执行未决地形政策。
- [x] 可编辑空模板和双语填写指南说明后续运行要求。
- [x] Editor 编译、窄测试、模板资产检查、双语审计及独立 Review 通过。

## 验证与学习

编译 Editor target；运行 DreamOfPadma.MVP.TASK036 自动化，检查模板资产类型与 DataTable RowStruct，运行 AuditDocs／ValidateProject -Strict／git diff --check。测试未完整草稿、错误表、重复身份、悬空绑定与引用解析。用户检查：创建角色资产，填写模型／表并读取缺失配置提示；不宣称战斗成功。

学习目标：区分软引用、稳定定义 ID 和战斗实例。预测替换模型是否改变角色 ID，检查资产并解释技能表为什么还需要执行器。状态 Not started；用户证据待提供。

## 风险与恢复

软引用不保证资产可加载或被烘焙。运行加载／烘焙和 GAS 执行器注册由后续 Owner 实现。技能表保持薄绑定索引，不放模式计算。仅恢复本任务精确差异，不编辑生成目录。

## 完成报告

- 最终状态：Verified；root 在 Local codex/mvp-demo-foundations 拥有静态配置范围。
- 交付：PadmaACTAuthoring.h／.cpp、ACTAuthoringTest.cpp、AuthorACTTemplates.py、五份可编辑 UE 模板与双语 ACTAuthoring 指南。角色／武器／技能身份和软引用独立于回合制卡、存活 Actor 与阵容状态。
- 验证：UE5.8.2 Editor 编译通过；automation-reviewed/index.json 中 DreamOfPadma.MVP.TASK036.AuthoringValidation 通过，测试警告／失败为零。覆盖不完整草稿、错误表、重复绑定、身份错配／冲突、外部引用 ID、显式特性选择器与 Montage 骨架错配。
- 资产：author-act.log 与 author-finalize.log 的 commandlet 错误／警告为零，对已有内容重跑通过。修正脚本还通过独立 Reviewer 的内存合法已有目录回归。空模板报告缺失内容，不宣称可玩。
- Review：已修复此前重跑 P2，静态结构归属和精确路径已审查。独立只读 Reviewer /root/full_mvp_contract_review 于 2026-09-08 返回 Pass，无剩余 P0–P2 问题；最终证据与 TASK-008／036 共 28 个实现路径均已核验。
- 证据目录：C:/Users/lenovo/.codex/visualizations/2026/09/08/01a07f68-77cf-7fe0-8479-93a66fb8a4f6/runtime-review；build-final.log、automation-reviewed.log 及 automation-reviewed/index.json、author-act.log、author-finalize.log。
- 学习：指南解释软引用、稳定 ID 与战斗实例；用户填写资产和复述待提供。
- 剩余：用户内容和逐项数值、ACT GAS 执行器／输入／动作、运行加载／烘焙、战斗总设置及地形政策执行。未增加玩法／GAS 依赖或数值。
- Changelog：Coordinator 已完成双语 Added 条目 — ACT 可编辑定义、空模板和配置校验。
- 集成：仅工作区候选；未提交、合并或推送。
