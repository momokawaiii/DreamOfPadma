# TASK-010 端到端可玩原型集成

- 英文原文（Agent 阅读）：`TASK-010-Playable-Prototype-Integration.md`
- 文档 ID：`TASK-010`
- 版本：`0.1`
- 状态：`Review`
- 上级里程碑或集成 Goal：`TASK-006 固定可玩原型垂直切片`
- Primary Role：`Integration Coordinator`
- Primary Agent：`Integration Coordinator`；不另设实现 Agent
- 分支/worktree：集成负责人 Local 工作区；不使用独立 Worktree
- 任务模式：串行集成、配置、证据和状态关闭；不在子任务写入集合内实现功能或修复缺陷

## Goal

把已 Review 的 TASK-007、TASK-008 和 TASK-009 集成为一条可重复的可玩路径。批准的默认地图打开 Demo Sandbox，用户选择 Demo 地块，进入对应 Encounter 场景，出 Demo 卡，看到角色召唤，并读取角色技能列表。任务留下编译、手工 PIE、文档和集成证据，供 GitHub 备份前 Review。

## 为什么现在做

三个子实现任务分别拥有 World、局部运行时以及 UI/数据路径。最后需要一个串行集成任务绑定默认入口、验证跨任务真实路径、拒绝范围漂移，并在不允许集成修复器暗中改变子任务规则的前提下记录用户证据。

## 首先阅读

- `AGENTS.md`
- `Docs/00_INDEX.md`
- `Docs/ProjectState.md`
- `Docs/Agent/Workflow.md`
- `Docs/Production/TaskTemplate.md`
- `Docs/Production/Tasks/TASK-006-Playable-Prototype-Cluster.md`
- `Docs/Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.md`
- `Docs/Production/Tasks/TASK-008-Encounter-Character-and-Skill-Data.md`
- `Docs/Production/Tasks/TASK-009-Card-Play-and-Character-Summon.md`
- `Docs/Decisions/ADR-0003-Playable-Prototype-Vertical-Slice.md`
- `Docs/Architecture/ProgramArchitecture.md`
- `Docs/Architecture/RuntimeFlow.md`
- `Docs/Architecture/Modules/PadmaGame/README.md`
- `Docs/Architecture/Modules/PadmaWorld/README.md`
- `Docs/Architecture/Modules/PadmaGameplay/README.md`
- `Docs/Architecture/Modules/PadmaUI/README.md`
- `Docs/Production/BuildMatrix.md`
- 只读参考 `E:\2026ue\padma`

## 决策状态

### 已冻结输入

- TASK-007、TASK-008 和 TASK-009 必须各自完成独立 Review 后才能集成。
- 最终路径是固定 Demo fixture，不是通用 SLG 路线、完整战斗或正式卡牌循环。
- 只有通过明确允许的中心配置修改，才能把默认地图改为项目自有 Demo Sandbox 地图。
- 手工 PIE 验收是端到端主要门槛；Debug 面板和大规模 UI 自动化不是前置条件。
- 用户是可见流程的最终验收者；Coordinator 必须把用户观察与 Agent 证据分开记录。

### 延期或开放输入

- 所有 Open/Proposed/Deferred/playtest-only 的玩法、路线、卡牌、资源、随机、存档、ACT、GAS、导入、ID、World Partition 和资源迁移决定保持不变。
- 子任务源代码或资源的任何缺陷必须返回所属 TASK；本集成任务不得通过扩大写入集合来修复。

## 范围

### 允许路径和写入集合

Integration Coordinator 只能修改：

- `Config/DefaultEngine.ini`：只设置本切片所需的 Demo Sandbox 默认地图项
- `Docs/Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.md`
- `Docs/Production/Tasks/TASK-007-SLG-World-Selection-and-Transition.zh-CN.md`
- `Docs/Production/Tasks/TASK-008-Encounter-Character-and-Skill-Data.md`
- `Docs/Production/Tasks/TASK-008-Encounter-Character-and-Skill-Data.zh-CN.md`
- `Docs/Production/Tasks/TASK-009-Card-Play-and-Character-Summon.md`
- `Docs/Production/Tasks/TASK-009-Card-Play-and-Character-Summon.zh-CN.md`
- `Docs/Production/Tasks/TASK-010-Playable-Prototype-Integration.md`
- `Docs/Production/Tasks/TASK-010-Playable-Prototype-Integration.zh-CN.md`
- `Docs/00_INDEX.md`
- `Docs/00_INDEX.zh-CN.md`
- `Docs/ProjectState.md`
- `Docs/ProjectState.zh-CN.md`

子 TASK 文件只能接收完成证据和状态更新；集成期间不得重写其 Goal、范围或验收边界。

### 不在范围内

- 修改子任务源代码、Content 资源、地图、模块 Build.cs 或 UI Widget 来修复集成失败。
- 添加通用路线解析器、战斗、技能执行、随机、资源、存档、ACT、GAS、Debug 面板、打包或资源迁移。
- 没有独立 Review、必需检查和用户 PIE 证据就把任务标记为 `Done`。
- 在本任务中合并、打标签、推送或创建 GitHub Release。

## 依赖与集成顺序

