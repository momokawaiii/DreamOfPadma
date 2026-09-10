# TASK-011 完整 MVP 任务簇与来源核对

- 英文原文：TASK-011-Full-MVP-Task-Cluster.md
- 文档 ID：TASK-011
- 版本：0.3
- 状态：Verified
- Primary Role / Agent：Integration Coordinator / 当前文档主 Agent
- 上级里程碑：完整可玩规则 MVP
- 工作区：当前 Local 检出；本契约拥有文档
- 授权：用户于 2026-09-08 要求此任务簇、确认独立 GAS 模式和卡牌集合，随后要求提取两份 PDF 并实现。运行执行绑定相应子契约。

## 目标

交付 TASK-011 至 TASK-035 共 25 份双语任务契约：本规划／核对任务与 24 项实现任务。提供[来源提取](../MVPSourceExtraction.zh-CN.md)、[决策登记表](../MVPDecisionRegister.zh-CN.md)、ADR-0004／0005 以及同步的规则／架构／生产入口。

v0.3 在该交付上新增画面设计和 TASK-037／038／039，并把地块查看、全局配置及最小关系增益接入现有任务的接口、依赖与验收。TASK-036 是单独交付的静态 ACT 配置任务。

完整 MVP 覆盖七时代章节结构中的一个七天时代、约八个共享沙盘节点、至少两张 A／两张 B／两张 C、基础战斗技能集和无奖励失败定义、资源操作、合成、世界变化／统治者响应、完整 Encounter 与 ACT、安全边界持久化及两种已确认整局胜利。明确整局失败仍需用户决定，不将第七天强行设为失败。

## 必读

阅读 AGENTS.md、Docs/00_INDEX.md、Docs/ProjectState.md、Docs/Agent/Workflow.md、Docs/Agent/CodexSetup.md、TaskTemplate、四份 Rules、五份英文策划基线、ProgramArchitecture、DataDrivenArchitecture、ModuleMap、RuntimeFlow、SaveSchema、相关模块 README、ACTDevelopmentContract 与 TASK-003 至 TASK-010。阅读来源提取所指的原 PDF。英文是规范源。

## 已确认决策与开放边界

- Encounter 与 ACT 都使用原生 GAS，目录、行结构、时钟、执行／状态体系、技能与效果实例由模式各自拥有。共享基础设施不要求效果相等，也不要求共用存活技能／AttributeSet／冷却。
- 回合制／ABC 部署卡与 ACT 角色卡是两套集合。战斗总设置选择 ACT 角色／武器；基础非 A 技能卡仍共享身份／槽位，但效果由模式分别配置。
- ACT 地形限制明确作用于特性、技能、武器或整角色。限制特性不能静默变成整角色禁用。
- 用户开工后提供占位素材，正式动画／渲染打磨后做。每个未接受数值（含临时测试值）均须用户逐项确认。
- SLG 为文明 6 式斜俯视，在六边形外观地图上按节点与预设连线移动，不自动按六边相邻移动。用户已接受静态章节定义资产、.umap 表现与 SaveGame 本局变化（ADR-0005）。
- 用户将 mycard 6–7 页契印沙盘召唤旧效果排除首版，后续重做。其他来源冲突明确保留；旧 PDF 不能静默覆盖后续接受规则。
- PDF 有五张明确 C 类人物，但没有独立 ACT 阵容／武器卡集。TASK-035／030 需要补充来源，不将 C 卡转换成 ACT 卡。
- 仍先交付 Encounter，已有 007–010 表现夹具保持有界。首个随机功能之前需要 TASK-005。

## 范围与独占写集

本任务修改下表链接的 25 份 TASK 配对（含本配对），以及以下确切文档路径：

