# TASK-040 模型资产与场景绑定

- 英文原文：TASK-040-Model-Assets-and-Scene-Bindings.md
- 文档 ID：TASK-040
- 版本：0.1
- 状态：Verified
- Primary Role / Agent：模型表现与集成 / root，唯一写入者
- 分支／worktree：Local codex/mvp-demo-foundations，基于 c946591；不新增 worktree
- 授权：用户于 2026-09-09 要求模型 Data Asset 和场景按选定角色绑定。这个独立配置／预览切片已完成 Ready 预检。

## 目标与决定

提供可在 UE 编辑的静态／骨骼模型资产、按领域区分的只读角色／技能解析，以及原生地块槽位配置预览。选定定义提供其模型和自身技能来源，场景 Actor 是可替换投影。在现有卡牌配置定义和未来卡库契约增加用户要求的三态移动元数据，不执行世界移动。

已接受：点击只查看地块全部信息，右侧独立“移动到此处”按钮提交 ABC 等卡牌移动。首版敌方信息全部公开。A 与附着的 B 成组移动，逐张检查，组校验失败时不能部分移动。FPS 留待未来，Encounter 与 ACT 保留 MVP 要求。mycard.pdf 中明确、无歧义且不与后续已接受规则冲突的数值可以采用；缺失／冲突字段仍向用户确认。

## 先读

AGENTS.md、Docs/00_INDEX.md、Docs/ProjectState.md、Source/AGENTS.md、Content/AGENTS.md、Scripts/AGENTS.md、ProgramArchitecture、DataDrivenArchitecture、ModuleMap、RuntimeFlow、SaveSchema、World／Game／UI 模块 README、WorldState、ADR-0004／0005、TASK-036 与 ACTAuthoring。预览定义不冻结 TASK-012 的存档／本局结构。

## 允许路径

- `Source/DreamOfPadma/Public/MVP/Core/Cards/PadmaCardMobility.h`
- `Source/DreamOfPadma/Private/MVP/Core/Cards/PadmaCardMobility.cpp`
- `Source/DreamOfPadma/Public/MVP/Presentation/Models/PadmaModelPresentation.h`
- `Source/DreamOfPadma/Private/MVP/Presentation/Models/PadmaModelPresentation.cpp`
- `Source/DreamOfPadma/Public/MVP/Game/Presentation/PadmaPresentationCatalog.h`
- `Source/DreamOfPadma/Private/MVP/Game/Presentation/PadmaPresentationCatalog.cpp`
- `Source/DreamOfPadma/Public/MVP/Game/Presentation/PadmaNodePreview.h`
- `Source/DreamOfPadma/Private/MVP/Game/Presentation/PadmaNodePreview.cpp`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-040/ModelPresentationTest.cpp`
- `Source/DreamOfPadma/Public/Demo/Encounter/DemoCharacterDefinition.h`
- `Source/DreamOfPadma/Public/MVP/Gameplay/ACT/Authoring/PadmaACTAuthoring.h`
- `Scripts/Editor/AuthorModelPreview.py`
- `Content/Padma/MVP/Presentation/ModelPreview/`
- `Docs/00_INDEX.md`
- `Docs/00_INDEX.zh-CN.md`
- `Docs/ProjectState.md`
- `Docs/ProjectState.zh-CN.md`
- `Docs/Changelog.md`
- `Docs/Changelog.zh-CN.md`
- `Docs/Production/MVPDecisionRegister.md`
- `Docs/Production/MVPDecisionRegister.zh-CN.md`
- `Docs/Production/MVPSourceExtraction.md`
- `Docs/Production/MVPSourceExtraction.zh-CN.md`
- `Docs/Production/Tasks/TASK-011-Full-MVP-Task-Cluster.md`
- `Docs/Production/Tasks/TASK-011-Full-MVP-Task-Cluster.zh-CN.md`
- `Docs/Production/Tasks/TASK-012-Run-State-and-Contracts.md`
- `Docs/Production/Tasks/TASK-012-Run-State-and-Contracts.zh-CN.md`
- `Docs/Production/Tasks/TASK-014-Character-Presentation.md`
- `Docs/Production/Tasks/TASK-014-Character-Presentation.zh-CN.md`
- `Docs/Production/Tasks/TASK-020-Card-Catalog-and-Lifecycle.md`
- `Docs/Production/Tasks/TASK-020-Card-Catalog-and-Lifecycle.zh-CN.md`
- `Docs/Production/Tasks/TASK-021-ABC-Sandbox-Operations.md`
- `Docs/Production/Tasks/TASK-021-ABC-Sandbox-Operations.zh-CN.md`
- `Docs/Production/Tasks/TASK-024-Sandbox-Graph-and-Movement.md`
- `Docs/Production/Tasks/TASK-024-Sandbox-Graph-and-Movement.zh-CN.md`
- `Docs/Production/Tasks/TASK-027-Battle-Routing-and-Travel.md`
- `Docs/Production/Tasks/TASK-027-Battle-Routing-and-Travel.zh-CN.md`
- `Docs/Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.md`
- `Docs/Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md`
- `Docs/Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.md`
- `Docs/Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.zh-CN.md`
- `Docs/Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.md`
- `Docs/Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.zh-CN.md`
- `Docs/Rules/WorldState.md`
- `Docs/Rules/WorldState.zh-CN.md`
- `Docs/Decisions/ADR-0005-World-Map-Storage.md`
- `Docs/Decisions/ADR-0005-World-Map-Storage.zh-CN.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.zh-CN.md`
- `Docs/Architecture/Modules/PadmaWorld/README.md`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Docs/Architecture/Modules/PadmaCore/README.zh-CN.md`
- `Docs/Architecture/Modules/PadmaWorld/README.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Architecture/Modules/PadmaUI/README.zh-CN.md`
- `Docs/Content/ACTAuthoring.md`
- `Docs/Content/ACTAuthoring.zh-CN.md`
- `Docs/Design/EN/06_MapAndPreparation.md`
- `Docs/Design/ZH/06_MapAndPreparation.md`
- `Docs/Production/Tasks/TASK-040-Model-Assets-and-Scene-Bindings.md`
- `Docs/Production/Tasks/TASK-040-Model-Assets-and-Scene-Bindings.zh-CN.md`
- `Docs/Content/ModelAuthoring.md`
- `Docs/Content/ModelAuthoring.zh-CN.md`

