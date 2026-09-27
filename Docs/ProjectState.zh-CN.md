# 项目状态

目标是可安装、离线玩的第零章 Demo。

NPR 新增手动网格/贴图输入、真实角色预览和“生成 NPR 角色”，详见[操作说明](../Plugins/PadmaNPR/ManualAuthoring.zh-CN.md)。基础版是不透明、显式主光着色，已验证七槽生成、PIE 与界面重开；不代表参考版全部效果或真实场景接收阴影。

PadmaNPR 已将陈千语九槽接入 `DA_Chen_NPR_Reference`，补齐干燥皮肤/衣服、头部平面 SDF 与软边、局部 0.88 刘海透明。11 项自动检查和真实待机姿态的头部坐标回归通过。最终美术效果、高速战斗下的时域稳定性仍待验收，无需手动 Python，见[使用与学习说明](../Plugins/PadmaNPR/ReferenceCharacter.zh-CN.md)。

当前只保留主地图的全部功能、它需要的战斗地图和陈的 ACT。旧预览与历史记录已清理；按用户要求，回退备份也已删除。见[清理说明](Production/ProjectCleanup.zh-CN.md)。

- 战略原型入口：`/Game/Padma/MVP/Playable/Maps/L_PadmaWorld`，已有 55 格绘景教程和基础操作。
- 陈千语入口：`/Game/Sandbox/ACT/Training/Maps/L_ChenACT`。陈和三个木头人已预摆，不进 PIE 就能调材质和灯光；C++ 接管战斗，F8 复用原 Actor。日常配置不再读取 Unity/Artifacts 缓存。环境参考 Toon，角色材质由用户逐步连线。见[使用说明](Content/ChenACTRenderLab.zh-CN.md)。
- 最新输入、下落攻击和武器切换还要实际试玩；训练面板布局、切页和闪烁也要看真机。
- ACT 已逐个核查 1427 个资产，整理为 1422 个活动资产，5 个旧测试/预览资产可从本次备份恢复；Attack01/Centimeter 历史目录退场，第一段普攻保留。全资产引用检查和 7 项针对性回归通过，见 [最终目录](Content/ACTAssetLayout.zh-CN.md)。通用 Buff/投射物/召唤物总表不是本次已完成功能。
- 正式 3D 烘焙地图、章节教程存档和完整通关流程还没交齐，正式卡牌及奖励数据仍待提供。
- Development、Shipping 打包和干净安装仍待验收；占卜和战略界面还不能只凭自动测试宣称画面验收完成。

小修复不建 TASK，中文文档只讲重点。详细入口看 [索引](00_INDEX.zh-CN.md)。

详细依据、操作和证据见[英文原文](ProjectState.md)。