- 必须先完成：TASK-006 集群/ADR-0003 获批；TASK-007、TASK-008 和 TASK-009 已实现并独立 Review，且无阻塞问题。
- 集成顺序：世界选择/上下文 -> Encounter 运行时/角色数据 -> 卡牌/UI -> 默认地图配置 -> 端到端 PIE。
- TASK-005 保持 `Review`，本任务不修改它。
- 如果项目设置或构建依赖与子任务结果冲突，必须停止并请求新的有边界架构/配置任务。
- 最终手工验收期间不得启动第二条 UE Editor/PIE 通道。

### 共享冲突

本任务拥有 `Config/DefaultEngine.ini` 和共享文档状态的串行修改权，不拥有子任务源代码或二进制资源目录。任何子任务验收失败都应返回所属任务，而不是在这里打补丁。

## 委派计划

不授权可写 subagent。可以让独立只读 Reviewer 检查集成 diff 和证据：

| Role | 工作包 | 权限 | 预期输出 |
|---|---|---|---|
| `reviewer` | 检查集成 diff、默认地图修改、端到端证据、任务状态和 Open/Deferred 保留。 | 只读 | P0-P3 问题及 `Pass`/`Block`。 |
| `learning-tutor` | 检查用户手工路径和复述是否与 Agent 证据分开记录。 | 只读 | 学习证据 Review。 |

## 验收标准

- [ ] 通过批准的默认入口启动项目时，进入项目自有 Demo Sandbox 地图。
- [ ] 一次 PIE 会话完成完整路径：选择 Demo SLG 地块 -> 进入对应 Encounter 场景 -> 看到一张卡 -> 出牌 -> 看到一个被召唤角色 -> 选择角色 -> 读取技能列表。
- [ ] 场景切换携带预期稳定 Demo 上下文，不依赖失效 Actor 指针。
- [ ] 卡牌和技能 UI 反映权威结果/事件或只读视图数据，不包含平行的召唤或技能实现。
- [ ] 重复出牌或无效 fixture 会给出可读失败，不产生非预期重复角色或部分状态。
- [ ] 编译、必需文档检查、子任务独立 Review 和手工 PIE 证据都已记录。
- [ ] TASK-005 保持不变，所有 Open/Proposed/Deferred/playtest-only 决定继续明确未决。
- [ ] 用户亲自确认可见路径后，子任务和本集成任务才能关闭。

## 验证方案

### 自动检查

- 在批准的单一构建通道编译 UE Editor Target。
- `pwsh -NoProfile -File .\Scripts\AuditDocs.ps1`。
- `pwsh -NoProfile -File .\Scripts\ValidateProject.ps1 -Strict`。
- `git diff --check` 和精确变更路径/配置审计。
- 不要求大规模 UI 自动化。只有子任务已经提供且不扩张范围时，才消费窄范围服务测试。

### 手工检查

1. 在 PIE 中启动默认地图。
2. 选择并确认 Demo SLG 地块。
3. 验证对应 Encounter 场景和保留的节点/场景/出生点上下文。
4. 观察一张 Demo 卡。
5. 出牌并验证在配置出生点恰好出现一个角色。
6. 选择角色并验证配置技能列表。
7. 尝试重复/无效出牌，验证可读失败且没有额外角色。
8. 记录用户验收观察以及截图或短视频。

### 必需证据

- UE Editor 编译输出。
- 覆盖每个步骤的手工 PIE 截图/视频。
- TASK-007 至 TASK-009 及本集成 diff 的独立 Review 报告。
- 精确变更路径、默认地图 diff、文档审计和最终 ProjectState/任务状态。
- 分开记录 Agent 产生和用户产生的学习证据。

## 学习契约

| 目标 | 所需证据 | 状态 |
|---|---|---|
| 端到端所有权追踪 | 用户能说出地块选择、切场上下文、Encounter 状态、召唤结果和技能视图分别由谁负责。 | `Not started` |
| 基于证据的集成 | 用户亲自走完整路径，记录一个预期观察和一个失败观察，并解释集成任务为什么不能暗中修复子任务范围。 | `Not started` |

- 学习者亲手练习：完成整条 PIE 路径，并记下每次场景边界首先观察到的稳定身份。
- 迁移问题：如果第二个地块无法进入场景，应该修改集成任务、World 任务还是切场契约？哪些证据决定 Owner？

## 风险与恢复

- 默认地图修改可能破坏已经验收的基础地图。恢复：保留 Git 历史中的原地图值；Demo 地图无法启动时只回退明确的配置行。
- 集成可能通过一次性蓝图修复掩盖子任务缺陷。恢复：停止，将缺陷返回所属 TASK，并保留原始证据。
- 参考项目资源可能在其他检出环境缺少依赖。恢复：使用项目自有占位资源，把迁移记录为未来任务。
- 安全回退是回退集成/配置/文档提交；不得删除子任务资源或生成目录。

## 完成报告

- 最终状态：`Review`；契约已准备，尚未集成。
- Primary Agent 和 Role：Integration Coordinator / Integration Coordinator。
- 修改文件：尚无；集成路径已在上方列明。
- 验收证据：等待子任务 Review、编译和用户 PIE 验收。
- 已运行检查及结果：等待集成。
- 未运行检查及原因：本契约处于 Review 时不授权集成或 PIE 工作。
- 已解决或接受的 Review 问题：等待。
- 剩余风险和未决问题：所有延期输入按设计保持未决。
- 已更新的中英文文档：契约配对已准备。
- Agent 产生的学习证据：等待。
- 用户产生的学习证据：等待。
- 集成提交或交接引用：等待用户批准及后续 GitHub 备份；不推送。
