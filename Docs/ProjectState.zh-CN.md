# 项目状态

- 英文原文：ProjectState.md
- 更新：2026-09-10；只放当前事实，操作前核对 Git 与受影响文件。
- 工作区：E:/2026ue/DreamOfPadma；当前 Local 分支 main，MVP 基线为 00b894d。后续 Demo 重写在独立 Local 分支进行。

## 当前优先级

下一产品交付是可安装的离线第零章 Demo：实际操作核心玩法，在不同节点先 Encounter 后唯一 ACT，最终占领王庭结束。Development 与 Shipping 均需测试；最终路径数值来自获批生产数据，卡牌文档待提供。见 [ChapterZero](Rules/ChapterZero.zh-CN.md) 和 [Roadmap](Production/Roadmap.zh-CN.md)。

[TASK-053](Production/Tasks/TASK-053-Grill-Technical-Docs.zh-CN.md) 将 grill 决策收敛到简洁的技术主文档，文档检查与聚焦独立评审已通过。目标新增编辑器烘焙地图/清单身份、章节运行时、存档阶段剧情分配，以及教程跳过/重玩奖励身份；本次文档修改尚未实现这些功能。Slate 剧情编辑器暂缓，可继续使用当前资产/表流程。

当前原生代码仍是 TASK-052 已验证工作区候选：55 格绘景教程、单击固定详情、A/B 确认、纵向羊皮纸手牌、地图导航、液体战争 UI 和 PIE 退出修复。独立连续地形/PCG 样板仍可用。用户美术/实体长按验收和打包行为未验证，证据保留在各 TASK。

当前 MVP 快照已于 2026-09-10 提交为 `00b894d`，并快进合入本地 `main`。后续 Demo 重写以此为基线，在新分支中进行。

用户自述已学习蓝图、DataAsset/DataTable/软引用、UMG/CommonUI 基础及 GAS。仅按需教学，不重复入门。

## 交付状态

| 范围 | 当前结果 |
|---|---|
| UE TASK-052 | 已验证的工作区候选：单击固定详情、显式 A／B 部署、五列纵向羊皮纸手牌、55 格新教程、单一所有者绘景输入与 50:50 液体动态 UI；构建、针对性回归及原生证据见 TASK-052 |
| UE TASK-051 | 固定教程绘景与 GPU 氛围保留；版本 1 的 162 格存档与 TASK-052 新网格继续兼容。此前检查保留在 TASK-051 |
| UE TASK-007／008 | 旧固定切场／召唤链路已于 TASK-048 退役；旧资产、源码与夹具测试移除 |
| UE TASK-036／040 | 保留 ACT 配置模板和通用模型目录／预览；坐镇预览已迁移到通用模型资产 |
| HTML TASK-042／043／044 | 迭代后 HTML 0.4 已获用户确认；旧报告中待手动验收文字属于历史 |
| UE TASK-046 | 原生工作区候选：日历／资源／ABC／合成／图地图／编队／对话、Encounter 与 ACT GAS、战斗事务及安全存档；Editor 编译和 11 项定向测试通过，地图／归处／双模式进入的真实渲染通过 |
| UE TASK-047 | 运行命名已迁移；Editor 编译及 12 项定向测试通过，包含旧存档读取。18 张表内容保留，原生类型标签与两张地图引用已重存；真实沙盘／战斗／回滚／沙盘路线通过 |
| UE TASK-048 | 工作区候选：地图编辑工具、固定锚点、抵达检查点、种子支路、三维镜头／地形／道路／模型与 PCG；Editor 编译及 22 项定向测试通过，PCG 已生成 200 个实例；用户交互验收待进行 |
| UE TASK-049／050 | 参考图风格策略 HUD、连续地形样板、500 个地表过滤 PCG 实例、7 个样板模型／18 个材质／3 个地标定义及 2 张 UI 皮肤贴图。真实激活栈、嵌套返回／焦点与只读实例／战斗详情；26 项定向测试通过，用户试玩／美术验收待进行 |
| 已验收 HTML 之外 | 正式 ACT 动作／连招、未覆盖 PDF 能力、好感 buff、FPS 游玩及正式美术仍待完成 |
| 工作流 TASK-045 | Verified：精简入口、有界委派、按风险审查和可选教学 |