- `Docs/Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.md`
- `Docs/Decisions/ADR-0004-Separate-Encounter-and-ACT-GAS.zh-CN.md`
- `Docs/Decisions/ADR-0002-Encounter-First-ACT-Preparation.md`
- `Docs/Decisions/ADR-0002-Encounter-First-ACT-Preparation.zh-CN.md`
- `Docs/Architecture/ProgramArchitecture.md`
- `Docs/Architecture/ProgramArchitecture.zh-CN.md`
- `Docs/Architecture/DataDrivenArchitecture.md`
- `Docs/Architecture/DataDrivenArchitecture.zh-CN.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/RuntimeFlow.zh-CN.md`
- `Docs/Architecture/ModuleMap.md`
- `Docs/Architecture/ModuleMap.zh-CN.md`
- `Docs/Architecture/SaveSchema.md`
- `Docs/Architecture/SaveSchema.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.md`
- `Docs/Architecture/Modules/PadmaGameplay/ACTDevelopmentContract.zh-CN.md`
- `Docs/Rules/Combat.md`
- `Docs/Rules/Combat.zh-CN.md`
- `Docs/Design/EN/03_CombatAIPlanner.md`
- `Docs/Design/ZH/03_CombatAIPlanner.md`
- `Docs/Production/Roadmap.md`
- `Docs/Production/Roadmap.zh-CN.md`
- `Docs/ProjectState.md`
- `Docs/ProjectState.zh-CN.md`
- `Docs/00_INDEX.md`
- `Docs/00_INDEX.zh-CN.md`
- `Docs/Changelog.md`
- `Docs/Changelog.zh-CN.md`
- `Docs/Production/MVPDecisionRegister.md`
- `Docs/Production/MVPDecisionRegister.zh-CN.md`
- `Docs/Design/EN/01_MainPlanner.md`
- `Docs/Design/ZH/01_MainPlanner.md`
- `Docs/Design/EN/02_SystemPlanner.md`
- `Docs/Design/ZH/02_SystemPlanner.md`
- `Docs/Design/EN/04_LevelContentPlanner.md`
- `Docs/Design/ZH/04_LevelContentPlanner.md`
- `Docs/Architecture/Modules/PadmaCore/README.md`
- `Docs/Architecture/Modules/PadmaCore/README.zh-CN.md`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaGame/README.zh-CN.md`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Architecture/Modules/PadmaUI/README.zh-CN.md`
- `Docs/Production/MVPSourceExtraction.md`
- `Docs/Production/MVPSourceExtraction.zh-CN.md`
- `Docs/Rules/WorldState.md`
- `Docs/Rules/WorldState.zh-CN.md`
- `Docs/Architecture/Modules/PadmaWorld/README.md`
- `Docs/Architecture/Modules/PadmaWorld/README.zh-CN.md`
- `Docs/Decisions/ADR-0005-World-Map-Storage.md`
- `Docs/Decisions/ADR-0005-World-Map-Storage.zh-CN.md`

Source、Content、Config、构建文件、TASK-005 至 TASK-010、生成目录、.agents 和 .codex 不在本文档契约中。用户后续实现要求在各自经 Review 的任务写集内执行；已有固定切片中发现的旅行／启动缺口须在运行实现前明确路径归属。

另一 Coordinator 已在本任务修改共享文档前完成 TASK-007 和共享集成更新。以 main 的 c946591、TASK-007 Done／TASK-008 Ready 为预检基线，保留独立提交 d8f107f 的 DefaultEngine.ini 修改。本任务不授权合并、推送、标签或发布。

## 交付矩阵

编号是身份，不是数字顺序排期。新运行时契约从 Backlog 开始，实际前置结果与相关决策满足后进入 Ready。

