# TASK-038 全局配置与增益界面

- 英文原文：TASK-038-Global-Preparation-and-Bonus-UI.md
- 文档 ID：TASK-038
- 版本：0.1
- 状态：Backlog
- 上级：[TASK-011](TASK-011-Full-MVP-Task-Cluster.zh-CN.md)
- Primary Role：UI / Input
- Primary Agent：Ready 时指定唯一写入者；尚未启动
- 分支／worktree：预检时指定 Local 分支；确需隔离时才用 worktree

## 目标

提供统一的玩家全局配置入口，组合权威服务提供的各模式出战配置、共享基础技能、关系／成长记录、增益来源及卡牌发现信息。

## 输入与就绪条件

阅读 AGENTS.md、Docs/00_INDEX.md、Docs/ProjectState.md、前置契约、[画面设计](../../Design/ZH/06_MapAndPreparation.md)、[MVP 决策](../MVPDecisionRegister.zh-CN.md)、ADR-0005、相关模块 README 和目录指令。英文是规范源，中文同步。

- 必要前置：[TASK-012](TASK-012-Run-State-and-Contracts.zh-CN.md), [TASK-019](TASK-019-Resource-and-Global-Ledgers.zh-CN.md), [TASK-020](TASK-020-Card-Catalog-and-Lifecycle.zh-CN.md), [TASK-035](TASK-035-ACT-Character-Cards-and-Roster.zh-CN.md), [TASK-039](TASK-039-Story-Relationships-and-Bonus-State.zh-CN.md)。
- 决策门禁：D20, D23, D26, D27, D28。未回答项不是默认值。
- 用户要求此能力；当前契约只规划它。编码前完成依赖、记录确切批准的行为／内容／数值、审查公共结构并进入 Ready。每个未接受数值都需逐项确认。
- Core 独立于表现；使用稳定身份和强类型命令／结果／视图边界。不宣称运行成功。

## 范围与允许路径

- `Source/DreamOfPadma/Public/MVP/UI/Preparation/`
- `Source/DreamOfPadma/Private/MVP/UI/Preparation/`
- `Source/DreamOfPadma/Private/Tests/MVP/TASK-038/`
- `Content/Padma/MVP/UI/Preparation/`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Architecture/Modules/PadmaUI/README.zh-CN.md`
- `Docs/Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.md`
- `Docs/Production/Tasks/TASK-038-Global-Preparation-and-Bonus-UI.zh-CN.md`

Coordinator 单独拥有共享 index／state／任务矩阵更新。Ready 前明确加入所需规范／ADR／公共契约路径，不推定宽泛租约。排除其他功能代码、配置／构建文件、.agents／.codex、生成目录和第三方内容。

## 范围外

直接修改世界／Actor、另一套装备／增益／图鉴权威、数值调整、NPC 成长规则、FPS 执行／配表、修改 ACT 子弹时间，以及开发者配表编辑器。

## 验收标准

- [ ] 地图和已批准局部画面可挂载一致的全局配置按钮；关闭通过强类型导航边界恢复此前画面／选择。
- [ ] ACT 通过 TASK-035 选择角色／武器，并经该服务读取 TASK-036 定义；不创建第二份阵容，也不直接编辑沙盘已部署回合制卡。
- [ ] FPS 使用独立能力／定义边界和 D26 批准的可用状态。不得把 ACT 表强转为 FPS 或把未实现模式标为可玩；可玩 FPS 需要独立契约。
- [ ] 共享基础技能身份／槽位提供 Encounter 与 ACT 效果视图，不合并两者消耗、时钟或执行。本页面独立于战斗中的 ACT Tab／子弹时间。
- [ ] 关系／成长和增益视图显示权威服务提供的来源、目标／卡类、模式／范围、条件、持续时间／时钟与当前资格；缺失或不可用数据服务保持明确状态。
- [ ] 遵守 D27 开启／修改／关闭／时间规则；浏览不会装备、授予增益或提交日历推进。校验修改返回结果／错误；全局预设与本局配置归属分离。
- [ ] 图鉴遵守卡牌服务批准的发现规则，包括原文中 C 卡首次成功前的信息边界；其他页签不能泄露隐藏定义。
- [ ] TASK-034 绑定真实导航宿主／服务；本任务用明确的合法／不可用样例检查边界，不编辑其他所有者的源码。
- [ ] 双语模块／任务报告、范围内验证及独立只读 Review 通过。

## 验证

只读开启／关闭与原画面恢复、拒绝或过期阵容修改、特性限制原因、缺失模式／服务、分模式技能视图、发现过滤及刷新不重复授予。最终集成观察地图与局部导航。

原生修改编译 Editor 目标；通过 Scripts/RunTests.ps1 运行 TASK038 窄自动化并核对实际用例结果。运行 Scripts/AuditDocs.ps1、适用的 ValidateProject.ps1 -Strict 和 git diff --check。记录可复现人工观察，不要求宽泛 UI 自动化套件。TASK-034 负责真实功能组合，集成失败交回功能所有者。

## 归属、学习与恢复

唯一 Primary 写入者，一条串行 UE 构建／Editor 通道；地图、资产、结构和模块 README 配对采用独占租约。有限 Architect／Reviewer 可以只读检查，不增加第二写入者。命令／事件与状态归属在场景重建和存档边界中保持，不使用 Actor 指针。

学习目标：跟踪设置按钮 → 命令 → 权威结果 → 刷新视图，指出哪些数据属于预设、哪些属于本局。 用户证据：Not started。实现交接提供一次预测和实际观察；Agent 产出不证明用户学习。

检查归属后仅回退本任务经 Review 的修改。不得删除生成／无关数据，未经明确授权不得合并／推送。

## 完成报告

- 状态：Backlog；本次为规划补充，未开始实现。
- 代码／资产、运行测试、用户观察及独立实现 Review：待完成。
- 英中配对同时创建。实现交接提供 Changelog 草稿、确切变更路径、证据和残余风险。

## 2026-09-09 已确认模型／移动交接

D26 已回答：ACT 首版可玩；FPS 仅为未来能力页面，不提供出战／技能执行。模型／表引用在 UE Data Asset 配置，不在玩家界面编辑。全局配置修改／时间／持久化和具体关系增益仍为 D27／D28。
