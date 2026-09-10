# TASK-046 原生 UE HTML 可玩切片

- 英文原文：TASK-046-Native-UE-HTML-Slice.md
- 文档 ID：TASK-046
- 版本：0.1
- 状态：Review - 原生实现候选，待用户试玩
- Primary：root；当前 Local codex/mvp-demo-foundations
- 授权：2026-09-09 用户要求将已验收 HTML 行为实现到原生 UE，参考 Combat，支持批量内容表并解释素材配置。用户明确允许 HTML 临时数值用于标注为 UE Demo 的内容。

## 结果与范围

将 HTML 0.4 已实现子集迁移为原生 UE 可玩切片：地图查看与卡牌移动、部署／合成／日历／资源、编队／对话、独立 Encounter 与 ACT GAS、基础技能交互、战斗回滚／提交和安全存读档。FPS 仅配置与目录；未实现 PDF 能力及好感 Buff 仍明确标注。Combat 只读参考行为，不引入其依赖或批量复制素材。

允许新增源码：Source/DreamOfPadma/{Public,Private}/MVP/{Core/Run,Core/Content,Game/Run,Game/Content,Game/Playable,Gameplay/Combat,Gameplay/Encounter,Gameplay/ACT/Runtime,World/Playable,UI/Playable}；测试在 Private/Tests/MVP/TASK-046。已有 ACT 配置／模型定义仅在需要时增加兼容的表现／运行字段。root 独占 DreamOfPadma.Build.cs、DreamOfPadma.uproject、Config/DefaultGame.ini 的烘焙／Cue 路径，以及 Config/DefaultEngine.ini 的 CommonUI viewport 配置项。保留原默认地图配置和地图。新建 Content/Padma/MVP/Playable 与 Scripts/Editor/{ExportPlayableData.cjs,AuthorPlayableContent.py,AuthorPlayableDemo.py}；生成时保留已有资产，仅追加文档列明的缺失生成绑定。文档：本配对、ADR-0006 配对、原生试玩／配置指南配对、索引／状态／更新记录及受影响架构／模块配对。

依赖：复用 TASK-007/008/036/040 工作区候选。本次授权的集成切片启用 TASK-012 至 039 中 HTML 子集相关契约，不宣称满足其中所有更广验收。已接受 HTML／用户选择覆盖旧通用门槛。具体所有权／加载／时钟见 ADR-0006。

## 工作包与证据

每个文件一个写入者。内容包负责类型化目录与测试内容生成输入；规则包负责中立本局值／命令和持久化适配；战斗包负责原生模式执行及临时战斗单位。root 负责接口接线、UI／世界组合、构建通道和文档。必需检查：Editor 编译、定向状态／目录／GAS／回滚自动化、不覆盖资产的生成、独立运行渲染及可复现试玩路径。交付前进行一次限定生命周期／存档审查。

不执行 Git 集成、提交、推送，不改第三方或引擎。源码与功能逐项对应及用户配置工作见 [NativePlayableDemo](../../Content/NativePlayableDemo.zh-CN.md)。

## 交付证据与限制

- UE 5.8.2 原生 Editor 编译成功。最终报告 `2026.09.09-08.20.49` 记录 11 项成功、零警告、零失败：8 项规则／状态、2 项目录、1 项 GAS 模式／生命周期。首轮 GAS Cue 扫描警告已消除。
- 生成了类型化表、软引用 DA、材质和 L_PadmaWorld／L_PadmaBattle。重复运行保留两张地图及已有字段，仅补齐守卫／攻击美术绑定；原生目录校验通过。
- `Artifacts/TASK-046/` 中有沙盘、归处、Encounter、ACT 的真实 D3D12 独立运行截图。两种战斗均经实际 OpenLevel 从沙盘进入。CommonUI 使用 All 输入模式，中文标签通过 Slate 字体回退渲染。截图证明启动／表现，不代表用户输入验收。
- 最后一次真实往返成功：`08.20.24.162 UTC` 沙盘就绪、`.543` 战斗就绪，`08.20.25.458` 退出／回滚后无待战事务，`.528` 再次回到沙盘。`roundtrip-render.log` 与 `roundtrip.png` 记录结果。非 Shipping 参数 `-PadmaDemoScenario=roundtrip -PadmaPlayableCapture=<path.png>` 可复现，调用原生命令而非模拟鼠标。
- `AuditDocs.ps1` 通过（220 个 Markdown／110 对），`ValidateProject.ps1 -Strict` 通过；限定源码／配置／文档空白检查通过。新指南作为可选学习／配置证据，不增加强制课程或复述要求。
- 限定独立审查发现的两项 P2（切场失败恢复、目标数量沿用）已修复，针对性源码复查 Conditional pass。世界销毁后注入切场失败，以及混合队伍／只剩一名敌人的手动选敌仍待检查；未重复全面审查。
- 证据在被忽略的本地产物中：`editor-build.log`、`authoring.log`、`Automation/index.json`、`*-render.log` 和截图。未执行打包版本测试或正式导入美术验证。
- 当前范围为已验收 HTML 子集；正式 ACT 连招、未实现 PDF 能力、等级成长、好感 buff 与 FPS 游玩未交付。对话现为文本／选项；Icon 数据接口与序列角色重绑定仍需后续 UI／表现工作。
- 用户试玩路线与操作检查见指南。UE 试玩前任务保持 Review；HTML 验收不自动视为 UE 验收。