保留既有 TASK-008／036 工作区候选，只添加清单中明确的移动配置头文件字段。排除生成目录和第三方内容。全部写入和串行构建／Editor 通道由 root 拥有。

## 范围外

完整 SLG UI、权威移动／扣费／附着、玩法阵容修改、GAS 执行、FPS 战斗、存读档、结构迁移、既有地图修改、资源重命名、构建依赖和数值调整。预览目录不是第二个玩法卡牌库存。

## 依赖与交接

- 必要前置：[TASK-036](TASK-036-ACT-Authoring-Definitions.zh-CN.md) 静态 ACT 结构；[TASK-008](TASK-008-Encounter-Character-and-Skill-Data.zh-CN.md) 仅用于已有只读 Demo 角色适配器与样例。
- 两者当前 Verified 工作区产物足够支持用户授权的增量预览，无须合并，也不绕过 TASK-009／010 集成门禁。
- TASK-014 消费通用表现描述／Actor。TASK-020／035 在实际卡库复用移动字段。TASK-024 拥有成组移动校验；TASK-037 经 TASK-034 组合消费表现。
- 静态预览槽位含 NodeId、SlotId 和示意实体／定义键，不修改世界状态，也不是 SaveGame 记录。
- ACT 读取既有 Model／AnimationClass／SkillTable 与武器字段。通用模型资产只含表现。模式来源在 Game 中适配，中立表现 Actor 不包含这些来源类型。
- 模型可显示与技能详情可读分别验证。空技能表不妨碍模型预览；未知／错误模式行必须报告失败。
- 新选择缺失或无效时清除过期模型／技能信息。无效地块布局不能只生成部分候选。

## 验收

- [x] 通用三态移动字段明确报告未配置状态和条件 ID，不把未指定卡牌判为可移动。
- [x] 通用 Data Asset 支持恰好一种静态／骨骼模型、可选兼容动画和表现变换。
- [x] 带领域的稳定定义 ID 解析选定来源，不复制 ACT 模型／技能权威。
- [x] 表结构、行／技能身份、缺失资源、重复项和动画兼容问题有可定位诊断；模型与技能就绪分开。
- [x] 原生地块预览加载槽位模型与只读技能，支持替换槽位选择；不修改世界／会话／费用／战斗。
- [x] UE Python 配置可安全重跑，保留既有内容并创建可编辑的项目资产／地图。
- [x] Editor 构建、范围自动化、资产创建／重跑、画面检查、双语／严格／差异检查与独立 Review 通过。

## 验证

使用 UE5.8 编译 DreamOfPadmaEditor。运行 DreamOfPadma.MVP.TASK040 以及受影响的 TASK036／Demo 配置测试。执行并重跑 Scripts/Editor/AuthorModelPreview.py，检查生成的 UE 地图和日志。运行 AuditDocs.ps1、ValidateProject.ps1 -Strict 与范围 git diff --check。日志保存在源码路径之外。

## 学习契约

概念：稳定实例／定义身份与软资产引用的区别；模型就绪与技能可执行的区别。指南提供替换模型／选择槽位的小练习。用户预测／操作／复述仍待提供，实现证据不是学习证据。

## Review 与恢复

