# 陈千语衣服槽 NPR

已新增参考版本 `M_NPR_Cloth_Reference_Masked`，并接入 `L_ChenACT` 中陈千语的 `M_actor_chen_cloth_01`。其它八个材质槽保持原绑定。

这版补齐了参考明暗重映射、分支法线、独立 GGX 粗糙度、Matcap UV 与加色、曝光补偿、逆色调映射和 Alpha 裁切。主光方向自动读取 Atmosphere Light 0。依然使用普通 DefaultLit，没有使用 UE 5.8 新 Toon 材质。

资产在 `/Game/Sandbox/ACT/Character/ChenQianyu/Art`：

- `Materials/MI_Chen_Cloth_NPR_Reference`：贴图绑定与已应用的风格参数。
- `Profiles/DA_Chen_Cloth_NPR`：风格参数的权威来源。
- `Textures/T_actor_common_matcap_05_D`：从参考工程复制的 Matcap。

修改 DA 后需显式应用，避免两边同时维护参数。UE Python 控制台执行：

```python
import padma_cloth
padma_cloth.apply_profile('/Game/Sandbox/ACT/Character/ChenQianyu/Art/Profiles/DA_Chen_Cloth_NPR', '/Game/Sandbox/ACT/Character/ChenQianyu/Art/Materials/MI_Chen_Cloth_NPR_Reference', 'M_actor_chen_cloth_01')
```

贴图和通道仍在 MI 调整。运行时也可使用原来的 Create Cloth MID 接法。参考版本中 Feather 表示乘法缩放，不是 smoothstep 宽度；原生金属度和粗糙度在 Reference 分组调。

已做 Shader 编译、GPU 数学、Profile/槽位隔离、保存重载与同镜头前后/转光检查。离屏截图与用户视口的曝光/GI 未做严格校准，不能据此宣称和 ZMDRender 逐像素一致。当前未包含动画雨水、绒毛、薄膜或屏幕空间边缘光；粗糙度 .3 倍率是固定表面配置，不代表已经实现雨湿系统。完整契约见 [英文说明](ReferenceCloth.md)。
