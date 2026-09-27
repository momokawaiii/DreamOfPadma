# ADR-0013：Cloth Core 与显式配置应用

PadmaNPR 新增独立 Runtime 模块，提供 HLSL 路径注册、Cloth 配置和 MID 创建。Editor 依赖 Runtime，二者都不依赖 Padma 玩法代码。

DA 管艺术参数，MI 管贴图与通道；服装按槽名覆盖勾选的分组。调用方负责把 MID 赋给网格及恢复，插件不自动替换陈千语材质。

先结合 Toon / ZMDRender 做普通 DefaultLit 混合接入，不引入新 BSDF 或 Pass，不使用 UE 5.8 Toon。角色画面对照与打包仍需验证。详见 [英文决定](ADR-0013-NPR-Cloth-Core.md)。