当前交互、动态与网格兼容性证据见 TASK-052；已接受玩法证据仍在 TASK-046，命名兼容性见 TASK-047。用户美术／实体长按验收和打包版本兼容性仍未验证。此前证据保留在原任务。可玩 MVP 快照已集成到本地 `main`，本次交付负责同步远程。

## 当前边界

- HTML 提供已实现行为的验收基准，不证明整份 PDF 已实现。规则含义以 Docs/Rules 与最新用户决定为准；正式 ACT 内容、成长、好感 buff 缺项保持明确。
- Encounter／ACT 使用独立 GAS 执行／配置与角色集合。基础技能共享身份／槽位，效果按模式配置。ACT 装备来自战斗总设置；地形限制必须指明对象。
- 普通 SLG 地图在六边形外观上使用手工有向节点／连线；固定绘景教程明确生成六边格邻接边，并使用七个绘景专属关键点坐标。新版本 3 网格为 55 格，薪火台有 1 名敌方守军，已存档版本 1 网格保留 162 格；不可通行格和原道路锁定继续有效。SaveGame 根据可信配置冻结布局及变化／剧情状态。单击固定查看，移动到此处移动已部署卡牌。A／B 部署捕获目标，确认时重新校验真实 Core 命令。敌情全公开。
- A 与附着 B 逐卡校验、整组原子移动；元数据分可移动／不可移动／条件移动。
- 合成原料成败都弃牌，成功 C 在当前点位登场；B 于晌午累加 A 倾向；共享基础卡独立弃牌／洗回。
- Encounter 普攻先进入选目标并可取消。基础手牌松手预览、确认释放，以卡牌／环境为来源；真伤不受 C 克制，基础物伤受克制。
- 局部战斗冻结沙盘时间；胜利提交，失败／退出恢复完整快照。当前两种胜利为统治者核心与战局天平。
- UI 页面与模态覆盖层使用独立激活栈，只有顶层覆盖层接收输入，详情不结算玩法。绘景 Slate 经 CommonUI 页面拥有导航输入，处理点击／中键拖动／滚轮；取消会清空自身按键与捕获。控制器不重复轮询绘景 WASD，旧三维相机仍由控制器负责。遭遇战详情使敌方自动行动等待，已提交动作可以完成；ACT 详情不改变自身时钟或 Tab 策略。地形和 PCG 属于装饰表现，图结构仍决定玩法。
- 当前原型可使用占位及标明临时性的 UE Demo 数值；明确无冲突的 mycard 值保留出处。最终离线 Demo 路径按 ChapterZero 使用获批生产值/资产；缺失卡牌/奖励输入仍待定。
- UE 构建／Editor／PIE 单通道；沿用 Local 并保留无关修改。任务状态不代表 Git 集成或远端操作授权。

## 下一步入口

- 原生 UE 试玩路线、逐文件功能与素材／配表指南：[NativePlayableDemo](Content/NativePlayableDemo.zh-CN.md)、[地图创作指南](Content/WorldMapAuthoring.zh-CN.md)、[策略表现指南](Content/StrategyPresentation.zh-CN.md)。默认地图为 `/Game/Padma/MVP/Playable/Maps/L_PadmaWorld`；在 `/Game/Padma/World/Maps/Editing/L_MapAuthoring` 编辑节点位置。
- 流程与简短委派：[Agent 工作流](Agent/Workflow.zh-CN.md)。
- UE 模型／ACT 配置：[ModelAuthoring](Content/ModelAuthoring.zh-CN.md)、[ACTAuthoring](Content/ACTAuthoring.zh-CN.md)。
- UE 任务依赖：[TASK-011 矩阵](Production/Tasks/TASK-011-Full-MVP-Task-Cluster.zh-CN.md)。启用对应交付时将旧 TASK-009／010 与已接受 HTML 对齐，避免再做一遍过时 UI。
- 相关未决项：[MVPDecisionRegister](Production/MVPDecisionRegister.zh-CN.md)，只看影响当前切片的条目；最新用户决定优先于旧通用 Backlog 文字。
- 历史证据与早期决定：[状态快照](Production/History/ProjectState-2026-09-09.zh-CN.md)、[Changelog](Changelog.zh-CN.md)及准确 TASK 报告。常规开工不加载历史。
