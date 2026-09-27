# Cloth 第一版：从节点进入代码

这版结合 Toon 与 ZMDRender 的公共结构，提供可复用的艺术化 Cloth Core，不保证原图逐像素一致，也不是新光照模型。

Shader 已按 Math、Normal、Lighting、Matcap 拆分，由 `Shaders/Private/PadmaCloth.ush` 组合。母材质主图只有 `MF_PadmaCloth → Material Attributes`，内部再分 Bindings 和 Parameters。文件职责和使用步骤见 [架构说明](Architecture.zh-CN.md)。Toon 的除法 Remap 与 ZMD 的乘法 Adjust 都保留，不能混用同一套数值。

打开插件内容中的 `M_NPR_Cloth` 查看薄材质入口；复制 `MI_NPR_Cloth_Template`，填自己的纹理、通道与法线编码。复制 `DA_Cloth_Default`，在 Defaults 填艺术参数，再按材质槽名增加 SlotOverrides，勾选真正需要覆盖的分组。

蓝图函数 **Create Cloth MID** 读取 DA 并生成独立动态材质。调用方自己赋给网格、保存引用和恢复原材质。DA 管艺术参数；MI 管贴图绑定，避免两边同时调同一参数。光方向目前需要显式设置，不会自动追踪关卡灯光。

第一版使用 DefaultLit 混合接法；引擎光照、曝光和色调映射仍参与。Debug 提供明暗、AO、Rim、NDF 和法线检查。陈千语现有材质没有被替换，需后续手动做视觉对照。

已通过 UE 5.8 编辑器编译、材质 Shader 编译、资产保存后重载、槽位/MID 隔离测试及真实 GPU 数学回读测试。这不等于角色画面已经验收。

雨湿、薄膜、绒毛、自定义 Pass 和 Substrate 专用接入暂未实现。严格排除 UE 5.8 新 Toon 材质。详细参数契约、默认值与限制见 [Cloth.md](Cloth.md)。
