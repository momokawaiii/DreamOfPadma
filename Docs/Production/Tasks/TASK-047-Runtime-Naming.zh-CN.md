# TASK-047 按运行职责规范命名

- 英文原文：TASK-047-Runtime-Naming.md
- 状态：Verified（工作区；用户试玩仍归 TASK-046）
- Primary：root；当前 Local codex/mvp-demo-foundations
- 授权：用户要求用专业运行代码／类型名称替换 Demo 命名；已保存并关闭编辑器，允许迁移及编译。

## 范围与验收

将 Source/DreamOfPadma/{Public,Private}/MVP 下可复用运行源码移至按职责组织的 Core/Game/Gameplay/Presentation/World/UI 目录。用领域名称替换 Demo／Playable 运行符号；GameMode、SaveGame 分别放入独立头文件。同步 include、generated 头文件名、原生测试、编辑器生成调用与当前文档引用。保留旧 Source/.../Demo 测试代码、内容包路径、稳定玩法 ID、序列化字段名、存档槽位／应用标识／版本及游戏行为。

root 拥有以上源码移动与依赖引用、Config/DefaultEngine.ini 精确 CoreRedirects、Scripts/Editor 导出／生成／迁移脚本、带版本的迁移清单、本任务配对、命名规范配对、当前原生指南／索引／状态／日志及相关架构引用。Content/Padma/MVP/Playable 二进制资产仅通过 UE 加载／重保存反射引用，不移动路径或修改第三方。源码移动前校验路径位于工作区；禁止覆盖现有目标文件。

验收：迁移后的运行类型／文件名不含 Demo／Playable 开发阶段词，而明确表达职责。精确重定向保留旧 DataTable／DA／GameMode／SaveGame 类型。迁移后验证旧存档字节、现有表行与两张地图，完成 Editor 编译、原有 11 项定向测试、真实地图／战斗往返、AuditDocs 和严格项目验证。一次限定只读兼容性检查，不重复全面玩法审查。不提交／合并／推送。

## 证据

[命名规范与完整文件／类型映射](../../Architecture/RuntimeNaming.zh-CN.md)及 `Scripts/Migrations/RuntimeNaming047.json` 记录 39 个移动文件、4 个拆分／兼容新文件、48 个改名符号及 39 条精确重定向。`FPadmaACTSkillRow` 保留为 ACT 创作绑定行，`FPadmaACTSkillEffectRow` 表示独立效果结构。另增加定向回归 `Private/Tests/Compatibility/PadmaRuntimeNamingTest.cpp`。

当前原生运行目录为 Core、Game、Gameplay、Presentation、World、UI。`Game/Framework` 拆开 GameMode／Controller；`Game/Save` 拆开存档封装与兼容读取。依赖的旧测试／夹具 include 及编辑器导出／生成脚本已使用新契约。当前架构／模块／模型指南、索引、项目状态和变更日志均同步双语；历史 TASK 证据与测试内容名称保持原样。

- Editor 目标：UE 5.8.2 `DreamOfPadmaEditor Win64 Development` 编译成功；本地 `Artifacts/TASK-047/editor-build.log`。
- 自动化：`Automation RunTests TASK046+TASK047`，附带 `-PadmaNamingLegacySave`；12 项成功、零警告／失败，报告生成于 UTC `2026.09.09-08.55.26`，位于 `Artifacts/TASK-047/Automation/index.json`。
- 改名前二进制用默认值写出独立 `PadmaNamingCompatibility047` 槽位，新读取入口成功读取原始旧字节。另构造合成的旧格式内容：保留其旧文件头，在 UE 写入正确属性长度时使用旧结构体名；非默认随机状态／日历／资源／编队、非空卡牌／节点／参数／移动配置／弃牌堆及完整嵌套本局均在读取后相等。重存为当前封装再读取也相等。没有覆盖用户普通存档槽位。
- 初次直接 `StaticLoadObject` 检查发现普通字符串查找不会应用 CoreRedirects，UE 通用存档读取也有相同类名字符串问题。测试现检查重定向注册表，生产代码经精确匹配当前／旧封装的适配器读取字节，再沿用应用标记／版本／状态校验。未更改序列化字段、格式版本或玩法值。
- `MigrateRuntimeTypes.py`：18 张表内容全部不变；17 张改行类型的表与目录共重存 18 个资产；两张地图重存为 `PadmaGameMode`，行类型选择器标签已更新。证据为 `asset-migration.json`／`.log`；原包备份在被忽略的 `content-before`。
- 更新后的创作脚本重新运行成功：覆盖已有资产数为零，保留两张地图，目录校验无错误（`authoring.log`）。
- 真实渲染沙盘 → 战斗 → 退出／回滚 → 沙盘通过；新 GameMode／页面加载，待处理战斗清零，返回后仍为同一组 21 张本局卡（`roundtrip.log`、`roundtrip.png`）。
- 限定独立兼容检查未发现源码映射／字段／默认值漂移，其非默认旧状态证据缺口已由上述成功测试补齐；没有重复全面玩法审查。
- 文档审计、严格项目验证和限定空白检查记录在 `Artifacts/TASK-047/validation.log`。

边界：上述证明当前 UE 版本的 Editor／运行兼容性，未执行打包／Cooked 存档迁移；UE 在该环境中的结构体重定向行为不同，不能由 Editor 测试推断打包兼容。TASK-046 用户交互验收及正式美术仍待完成。没有请求新课程；未暂存／提交／合并／推送，未迁移第三方或旧夹具资产。
