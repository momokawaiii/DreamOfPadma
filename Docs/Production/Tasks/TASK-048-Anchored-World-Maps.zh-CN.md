# TASK-048 剧情锚点地图与旧链路退役

- 英文原文：TASK-048-Anchored-World-Maps.md
- 状态：Verified 实现候选；用户交互验收待进行
- Primary：root；当前 Local codex/mvp-demo-foundations
- 授权：退役旧 Demo 资产／代码／测试，完善三维地图，开放地图定义／编辑，加入种子生成和 PCG；保留配表指定的锚点身份与坐标。

## 范围与验收

一次完整地图交付。保留无关工作区修改及当前 Encounter／ACT 行为。root 负责集成、源码／配置／构建、资产迁移、脚本、共享文档及验证；有界工作包分别负责地图定义／生成、世界表现／PCG、旧链路退役。UE 编译和资产操作由 root 串行执行。保留第三方内容和历史证据，不暂存／提交／合并／推送。

允许路径：Source/DreamOfPadma 下 Core／Game／World／Presentation／UI 及相关旧代码／测试；Config 启动／打包配置；DreamOfPadma.uproject 和 Build.cs 中 PCG 依赖；项目地图／模型预览资产；Scripts/Editor 退役／地图创作脚本；相关双语 Docs。旧资产须精确备份后经 UE 删除，并验证引用；不删除无关内容。

验收：预览迁移后退役旧 Demo 地图／定义／运行链路，新地图默认启动。场景支持镜头导航、悬浮／选中／路径反馈及地形／建筑／驻军视觉配置。手工定义和生成地图采用稳定 ID、显式边、可编辑数据与校验，保留固定锚点。种子可复现，PCG 只生成环境装饰，不改变通行拓扑。本局／存档／回滚使用同一份固定地图。测试覆盖合法绕路分支，拒绝非法生成图、不可能满足的道路前置和不兼容存档。完成 Editor 编译、定向自动化、真实资产／PCG 渲染与地图战斗往返、文档审计和严格项目验证；交互由用户试玩。

已确认：FixedNPC 固定出现在指定区域／位置；MainStory 人物的相遇／完成记录影响后续相关剧情。配表检查点在抵达时一次性检查 Seen／Completed，固定命中或错过的标记和对话。可以绕过前面的节点，不提前判定错过，不增加全锚点通行或胜利门槛。对话沿用既有晌午操作规则，抵达记录独立处理。迁移前已确认编辑器关闭。

## 证据

地图创作工具已放在 `/Game/Padma/World/Maps`，首区域默认种子 12345（7 个手工节点、2 个生成支路节点、13 条边），默认运行地图为 `L_PadmaWorld`。6 个旧资产、21 个旧源码文件（含 2 份旧测试文件／3 项测试）及 `AuthorDemoEncounter.py` 已退役；精确备份位于 `Artifacts/TASK-048/legacy-backup`。预览目录／地图已改用通用坐镇模型，保留当前 TASK-040、TASK-046 和命名／存档兼容测试。UE 删除后留下五个磁盘残留，经与备份核对哈希一致后清理；可重复执行的退役脚本现同时检查磁盘与注册表。