| TASK | 独立交付结果 | 必需前置 | 决策门禁 |
|---|---|---|---|
| [012](TASK-012-Run-State-and-Contracts.zh-CN.md) | 本局状态与共享契约 | 010, 011, 040 | D04 |
| [013](TASK-013-GAS-Infrastructure.zh-CN.md) | 原生 GAS 基础设施 | 012 | D01, D04 |
| [014](TASK-014-Character-Presentation.zh-CN.md) | 角色渲染与动画 | 012, 040 | D02 |
| [015](TASK-015-Combat-Calculation-Contracts.zh-CN.md) | 战斗计算契约 | 012 | D03, D06, D22 |
| [016](TASK-016-Encounter-Timeline.zh-CN.md) | Encounter 行动时间线 | 015 | D05, D06 |
| [017](TASK-017-Encounter-GAS-Playable-Loop.zh-CN.md) | 基于 GAS 的可玩 Encounter | 013, 014, 015, 016 | D01, D03, D05, D06 |
| [018](TASK-018-Calendar-and-Phase-Scheduler.zh-CN.md) | 日历与阶段调度 | 012 | D07, D22 |
| [019](TASK-019-Resource-and-Global-Ledgers.zh-CN.md) | 资源与全局数值台账 | 005, 012, 018 | D03, D08 |
| [020](TASK-020-Card-Catalog-and-Lifecycle.zh-CN.md) | 卡牌目录与生命周期 | 005, 012, 018, 019, 040 | D01, D03, D09, D22 |
| [021](TASK-021-ABC-Sandbox-Operations.zh-CN.md) | ABC 沙盒操作 | 019, 020, 024 | D03, D10 |
| [022](TASK-022-Synthesis-Resolution.zh-CN.md) | 合成结算 | 005, 019, 020, 021 | D03, D11, D22 |
| [023](TASK-023-Synthesis-UI.zh-CN.md) | 合成预览与界面 | 021, 022 |  |
| [024](TASK-024-Sandbox-Graph-and-Movement.zh-CN.md) | 沙盒节点图与移动 | 012, 019, 020 | D03, D12, D21, D24, D25 |
| [025](TASK-025-Node-Operations-and-Story.zh-CN.md) | 节点操作与故事变化 | 018, 021, 024 | D03, D12, D19 |
| [026](TASK-026-Ruler-World-AI.zh-CN.md) | 统治者世界 AI | 005, 015, 018, 019, 024, 025 | D03, D13 |
| [027](TASK-027-Battle-Routing-and-Travel.zh-CN.md) | 战斗路由与场景切换 | 017, 021, 024, 025, 026, 035 | D14, D24, D26 |
| [028](TASK-028-Battle-Transactions.zh-CN.md) | 战斗快照事务 | 005, 018, 019, 020, 021, 022, 024, 025, 026, 027, 035 | D04, D15 |
| [029](TASK-029-Safe-Boundary-Save-Load.zh-CN.md) | 安全边界存档与读档 | 028, 035 | D04, D15 |
| [030](TASK-030-ACT-GAS-Playable-Loop.zh-CN.md) | 基于 GAS 的可玩 ACT | 013, 014, 015, 017, 027, 028, 035 | D01, D03, D04, D06, D16, D23 |
| [031](TASK-031-ACT-Repository-and-Bullet-Time.zh-CN.md) | ACT 卡库与子弹时间 | 020, 030 | D01, D09, D17, D23 |
| [032](TASK-032-Run-Outcomes-and-Restart.zh-CN.md) | 整局胜负与重开 | 019, 025, 026, 027, 029 | D03, D18 |
| [033](TASK-033-First-Era-Content-Pack.zh-CN.md) | 首个时代内容包 | 023, 025, 026, 029, 031, 032, 035, 037, 038, 039 | D02, D03, D11, D12, D13, D18, D19, D22, D23, D24, D25, D26, D27, D28 |
| [034](TASK-034-Full-MVP-Integration.zh-CN.md) | 完整 MVP 集成与验收 | 005, 010, 012, 013, 014, 015, 016, 017, 018, 019, 020, 021, 022, 023, 024, 025, 026, 027, 028, 029, 030, 031, 032, 033, 035, 037, 038, 039 | ALL |
| [035](TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md) | ACT 角色卡与出战阵容 | 012, 019, 020, 036, 040 | D01, D02, D03, D20, D23, D27 |

