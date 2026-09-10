# TASK-053 基于 Grill 的技术文档整理

- 文档 ID：TASK-053
- 版本：1.0
- 英文原文：[TASK-053-Grill-Technical-Docs.md](TASK-053-Grill-Technical-Docs.md)
- 状态：Verified
- Primary / Local 工作区：Codex Primary；E:/2026ue/DreamOfPadma，codex/mvp-demo-foundations
- 用户授权：2026-09-10 要求根据链接中的 grill 结论细化并精简现有技术文档

## 结果与准确范围

收敛技术归属，区分当前代码、已接受离线 Demo 目标和暂缓/待定设计。使用 padma-task-runner；结构性目标契约写入 ADR-0010。不修改源码、配置、UE 资产、存档格式或生成目录，不执行 Git 集成或远程操作。

下列英文路径及同目录 .zh-CN.md 配对为全部文档写集；证据仅写 Artifacts/TASK-053，保留既有工作区修改。

```text
Docs/Architecture/ProgramArchitecture.md
Docs/Architecture/DataDrivenArchitecture.md
Docs/Architecture/ModuleMap.md
Docs/Architecture/RuntimeFlow.md
Docs/Architecture/SaveSchema.md
Docs/Architecture/Modules/PadmaCore/README.md
Docs/Architecture/Modules/PadmaGame/README.md
Docs/Architecture/Modules/PadmaWorld/README.md
Docs/Architecture/Modules/PadmaEditor/README.md
Docs/Content/WorldMapAuthoring.md
Docs/Production/Roadmap.md
Docs/Production/BuildMatrix.md
Docs/Production/MVPDecisionRegister.md
Docs/ProjectState.md
Docs/00_INDEX.md
Docs/Changelog.md
Docs/Rules/ChapterZero.md
Docs/Decisions/ADR-0010-Offline-Demo-Content-and-Map.md
Docs/Production/Tasks/TASK-053-Grill-Technical-Docs.md
```

## 来源与决策核对

来源：[Skills Testing](codex://threads/01a06a4e-a758-72a2-abf6-985bd08d6ae1)，2026-09-10 用户回答及对应问题。应用读取返回空消息项，已从匹配的本地会话记录读取准确问题/回答及最终总览。早期助手建议不单独构成授权。

| 证据 | 采用含义 / 主文档 |
|---|---|
| Q1、Q4–Q8 | 可安装、发行质量的离线 Demo，两种构建配置，不接 Steam；ChapterZero / Roadmap / BuildMatrix |
| Q11、Q14、Q15 | 真实黄金路径、不同节点先 Encounter 后一个 ACT、王庭结尾、最终生产值；ChapterZero |
| Q12、Q16 → Q21 | 固定锚点/边/Seed，后续编辑器烘焙决定覆盖早期运行时混合方案；WorldMapAuthoring |
| Q17–Q24、Q28–Q29 | 章节/数据分离、正交分类、MapKey/不可变清单、独立剧情版本；DataDrivenArchitecture / SaveSchema |
| Q25、Q30 → Q33 | 阶段首次加载地图分配剧情并成为存档状态，读档不重抽；ChapterZero / RuntimeFlow |
| Q26、Q31 | 区分状态与 TutorialRunId；跳过可继续，主动重玩可再获奖，同次重试不重复；RuntimeFlow / SaveSchema |
| Q34、Q36–Q38 | 检查点一次锁定、固定注册表、对白表/文本表、纯表现 Cue；数据/运行文档 |
| Q19、Q23、Q27–Q28、Q32、Q35；08:52:50 UTC 暂缓决定 | 保留受限 Slate/UEdGraph 源到运行时设计，但编辑器/编译/制作 UI 延期；PadmaEditor |
| Q39–Q45 | 未回答，保留提案身份，包括特殊节点直接派发选择、参数编码、教程事件和完整编译门禁 |
| 当前代码/ADR | 单运行模块、同步目录、冻结布局/存档封装、模式隔离及可激活 UI 仍为当前状态；不宣称迁移 |

已核对 Core/Content、Core/Run、Core/WorldMap、Game/Content、Game/Save、Build.cs 及 ADR-0004/0007/0008/0009 的当前依据，不断言外部引擎规范或 Civ6 算法正确性。

## 验收

- [x] 每个已接受 grill 决策有归属，后续回答覆盖早期选项。
- [x] 删除中央文档重复规则/模块/待办表述，保留规范与历史来源链接。
- [x] 明确地图/数据/状态生命周期、剧情分配及教程奖励/恢复要求，不冒充已实现。
- [x] Q39–Q45 和未提供最终值仍待定，剧情编辑器暂缓。
- [x] 中英文一致性、相对链接和范围内空白检查。
- [x] AuditDocs 与适用项目验证。
- [x] 按 Workflow 对公开/存档/确定性目标契约做一次独立只读评审。

## 证据与剩余工作

任务开始副本在 Artifacts/TASK-053/before/*.txt；准确路径和初始 Git 状态为 allowed-paths.json、status-before.txt。该基线包含先前未提交编辑，不以 HEAD 差异替代本轮差异。

验证完成：

- AuditDocs 通过：290 个 Markdown / 145 对语言文件，见 audit-docs.log。
- ValidateProject -Strict 通过，见 validate-project.log。
- 全部 38 个修改文档通过本地链接、配对 ID/版本、围栏及相对任务开始差异的空白检查，见 check-docs.cjs 和 doc-checks.json。
- 独立只读评审 docs_053_review：Pass，无 P0–P3 问题；review.json 记录范围与限制。
- 五组中央架构文档由 1,644 行减至 636 行（减少 61%）；完整写集的开始/当前体量见 doc-checks.json。
- 原始问题/回答出处保存在 grill-evidence.json。未观察到文档写集之外的 Git 状态变化，既有编辑保留。

本交付仅改文档，不重跑运行时构建/PIE/打包。实际 Demo 仍需 Roadmap 中的实现/内容/用户验收，文档完成不证明这些结果。

Commit/merge/push：未执行，未获授权。