| 文件／目录 | 职责 |
| --- | --- |
| `Core/Content/PadmaContentTypes.*`、`Core/WorldMap/PadmaWorldMapDefinition.*` | 地图／锚点／检查点值契约，确定性支路生成，图／前置条件校验与签名 |
| `Core/Run/PadmaRunRules.*`、新增 `PadmaRunStory.cpp` | 冻结本局拓扑、NPC 相遇／完成、一次性检查点、兼容且原子的恢复／回滚 |
| `Game/WorldMap/PadmaWorldMapAsset.*`、`PadmaMapAuthoringPreview.*` | 可编辑资产、检查点表显式导入、生成／校验按钮、临时场景手柄 |
| `Game/Content/PadmaContentCatalog.*`、`Game/Run/PadmaRunSubsystem.*` | 地图／主题加载、保留旧图、按配置原子创建新局 |
| `Game/Framework/PadmaPlayerController*`、`UI/Screens/PadmaGameScreen.*` | 镜头／三维查看、驻军与手牌布局、种子确认、提交命令 |
| `World/Map/PadmaWorldMapActor.*`、`PadmaMapVisualTheme.*`、`World/PCG/PadmaDecorationPoints.*` | 缓存立体地表、模型／反馈、独立环境点与真实 PCG 静态网格生成 |
| `Game/Presentation/PadmaPresentationCatalog.*`、TASK-040 模型测试 | 移除退役角色适配，保留其他枚举显式值与通用预览 |
| `Scripts/Editor/AuthorWorldMaps.py`、`RetireLegacyDemo.py`、`AuthorModelPreview.py`、`ExportPlayableData.cjs` | 创建／保留地图工具，精确退役与残留审计，通用预览，旧导出字段默认兼容 |
| `Tests/WorldMap/*`、`.uproject`、`Build.cs`、`Config/DefaultGame.ini`、现有默认地图配置 | 地图／本局测试，内置 PCG 依赖与世界内容烘焙；保留用户当前 L_PadmaWorld 启动选择 |

除明确写出的脚本／配置外，上表路径均相对于 `Source/DreamOfPadma/{Public,Private}`。分层使规则值独立于 PCG／Actor／UI，将加载与编辑分开，并允许替换美术。本次无需万能管理器或另建项目插件。[创作指南](../../Content/WorldMapAuthoring.zh-CN.md) 列出素材资产与操作步骤；[ADR-0007](../../Decisions/ADR-0007-Anchored-Map-Generation.zh-CN.md) 记录冻结布局的存档修订。

## 验证与边界

- 源码修正后，UE 5.8.2 `DreamOfPadmaEditor Win64 Development` 编译通过，当前没有已知新增编译／反射错误。
- 22 项定向自动化通过：8 项地图测试、11 项保留的 TASK-046、2 项 TASK-040、1 项 TASK-047 兼容测试。证据：`Artifacts/TASK-048/Tests/index.json`、`automation.log`。此前测试构造中的 TArray 自引用、旧 JSON 缺少新增字段默认值均已修正后重跑通过。
- 创作工具可校验／生成；场景预览创建 7 个手柄，并能清空全部临时地图／节点／手柄 Actor。通用预览的坐镇、建筑、地面均为 `Visual=1`，不再需要旧角色类型。
- 真实原生渲染：`WorldMap.png`（1920×1080）、`render-world.log`、`render-roundtrip.log`。地图 → Encounter → 回滚 → 地图通过；返回地图生成 200 个 PCG 实例，`layout-build=1`，未观察到运行错误。首轮截图发现的三角形绕序、本地化文本导入、材质输出／实例用途问题已修正。几何缓存与悬停／选中状态分开。
- 新 UE 进程的最终 `assets-final.log` 通过：重复创作保留工具资产，三种通用模型预览均成功，注册表和磁盘均无旧资产；核对无文件后删除旧空目录。`EditorBuild.log` 保存成功编译记录。
- 独立定向复核依据 22/22 报告关闭五项地图／存档问题，结论 **Pass**。`AuditDocs.ps1` 通过（230 份 Markdown／115 对），`ValidateProject.ps1 -Strict` 通过，限定范围的 `git diff --check` 通过。生成证据保持忽略；没有暂存／提交／合并／推送。

交互仍由用户试玩。美术为可配置的低多边形占位；截图来自真实 UE，不代表正式画面目标。本次是小型手工图加种子支路，不包含自由邻接、完整自定义图编辑插件、大世界地形雕刻或竞技对称／公平保证。支路间距仍需视觉创作检查。完整剧情内容与打包兼容性尚未验证；NPC 分支文本和既有数值属于临时测试内容。