用户澄清 D23 新增 [TASK-036](TASK-036-ACT-Authoring-Definitions.zh-CN.md)，作为独立配置先行切片。它没有运行时前置，从 035 接管静态 ACT 角色／武器／技能资产，向 035／030 提供软引用定义，不关闭可玩内容或数值门禁。此后续契约不属于原始 102 路径规划交付清单。

## 推荐执行顺序与可见检查点

1. 获授权后集成已 Review 的 TASK-008，再按 009 → 010 完成卡牌／UI 和固定切片验收。这独立于完整地图与全局配置工作。
2. 012 → 013／014／015 → 016 → 017 得到首个 GAS Encounter。独立可注入的卡牌／付款／生命周期 provider 使此阶段可测；020 提供正式 provider，034 负责绑定。
3. 018 → 019 → 020 → 024 → 021 → 022 → 023 实现日历、经营、图／部署与合成。TASK-005 先于相关随机功能。035 可在 012／019／020 后进行；025 依赖 018／021／024，再进行 026。
4. 027 → 028 → 029 将已 Review 的世界、Encounter 与阵容上下文接入完整事务和安全保存。
5. 030 → 031 在 Encounter、路线、事务和阵容可用后实现 ACT，使用自己的 GAS 模式定义及共享基础技能映射。
6. 032 提供整局结局，033 编写用户批准的首时代内容包，034 验证完整可玩一局及恢复路径。

地图／配置分支：012／014／024／025／040 后进入 037；039 等待其列出的规则／剧情服务，038 等待 035 和 039。012 先冻结中立增益来源端口，相关计算任务用注入样例完成自身消费者，034 再绑定 039 的真实来源；不形成反向任务依赖。033／034 在这三项新任务也完成后验收完整画面与效果。

不暗示工时承诺：未决规则／数值和用户素材导入会影响实现进度。每次问下一项实际阻塞的问题，同时推进独立工作。

TASK-012 先冻结中立卡牌／付款／生命周期端口，017／030 消费、020 实现、034 注入。019 拥有熵归因／灾变值契约，025 应用世界后果、026 消费反制事件；033／034 明确验证低熵安全和可复现高熵收益／灾变两条路线，全部触发和数值仍由用户确认。

## MVP 覆盖

| 已接受要求 | 功能 Owner | 最终观察 |
|---|---|---|
| 黎明／上午／黄昏、七天时代和七时代结构 | 018 | 推进完整首时代，不跳过调度效果 |
| 心流／算力、熵、信仰、威慑、天平、核心／稳定值 | 019／026／032 | 分账、可见 HUD 与设计费用／后果 |
| ABC、基础技能、生命周期、沙盒技能与合成 | 020–023 | 放置／附着／用卡，查看预览，成功或无奖励失败 |
| 共享节点、移动、条件道路、轶闻／熔炉／薪火和 AI | 024–026 | 归属／状态变化与统治者响应 |
| 模式路线、切图、完整提交／回滚 | 027／028 | 两种局部模式返回一致状态，失败／退出精确恢复 |
| 角色渲染／反馈与独立 GAS 执行 | 013–017／030／031 | 可见 Encounter 时间线、ACT 动作与 Tab 卡库 |
| 独立 ACT 集合、总设置和地形限制 | 035／027／030 | 选择角色／武器，直接命令也校验特性限制 |
| 安全保存、双胜利、明确失败与重开 | 029／032 | 往返、两种胜利、失败和干净新局 |
| 真实来源卡牌／内容及用户完整游玩 | 033／034 | 七天经营／合成／世界／战斗／保存／结局路线 |

## 归属与实现交接

一个 Primary Agent 拥有一项任务及其精确路径。当前策略使用 Local feature branch；需要真实独立写隔离时才使用 worktree。只有一条 UE 构建／Editor／PIE 通道。共享头文件、模式结构、模块 README 配对、地图、二进制资产、构建配置和最终装配采用串行租约。

状态 Owner 实现 TASK-012 快照参与接口。TASK-035 在持久化前区分全局预设与本局装备。TASK-034 只连接公共 provider／适配器，不内部修复子功能源码；缺陷回原 Owner。单个打断／死亡由 Encounter 时间线规则处理，不自动成为战斗终局。