只读 Architect 和 Reviewer 检查契约及实际代码／资产／证据，不设写入委派。没有其他写入者拥有这些路径。核对归属后仅恢复本任务精确变更。没有提交、合并或推送授权。

## 完成报告

- 状态：2026-09-09 为 Verified，实现验收通过。本地集成与用户 PIE／学习验收仍待完成；未提交、暂存、合并或推送。
- Review：独立只读架构／验收复核为 Pass，无剩余 P0–P3。两项 P2（重跑校验及无效的骨架失败样例）和一项 P3 数组边界问题已修复并复核。
- 修改路径：上方允许路径中列出的 12 份源码／头文件／测试／脚本，以及全部 52 份 Markdown（26 组同步配对）。Content 目录条目具体为下列六个文件。既有 TASK-008／036 候选变更得到保留，仅在两个明确授权头文件追加移动元数据。
- 代码结果：Core 移动配置值；中立模型资产／描述／Actor；Game 层 Demo／ACT／通用模型适配器；静态地块布局与可替换预览绑定；仅创建缺失资产的配置脚本与专项测试。专用预览 GameMode 不生成 Pawn；非 Shipping 截图需显式启用，销毁时取消相关回调。
- 资产结果位于 `Content/Padma/MVP/Presentation/ModelPreview/`：`DA_Model_Character_Template.uasset`、`DA_Model_Building_Placeholder.uasset`、`DA_Model_Ground_Placeholder.uasset`、`DA_ModelPreview_Catalog.uasset`、`DA_NodeScene_Preview.uasset`、`L_ModelAuthoringPreview.umap`。
- 证据根目录：`C:/Users/lenovo/.codex/visualizations/2026/09/08/01a07f68-77cf-7fe0-8479-93a66fb8a4f6/runtime-review`。证据仅本地保存，不提交。
- 构建：UE5.8.2 DreamOfPadmaEditor Development Win64 成功，`model-build.log` 记录最终无 Pawn GameMode 构建。UBT 初次需要工作区外的正常用户日志／缓存权限，获准提权重试后成功。
- 自动化：`model-tests-reviewed/index.json` 为 6 项 Success，各用例警告／错误均为零，包括 TASK007 切场、两项 TASK008、TASK036 配置与两项 TASK040。骨架用例先用有效 UAnimInstance 派生类证明兼容，再验证不同骨架被拒绝；覆盖模型／技能独立就绪、错误模式／行身份、替换、布局错误、旧内容清除与销毁。
- 资产校验：`model-author.log` 记录创建，`model-author-rerun.log` 校验已保存预览。`model-finalize.log` 记录无效内存既有绑定导致两次预期失败，随后成功重新加载有效保存地图。检查显式确认错误样例已装入，未保存无效选择。
- 画面 smoke：`model-render.log` 与 `model-preview.png` 为实际 UE 独立渲染，退出码 0。已检查 1280×720 图像：坐镇占位、建筑、地表，无默认 Pawn。坐镇日志解析原始三条 Demo 技能定义。自动化之后唯一新增源码是预览 GameMode，已由最终构建和此次渲染覆盖。
- 日志边界：独立运行启动时仍有既有编辑器 widget-factory 与 r.MotionVectorSimulation 警告。自动化发现阶段在项目用例开始前也会输出引擎自测条件信息；六项导出的项目记录没有警告／错误，不宣称完整日志零警告。
- 文档检查：AuditDocs（198 份 Markdown／99 对）、ValidateProject -Strict 和 git diff --check 通过。扩展审计覆盖当前候选全部 Markdown 链接、29 份 TASK-012–040 依赖／元数据配对、依赖无环、Python 语法与空白。Config/DefaultEngine.ini、DreamOfPadma.Build.cs 和 DreamOfPadma.uproject 与 HEAD 相同。
- 剩余边界：完整 SLG／查看 UI 及成组移动／扣费归 TASK-024／037；正式 ABC 卡库接入归 TASK-020；ACT 阵容／GAS 执行归 TASK-035／030。用户尚未提供真实模型／动画集，空通用角色和 ACT 模板有意保留待填写。软引用预览解析不代表战斗就绪。
- 后续需要的决定：逐卡移动策略／条件和缺失／冲突数值、D27 配置时机／状态归属、D28 关系内容及其他后续任务门禁。已接受的点击／公开／FPS／成组规则不重新提问。
- 学习证据：Agent 实现／复现完成。[英文模型指南](../../Content/ModelAuthoring.md) 与[中文镜像](../../Content/ModelAuthoring.zh-CN.md) 提供预测、模型替换、无效选择、复述与迁移步骤；用户实操／复述仍待提供。
- Changelog 条目：UE 模型定义与只读地块场景预览、强类型模型／技能来源解析及三态移动元数据；完整玩法集成待完成。
