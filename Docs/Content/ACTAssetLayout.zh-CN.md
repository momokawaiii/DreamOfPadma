# ACT 最终资产目录

详细约定见 [英文说明](ACTAssetLayout.md)。

- 地图入口：`/Game/Sandbox/ACT/Training/Maps/L_ChenACT`。角色和木头人仍预摆在场上，不用 PIE 就能调模型材质和平行光。
- 陈千语：`Character/ChenQianyu` 下按 `AbilitySystem`、`Blueprints`、`Animation`、`Art`、`Presentation/Camera` 分工。
- 扶摇：`Weapon/Fuyao`。角色副手剑和剑鞘在角色自己的 `Art/Equipment`。
- 木头人、地面、灯光测试资源归 `Training`。
- `Attack01`、`Centimeter` 不再作为历史目录保留，但第一段普攻、现用模型、骨架和刀光都保留。旧的独立单攻击配置和预览蓝图退出活动资产；测试用 C++ 临时配置。
- 同名特效不等于同内容；保留有区别的 `BladeContact` 特效。Imported 与 Runtime 动画也有根运动配置差异，不能直接删一份。

策划现在改角色 DA、技能 DT/DA 和 Montage 的 AN/ANS；美术与场景在编辑器改；规则和生命周期在 C++。通用 Buff、法术场、投射物、召唤物，以及你提出的总表体系还需要后续接口实现，这次不会用空表冒充已完成。已有正式总表保留在 Padma 目录，不复制出第二套权威数据。

逐文件用途、旧新路径和引用在 `Artifacts/ACTFinalLayout/AssetLedger.md`、`plan.json`。迁移前所有资产已备份到同目录 `before-migration.zip`。这些只是检查记录和恢复备份，运行时不依赖它们。