文档要求及后续实现要求提供任务意图；未决玩法与数值仍须用户回答。技术结构由 Architect Review。关闭门禁所需规范路径在预检时明确加入，不视作任意写入授权。

## Review 与验收

Docs/Architecture/AGENTS.md 要求架构与受影响规则 Review。只读架构师已检查模式／状态边界并指出任务、provider、生命周期修正；只读最终 Reviewer 检查实际文件与双语一致性。两者均不可写。

- [x] TASK-011–035 双语契约包含目标、精确路径、依赖、验收、测试、学习和恢复。
- [x] 明确独立 GAS 模式、角色集合、共享技能、占位素材与逐项数值权限。
- [x] 已读并记录 PDF 来源，冲突可见。
- [x] 所有已接受要求得到覆盖，无循环或隐藏依赖。
- [x] 共享规则／架构／生产入口已同步。
- [x] 配对、链接／图、严格项目和空白检查通过。
- [x] 独立 Review 无未解决 P0–P2 问题。

## 验证

运行 Scripts/AuditDocs.ps1、Scripts/ValidateProject.ps1 -Strict、任务链接／依赖／写集审计以及限定 git diff --check。AuditDocs 只检查配对存在性；链接与译文含义单独检查。本任务不宣称 UE 构建或 PIE 成功。

## 学习契约

| 目标 | 证据 | 状态 |
|---|---|---|
| 共享基础设施与独立模式 | 预测 ACT 专用表修改是否影响 Encounter，检查边界并解释一种禁止依赖。 | Not started |
| 规则地图与渲染地图 | 追踪 NodeId／连线到显示六边格与已存归属，解释为何移动镜头不能改变邻接。 | Not started |

实践：选择一张来源卡，沿 TASK 追踪身份、合法模式、效果 Owner 和未决字段。迁移：解释另一模式如何复用基础技能身份而不继承 ACT 序列。Agent 文档不代表用户学习已完成。

## 风险与恢复

主要风险为跨模式耦合、陈旧来源规则、定义缺失、隐藏依赖和共享写冲突。具体冲突在相关实现前解决，同时推进独立已授权工作。恢复前重新检查归属，仅变更本任务文档差异，保留无关工作。

## 版本 0.2 完成报告

- 最终状态：Verified；文档已验证并通过独立 Review。后续用户决定 D22d／D23 已记录，运行实现使用独立契约。
- 修改文件：链接的 25 份 TASK 中英文配对与上方精确附加清单。
- 检查：AuditDocs 通过（182 份 Markdown／91 对）；严格项目验证、git diff --check 通过；102 路径／942 链接／24 子任务依赖及双语审计通过，无环。Architect、System、Numerical 和最终 Reviewer 对文档返回 Pass。不宣称运行或用户学习证据。
- 子契约已纳入架构修正：独立模式 provider、强类型卡／实体身份、打断／死亡生命周期、ACT 阵容本局分类及直接命令地形限制校验。
- 用户学习证据：尚未提供。
- 集成：工作区交付；未合并或执行远端操作。

## 版本 0.3 地图与全局配置补充

用户最新要求补充悬浮／含模型局部场景，以及全局 ACT／FPS／关系增益视图。当前 root 仍是唯一文档写入者。除上方原有相关路径，本补充还拥有以下确切路径：

- `Docs/Design/EN/06_MapAndPreparation.md`
- `Docs/Design/ZH/06_MapAndPreparation.md`
- `Docs/Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.md`
- `Docs/Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.zh-CN.md`
- `Docs/Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.md`
- `Docs/Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.zh-CN.md`
- `Docs/Production/Tasks/TASK-039-Story-Relationships-and-Bonus-State.md`
- `Docs/Production/Tasks/TASK-039-Story-Relationships-and-Bonus-State.zh-CN.md`

