# TASK-042 HTML 可玩规则原型

- 英文配对：TASK-042-HTML-Playable-Prototype.md
- 文档 ID：TASK-042
- 版本：0.2
- 状态：Review
- Primary：root，负责集成与 UI；当前 Local 分支。一名有限范围 worker 负责 data.js、engine.js、engine.test.cjs；只读架构 Agent 提供 UE 建议。
- 授权：2026-09-09 用户纠正顺序：先开发可玩的 HTML 原型对齐规则，再继续 UE 制作。仅 HTML 可使用可修改的临时数值。

## 目标

交付离线浏览器游戏，接通部署、每日经营、合成、显式节点移动、Encounter／ACT、回滚和胜利流程。保留现有 UE 候选成果，暂停继续开发 UE。卡牌原始数值保留页码来源；部分实现和原型提案必须可见。

## 范围与归属

允许修改：`Prototypes/HTML/`；本任务中英配对；`Docs/Production/MVPDecisionRegister` 配对；`Docs/Rules/Synthesis`、`Time`、`Combat` 配对；`Docs/00_INDEX`、`ProjectState`、`Changelog` 配对；`Docs/Production/CurrentBuildTestPlan` 配对中的入口链接与覆盖说明。root 负责集成和文档，保留共享文档已有变更。不修改 Source、Content、Config、UE 生成目录，不暂存或集成 Git 变更。

## 已确认决策与门槛

- 点击仅查看；显式“移动到此处”移动卡牌。A 与附着 B 逐张校验后原子移动。敌方地块内容全部公开。
- 合成参与的 A／B 无论成功失败都进入弃牌堆，失败没有奖励。每天进入晌午时，A 倾向向附着 B 累加一次。成功的 C 直接生成在 A／B 当前所在节点，不进入手牌，不再收取部署费用（用户纠正）。
- Encounter 和 ACT 分别执行。ACT 阵容／武器独立于地图 ABC；基础技能共享身份与槽位，分别配置模式效果。
- 战斗冻结日历，成功提交，失败／退出恢复完整入战前本局状态。
- 缺失的 HTML 数值可以临时拟定；正式 UE 数值和未定规则含义仍未确认。用户已接受独立基础技能弃牌堆、耗尽洗回共享槽位、手动 Encounter／ACT 试玩入口。真伤不受 C 卡克制影响；物理伤害受影响。

## 当前 HTML 补充

双击归处打开完整编队界面，含“节点细节”“主角展示”“其他模式”。坐镇归处的主角与招募的 ACT／FPS 手下使用独立身份。可切换服装、武器种类与兼容姿势；FPS 仅预览选角。原地块场景查看保持独立。熔炉与守望人操作进入有台词和选项的对话；完成时才发放一次结果，取消不发放。新插画／台词明确为原型占位内容，不是已接受剧情或正式卡牌技能。提供双语试玩指南与 UE CommonUI／展示场景／剧情架构建议，不写 C++，不更改引擎依赖。

## 验收与检查

- 实际浏览器入口和简短默认游玩路线，具备真实操作。
- 可修改临时参数，明确来源与实现覆盖。
- 独立命令／状态层、稳定 ID、带种子结果和安全边界保存／继续。
- Node 检查日历边界、合成／原料流转、移动原子性、模式隔离、行动条／用牌限制、ACT 子弹时间和完整回滚。
- 浏览器实际检查部署／合成／移动及两种战斗；不声称执行了 UE／GAS。
- 双语文档审计、严格项目验证和范围内空白检查。

## 证据

初版 34 项引擎测试通过。浏览器自动化拒绝访问本地文件 URL，未尝试替代浏览器访问；用户已确认 index.html 能打开并进入地图。当前归处／对话／合成修正证据记录如下。不要求学习测评。

## 完成报告——v0.2 候选

- 已实现：离线游戏、归处双击／三栏目、独立主角服装／武器／姿势预览与 ACT／FPS 选角、选项驱动的熔炉／守望人对话，以及 C 在原料节点生成且不二次收部署费。[试玩指南与 UE 建议](../../../Prototypes/HTML/README.zh-CN.md)。
- 代码路径：`Prototypes/HTML/index.html`、`data.js`、`engine.js`、`engine.test.cjs`、`app.js`、`home-ui.js`、`style.css`、`home.css` 及 README 配对。root 负责 UI／文档；html_state_amendment 负责三份状态／数据／测试文件。保留已有无关工作区变更。
- 文档配对：本任务、MVPDecisionRegister、Synthesis、Time、Combat、00_INDEX、ProjectState、Changelog、CurrentBuildTestPlan 和 Prototypes/HTML/README。共享状态已明确 HTML 优先，本轮不继续开发 UE。
- 状态验证：47/47 项引擎用例，包括 13 项新增合成／对话／身份／迁移检查。独立 reviewer 重跑用例、检查 42 组纯模板组合和 JS 语法，未发现 P0–P2。
- Review：独立 html_ui_review 结论 **Conditional pass**。浏览器自动化被本地文件 URL 策略拒绝，因此原生双击、新布局与浏览器游玩仍是手动验收条件。用户只确认了初版进入地图，不扩大为新增页面验收。纯模板执行不是浏览器证据。
- 模板证据：6 次归处渲染（3 栏目 × 2 模式选择）、30 次主角 SVG 渲染（2 角色 × 3 服装 × 5 组兼容武器／姿势）及 6 个剧情台词节点；检查模板执行／标题／undefined 值／SVG 结构，不检查实际布局。
- 仓库验证：AuditDocs 通过（206 份 Markdown／103 对）；ValidateProject -Strict 通过；已跟踪文件的范围内 diff 检查及未跟踪 HTML／任务的空白检查通过。JavaScript 语法检查通过。没有更改 UE 文件，因此无需重新构建 UE 运行时。
- 剩余：用户视觉／游玩验收、正式美术／剧情，以及明确列出的未实现 PDF 系统。UE CommonUI／舞台／剧情建议没有实施，也不是已接受 ADR。本轮没有新增 C++、UE 地图／资源、引擎依赖，未提交、合并或推送。
