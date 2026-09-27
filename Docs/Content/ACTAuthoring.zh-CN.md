# 陈 ACT 配置指南

- 文档 ID：CONTENT-ACT-AUTHORING-001
- 版本：1.0
- 更新：2026-09-27
- 状态：只保留陈 ACT 的配置路径

使用 `Content/Sandbox/ACT/Character/ChenQianyu` 下已有的角色、技能、武器、ABP 和 Montage。通用的 `Content/Padma/MVP/Definitions/ACTCharacterCards` 模板已经删除，不要重新创建。

`L_ChenACT` 保存一个陈和三个木桩。运行时可以借用并重置这些对象，但不能悄悄替换场景布局。Montage 的 AN/ANS 控制动作窗口，GAS 负责激活、提交、取消和结束。

可用的检查包括 ACT 资产审计、场景参与者验证，以及 `DreamOfPadma.ACT.*` 自动化测试。构建或静态配表通过不等于 PIE、GPU 画面、原作视觉还原或打包通过。

详细依据见[英文原文](ACTAuthoring.md)。
