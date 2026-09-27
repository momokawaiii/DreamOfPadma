# PadmaNPR architecture and integration

The [reference character guide](ReferenceCharacter.md) adds nine `_Reference` masters, source-derived shader files and DA scalar/vector maps to this architecture. It is the current placed-Chen example; the original generic templates and separate Cloth interface remain available.

The plugin implements Cloth and a minimal character workflow for Face/Skin/Hair/Eye/helper shadows. The character component and Studio Apply/Restore are implemented; custom rendering passes remain future work. See [Character](Character.md) for the current ownership and integration contract. Chinese guide: [Architecture.zh-CN.md](Architecture.zh-CN.md).

## File map

Paths are relative to the plugin. All source files are under `Source` and shaders under `Shaders/Private`.

| File | Responsibility |
|---|---|
| `PadmaNPR.uplugin` | Content mount, Runtime and Editor module descriptors |
| `PadmaNPRRuntime/PadmaNPRRuntime.Build.cs` | Runtime dependencies, independent of gameplay and Editor |
| `PadmaNPRRuntime/Private/PadmaNPRRuntimeModule.cpp` | Register `/Plugin/PadmaNPR` shader path at PostConfigInit |
| `PadmaNPRRuntime/Public/PadmaClothProfile.h` | Typed settings, slot overrides, Profile Data Asset and Blueprint factory API |
| `PadmaNPRRuntime/Private/PadmaClothProfile.cpp` | Validate/resolve settings, pack 16 parameters and create an independent MID |
| `PadmaNPREditor/PadmaNPREditor.Build.cs` | Editor-only dependencies |
| `PadmaNPREditor/Private/PadmaNPREditorModule.cpp` | Studio toolbar/navigation, character DA Details editing, Apply/Restore and lifecycle test |
| `PadmaNPREditor/Private/PadmaClothAuthoring.h/.cpp` | Create fresh template graph, encapsulate it, create template MI and DA |
| `PadmaNPREditor/Private/PadmaClothGraph.h/.cpp` | Partition graph into functions, validate candidate, save and connect master |
| `PadmaNPREditor/Private/PadmaClothTests.cpp` | Profile ownership, GPU math and graph contract tests |
| `PadmaMath.ush` | Safe normalization, Toon Remap, ZMD Adjust, ColorBlend |
| `PadmaClothNormal.ush` | RG normal reconstruction and artistic normal blend |
| `PadmaClothLighting.ush` | Diffuse band, AO, rims and reference GGX NDF |
| `PadmaMatcap.ush` | View-aligned basis, UV and layer evaluation |
| `PadmaCloth.ush` | Stable include entry, result structure and final composition |
| `Content/Functions/MF_PadmaClothBindings.uasset` | Textures, UV, channels, normal decoding and world inputs |
| `Content/Functions/MF_PadmaClothParameters.uasset` | Named global artistic parameter declarations |
| `Content/Functions/MF_PadmaCloth.uasset` | Bindings + parameters + Custom adapter + Material Attributes |
| `Content/Materials/M_NPR_Cloth.uasset` | One function call and native surface settings |
| `Content/Materials/MI_NPR_Cloth_Template.uasset` | Duplicate into project-owned surface MIs |
| `Content/Profiles/DA_Cloth_Default.uasset` | Duplicate into project-owned typed configuration |
| `Content/Textures/T_Cloth_Default*.uasset` | Neutral packed mask, Matcap masks and encoded normal |

## Integration

```text
Project MI (textures/channels) + Profile (Defaults + SlotOverrides)
    -> Create Cloth MID -> caller retains MID and calls SetMaterial

MI -> M_NPR_Cloth -> MF_PadmaCloth
    MF_PadmaClothBindings + MF_PadmaClothParameters
    -> Custom / PadmaCloth.ush -> Material Attributes -> native DefaultLit
```

