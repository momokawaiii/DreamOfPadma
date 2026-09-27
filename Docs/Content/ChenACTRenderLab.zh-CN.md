# 陈千语：木桩和三渲二共用地图

打开 `/Game/Sandbox/ACT/Training/Maps/L_ChenACT`。
以后就在这张地图学材质、测战斗，不另建教学地图。

现在陈千语和三个木头人已经预摆在 `ACT/Participants` 中。**不用进 PIE**，直接选角色的 Mesh 调材质，旋转平行光看三渲二效果，正常保存地图即可。测试攻击和 B 面板才进 PIE。

角色蓝图是 `Blueprints/BP_ChenACTPlaced`，木头人是 `WoodenDummy/BP_ACTWoodenDummy`。
训练场的 `Scene Player / Scene Targets` 引用这四个实例，不再开局生成替代品。
F8 清理战斗和 GAS 状态、回原位，复用同一批 Actor；不会覆盖手调的身体模型、材质和网格偏移。
血量、防御等在每个角色的 `Combat / Scene / Scene Spec` 调；技能参数仍在 DA/DT，窗口仍在 Montage 的 AN/ANS 中调。执行逻辑留在 C++。

`Apply Scene Presentation` 是明确恢复默认装配的按钮，会重设模型和默认挂点，手调之后不要随便点。无需单独再造一个预览角色，同一预摆蓝图就能兼顾编辑器和战斗。

- `ACT` 文件夹：原来的角色训练逻辑。B 打开面板，F8 重置木桩和记录。
- 三个圆柱已换成 Wjgz 的木头人，打中会播放原来的受击动画，连打会重新播放。血量、防御和记录规则不变。
- `RenderLab` 文件夹：按 Toon 的 `Map_Main` 重建主光、环境光、浅灰雾背景和两层地面。
- 主光强度 6，灰色 Cubemap 环境光 0.2；手动曝光、补偿 0，不使用物理相机曝光。先固定曝光，只转主光看明暗。
- 原来的训练碰撞地面保留但不显示，新地面只负责画面。上一版补光和参考球已从地图撤掉。
- B 展示仍有原来的景深。比较材质时，保持相同镜头、灯光和曝光；在 PIE 中改灯光不会自动保存。

角色资产在 `Content/Sandbox/ACT/Character/ChenQianyu`，武器在 `ACT/Weapon/Fuyao`，训练资源在 `ACT/Training`。旧历史目录退场，完整说明见 [ACT 最终目录](ACTAssetLayout.zh-CN.md)。
后续亲手创建的材质、函数、贴图放到它的 `Art/Materials`、`Art/Materials/Functions`、`Art/Textures`，用到时再建。
这里只复制了 Toon 地面用到的 7 个资产，放在 `Training/Environment/ToonReference`。源项目没改；角色材质和角色边缘光留给你自己连。

从固有色、明暗分区开始，再做脸部 SDF、头发高光、其他部位和描边。
Padma 有 9 个材质槽，参考角色有 13 个；完整复刻前要核对模型分区，不能直接按编号替换。

环境脚本：`Scripts/Editor/ConfigureChenACTRenderLab.py`。重复运行会恢复固定曝光基准，不改角色材质。
木头人在 `Content/Sandbox/ACT/Training/WoodenDummy`；现在模型、材质、大小直接在各木头人的 Mesh 上改，受击动画仍在训练场中选。旧的 Training Targets 和模型字段仅供初始装配/旧测试，不会覆盖预摆实例。
预摆配置脚本是 `ConfigureChenSceneParticipants.py`；重载检查是 `VerifyChenSceneParticipants.py`，记录和原地图备份在 `Artifacts/ChenSceneParticipants`。
日常打开、预览和战斗不读 Unity 缓存。战斗待机、武器和特效绑定脚本也已改用现有 Content 资产；缺资产会报错，不再偷偷从 Artifacts 导入。Artifacts 里的原始提取和记录保留，仅供离线导入或追溯。
备份和检查记录在 `Artifacts/ChenACTLayout`，Toon 参考参数和实机截图在其 `ToonReference` 子目录。
