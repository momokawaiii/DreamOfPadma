# 插件接入结构

当前陈千语已增加九槽 `_Reference` 材质，使用 DA 标量/颜色映射和参考 Shader。具体文件职责与学习顺序见[参考角色说明](ReferenceCharacter.zh-CN.md)；旧通用模板继续保留。

插件现在由 Runtime 和 Editor 两个模块组成。Runtime 管 DA、角色组件、Shader 路径与参数；Editor 管 Slate 面板、应用/恢复和材质生成。

角色使用“源 MI 贴图 + DA 参数 → 组件/生成 MI → 薄母材质 → 函数入口 → HLSL”。编辑器生成可保存的 MI，游戏生成角色独立的 MID。母材质内部的资源节点是真实渲染输入，不只是预览。

Cloth 保留原来的专门配置；Face、Skin、Hair、Eye 和辅助阴影由新角色 DA 管理。日常接入不需要 Python。具体用法见 [角色指南](Character.zh-CN.md)，文件清单见 [英文架构](Architecture.md)。

当前没有额外 Renderer 模块、Atlas、自定义 BSDF 或 RDG 描边。专用预览区仍未实现。