原 v0.2 完成报告是历史证据，v0.3 独立 Review。[画面设计](../../Design/ZH/06_MapAndPreparation.md) 区分已接受要求、PDF 依据和建议。D24–D28 保留未答行为／数值。TASK-008／036 保持 Verified，不推定合并授权。

| Task | 交付 | 必要前置 | 门禁 |
|---|---|---|---|
| [037](TASK-037-Node-Inspection-and-Local-Scenes.zh-CN.md) | 地块查看／局部场景 | 012, 014, 024, 025, 040 | D02, D12, D21, D24, D25 |
| [038](TASK-038-Global-Preparation-and-Bonus-UI.zh-CN.md) | 全局配置／增益视图 | 012, 019, 020, 035, 039 | D20, D23, D26, D27, D28 |
| [039](TASK-039-Story-Relationships-and-Bonus-State.zh-CN.md) | 最小关系／增益状态 | 012, 015, 018, 019, 020, 025 | D03, D19, D27, D28 |

033／034 还依赖 037／038／039。037 输出导航意图，不依赖 038 实现，034 绑定真实服务。FPS 战斗等待 D26。早先近似节点／内容数量仍是规划目标，服从用户布局／内容决定。本补充不宣称新增 UE 运行验证。

## 版本 0.3 完成报告

- 最终状态：Verified，仅指文档补充已验证；画面设计仍为 Draft，TASK-037／038／039 保持 Backlog，D24–D28 未获用户回答的部分仍为 Open。
- 交付：更新 46 个既有 Markdown 路径，新增 8 个路径，共 27 对。精确清单见下表；未更改 Source、Content 或配置。
- 来源：重新核对桌面 cd 下两份 PDF 的路径与哈希；mygame 第 3、5、6–11 页作为画面／信息设计依据，用户后续确认覆盖冲突旧规则。
- Review：独立只读架构复核返回 Pass，无剩余 P0–P2。已补齐 012 中立增益端口与消费方归属，039 的效果目标由 D28 决定，UI 旧点击移动语义已纠正。主交付矩阵同步新增依赖／门禁。
- 检查：AuditDocs 通过（194 份 Markdown／97 对），严格项目验证与 git diff --check 通过；当前完整工作区候选的 1,034 个本地链接及 28 份后续任务依赖／双语元数据检查通过，无依赖环。当前候选合计 144 个路径，包含此前交付；本次仅为本表的 54 个文档路径。
- 设计预览：会话可视化草图通过 1024／736／360 宽度和明暗主题共六组浏览器检查；悬浮、地块进入／返回、全局配置各页、共享技能模式切换通过，无横向溢出。草图不作为 UE 运行、动画或游戏规则验收证据。
- 待决：点击是否包含移动、敌方信息公开和 FPS 首版范围已提问；配置时机／状态归属、具体关系效果与数值继续逐项确认。
- 学习：用户操作／解释证据尚未提供。没有提交、合并或推送，也未重新运行 UE；TASK-008／036 保留此前独立证据。

