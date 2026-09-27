# 角色 NPR 怎么用

本文描述旧通用原型。当前关卡陈千语使用 `DA_Chen_NPR_Reference`，九个槽位统一走[参考版流程](ReferenceCharacter.zh-CN.md)，其脸部曲线、头发色带、皮肤与衣服参数以参考版说明为准。

现在可以在 **窗口 → Padma NPR Studio** 中读取角色 DA、修改参数，再选关卡角色点 **应用到角色**。不需要手动导入 Python 库。

陈的入口是 `L_ChenACT`。角色配置位于 `Art/Profiles/DA_Chen_NPR`；贴图放在 `Art/Materials/MI_Chen_NPR_*`，应用后生成的 MI 在 `Profiles/Generated`。不要修改生成 MI 的艺术参数，下次应用会覆盖它们。衣服继续使用原来的 Cloth DA。

**操作顺序：**内容浏览器选 DA → Studio 读取所选 → 关卡选陈 → 应用到角色 → 在角色的 PadmaNPRComponent 中指定 Key Light → 保存 DA、生成的 MI 和关卡。修改 DA 后再次应用；“恢复角色”可以恢复接入前的材质。

配置管参数，源 MI 管贴图，组件管网格、灯光和头部骨骼。编辑器使用可保存的 MI，游戏使用每角色独立的 MID。头部和灯光数据使用网格自己的 Custom Primitive Data 0～19，其他系统不要占用这一段。

已接入面部 SDF、皮肤暖色、头发双层各向异性高光、眼部高光与 Matcap。刘海通过区域遮罩和头部高度限制做时间抖动透明；额头阴影是现有辅助几何的透明压暗近似。Debug 0～4 分别看最终颜色、明暗分带、SDF、法线、刘海区域。

这是单主光的最简艺术化工作流：没有修改引擎，没有使用 5.8 新 Toon 节点，也没有实现自定义 Substrate BSDF、Atlas、真实头发投影或 JFA 描边。皮肤暖色不等于物理皮下散射，眼睛也没有凭空增加角膜几何。截图和测试不等于已经达到参考工程的最终画质。

文件职责、资源通道、限制和验证细节见 [英文契约](Character.md)。
