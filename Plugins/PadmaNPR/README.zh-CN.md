# Padma NPR Studio

**新的手动输入、真实预览和“生成 NPR 角色”已接通：[操作说明](ManualAuthoring.zh-CN.md)。** 右上角“已有 DA / 参考版”保留陈千语原接入流程。新基础 Shader 的范围与参考版不同，请先看说明中的边界。

历史接口规划见 [角色接入与使用指南 v1](CharacterWorkflow.zh-CN.md)：现有用法、Shader/C++/面板分工，以及“网格＋手动指定贴图语义→生成 NPR 角色”的下一版接口。先完成手动接口，再做自动匹配、描边和第二角色验证。

当前陈千语已切换到 `DA_Chen_NPR_Reference` 九槽参考版：[使用与学习顺序](ReferenceCharacter.zh-CN.md)。下面的旧通用版仍保留，参数接口与参考版不同。

在 **窗口 → Padma NPR Studio** 打开。现在可以编辑角色 DA，向关卡角色应用或恢复材质，不需要手动执行 Python。

- [角色配置与使用](Character.zh-CN.md)：Face SDF、皮肤、Hair、Eye、刘海和辅助阴影。
- [参考衣服槽](ReferenceCloth.zh-CN.md)：保留已经完成的陈千语主衣服配置。
- [架构](Architecture.zh-CN.md)：DA、MI、组件、HLSL 与编辑器的职责。

面板采用 UE 原生深灰主题、工具栏和折叠属性分组。专用预览仍是占位区，当前在 L_ChenACT 中调灯光和观察角色。

不修改引擎，不使用 UE 5.8 新 Toon 材质；高级 Atlas、渲染 Pass、描边和自定义 BSDF 尚未实现。详细边界见 [英文说明](Character.md)。
