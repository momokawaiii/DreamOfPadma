# ADR-0015：角色 NPR 接入

角色 DA 管艺术参数，源 MI 管贴图。Studio 用原生 C++ 应用和恢复；编辑器保存派生 MI，游戏创建独立 MID，不需要 Python。

组件按头部骨骼和指定灯光更新网格数据，保留 Custom Primitive Data 0～19。Cloth 原配置继续使用。

不修改引擎、不用 5.8 Toon 节点。当前角色核心走普通 Unlit；刘海是时间抖动透明，额头阴影是辅助几何近似。Atlas、自定义 Substrate BSDF 和高级描边还没有实现。

详见 [英文决策](ADR-0015-NPR-Character-Binding.md) 与 [使用指南](../../Plugins/PadmaNPR/Character.zh-CN.md)。