| 英文路径 | 中文路径 |
|---|---|
| `Docs/00_INDEX.md` | `Docs/00_INDEX.zh-CN.md` |
| `Docs/Architecture/Modules/PadmaCore/README.md` | `Docs/Architecture/Modules/PadmaCore/README.zh-CN.md` |
| `Docs/Architecture/Modules/PadmaUI/README.md` | `Docs/Architecture/Modules/PadmaUI/README.zh-CN.md` |
| `Docs/Decisions/ADR-0005-World-Map-Storage.md` | `Docs/Decisions/ADR-0005-World-Map-Storage.zh-CN.md` |
| `Docs/Design/EN/04_LevelContentPlanner.md` | `Docs/Design/ZH/04_LevelContentPlanner.md` |
| `Docs/Design/EN/06_MapAndPreparation.md` | `Docs/Design/ZH/06_MapAndPreparation.md` |
| `Docs/Production/MVPDecisionRegister.md` | `Docs/Production/MVPDecisionRegister.zh-CN.md` |
| `Docs/Production/MVPSourceExtraction.md` | `Docs/Production/MVPSourceExtraction.zh-CN.md` |
| `Docs/Production/Roadmap.md` | `Docs/Production/Roadmap.zh-CN.md` |
| `Docs/Production/Tasks/TASK-011-Full-MVP-Task-Cluster.md` | `Docs/Production/Tasks/TASK-011-Full-MVP-Task-Cluster.zh-CN.md` |
| `Docs/Production/Tasks/TASK-012-Run-State-and-Contracts.md` | `Docs/Production/Tasks/TASK-012-Run-State-and-Contracts.zh-CN.md` |
| `Docs/Production/Tasks/TASK-014-Character-Presentation.md` | `Docs/Production/Tasks/TASK-014-Character-Presentation.zh-CN.md` |
| `Docs/Production/Tasks/TASK-015-Combat-Calculation-Contracts.md` | `Docs/Production/Tasks/TASK-015-Combat-Calculation-Contracts.zh-CN.md` |
| `Docs/Production/Tasks/TASK-019-Resource-and-Global-Ledgers.md` | `Docs/Production/Tasks/TASK-019-Resource-and-Global-Ledgers.zh-CN.md` |
| `Docs/Production/Tasks/TASK-020-Card-Catalog-and-Lifecycle.md` | `Docs/Production/Tasks/TASK-020-Card-Catalog-and-Lifecycle.zh-CN.md` |
| `Docs/Production/Tasks/TASK-022-Synthesis-Resolution.md` | `Docs/Production/Tasks/TASK-022-Synthesis-Resolution.zh-CN.md` |
| `Docs/Production/Tasks/TASK-024-Sandbox-Graph-and-Movement.md` | `Docs/Production/Tasks/TASK-024-Sandbox-Graph-and-Movement.zh-CN.md` |
| `Docs/Production/Tasks/TASK-025-Node-Operations-and-Story.md` | `Docs/Production/Tasks/TASK-025-Node-Operations-and-Story.zh-CN.md` |
| `Docs/Production/Tasks/TASK-027-Battle-Routing-and-Travel.md` | `Docs/Production/Tasks/TASK-027-Battle-Routing-and-Travel.zh-CN.md` |
| `Docs/Production/Tasks/TASK-033-First-Era-Content-Pack.md` | `Docs/Production/Tasks/TASK-033-First-Era-Content-Pack.zh-CN.md` |
| `Docs/Production/Tasks/TASK-034-Full-MVP-Integration.md` | `Docs/Production/Tasks/TASK-034-Full-MVP-Integration.zh-CN.md` |
| `Docs/Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.md` | `Docs/Production/Tasks/TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md` |
| `Docs/Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.md` | `Docs/Production/Tasks/TASK-037-Node-Inspection-and-Local-Scenes.zh-CN.md` |
| `Docs/Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.md` | `Docs/Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.zh-CN.md` |
| `Docs/Production/Tasks/TASK-039-Story-Relationships-and-Bonus-State.md` | `Docs/Production/Tasks/TASK-039-Story-Relationships-and-Bonus-State.zh-CN.md` |
| `Docs/ProjectState.md` | `Docs/ProjectState.zh-CN.md` |
| `Docs/Rules/WorldState.md` | `Docs/Rules/WorldState.zh-CN.md` |

## 2026-09-09 配置继续交付

D24／D25／D26 已回答：点击只查看、右侧独立移动卡牌、敌方地块全公开、FPS 留待未来、ACT 可玩。D29 确定三态与 A／附着 B 逐卡组校验，D03 允许 mycard.pdf 明确不冲突数值。[TASK-040](TASK-040-Model-Assets-and-Scene-Bindings.zh-CN.md) 新增独立 UE 模型资产／预览。TASK-012／014／020／035／037 现消费 040，以各自必要前置为准。040 直接使用既有 Verified 008／036 工作区结构，不推定集成授权。早先 HTML 预览仍仅为设计依据。