Parameters remain global with unchanged names/defaults. Profile owns artistic settings; MI owns resource and encoding bindings; the caller owns dynamic light/debug settings and MID lifetime. The factory validates, resolves a named slot and writes a snapshot into a new MID. It does not edit the parent MI, assign a mesh, tick a light or automatically refresh after a DA edit.

1. Enable the plugin and show Plugin Content. This checkout already includes templates.
2. Duplicate `MI_NPR_Cloth_Template` into project content for each different clothing surface. In DreamOfPadma use `Content/Padma`.
3. Bind textures and actual channel/encoding conventions. `_P` does not guarantee ORM. See [Cloth.md](Cloth.md).
4. Duplicate `DA_Cloth_Default`. Edit Defaults; add exact named slot overrides and enable only groups that differ.
5. Call **Create Cloth MID** in the owning actor Blueprint with Self, the surface MI, Profile and exact mesh slot name. Check the error/null result, retain the MID, resolve the slot index and call **Set Material**.
6. Set `NPR_LightDirectionWS` on the MID when needed: surface-to-light direction, typically the negative directional-light forward vector. Update it when that chosen light moves.
7. Recreate/reassign when applying DA edits. Cache MIDs instead of allocating them every frame; restore originals when appropriate.

Directly assigning an MI works for texture experiments but does not read a DA. Studio opens from **Window > Padma NPR Studio**. Its character DA inspector is editable; other assets are read-only and Open launches their native asset editor. Studio's preview remains a placeholder.

## Authoring and maintenance

Master -> one Cloth function -> Material Attributes. Double-click to inspect Bindings, Parameters and the Custom adapter. Internal nodes are real render inputs, not preview-only nodes. This layout improves navigation, not GPU instruction/sampler cost.

`PadmaNPR.CreateClothTemplates` generates a new set only when targets are absent. `PadmaNPR.OrganizeClothGraph` upgrades the supported original flat template once. It preserves Custom code and component masks, rejects extra material outputs and existing function targets, compiles a candidate and saves dependencies before reconnecting the master. Independent default texture packages prevent function-to-master dependency cycles. An already attribute-based material is left alone. Partial saves are reported, not automatically rolled back; keep source control/backups for upgrades.

The authoring C++ creates fresh assets; editing it does not silently regenerate saved graphs. Existing content is the runtime source. New Profile settings require coordinated changes to the typed field, parameter packing, declaration and HLSL consumption. Algorithm changes belong in their shader module. Start learning with Math, then Normal/Lighting/Matcap, then Cloth composition.

The backend is Opaque, two-sided DefaultLit with world-space normals. Native lighting, exposure and tone mapping still apply. There is no UE 5.8 Toon BSDF/profile/atlas. Alpha cutout, separate E texture and per-texture specular mask are not implemented. No Chen assignments were changed. GPU/math/graph tests do not establish final character visual parity, performance or packaged behavior.

## Character extension file map

| File | Responsibility |
|---|---|
| `PadmaNPRProfile.h/.cpp` | Typed Face/Skin/Hair/Eye slot settings, resource-MI bindings, identity, validation and packing |
| `PadmaNPRComponent.h/.cpp` | Mesh/light/head binding, per-mesh frame data, runtime MIDs and ownership-aware restore |
| `PadmaNPRStudioLibrary.h/.cpp` | Native transactional apply/restore, persistent derived MIs and optional mask bake |
| `PadmaCharacterAuthoring.cpp` | Generate five thin master materials and their resource/Custom adapter functions |
| `PadmaCharacterTests.cpp` | Binding lifecycle, isolation and GPU math regressions |
| `PadmaCharacterCommon.ush` | Shared cel, fill and Matcap basis |
| `PadmaFace.ush` | Head-local face SDF band and overhead stabilization |
| `PadmaSkin.ush` | Artistic warm transmission response |
| `PadmaHair.ush` | Shifted tangent lobes and backlight |
| `PadmaEye.ush` | Independent highlight, Fresnel and Matcap response |

The earlier integration steps above describe Cloth only. Character usage and limitations are owned by [Character.md](Character.md).
