# Cloth Core v1

This is a reusable artistic Cloth core derived from Toon and ZMDRender conventions, not a pixel-identical port or a new BSDF. Read [the Chinese guide](Cloth.zh-CN.md) for the learning entry point.

## Implementation

- `Shaders/Private/PadmaCloth.ush`: stable entry and final composition. Includes `PadmaMath.ush` (vectors/remaps/blend), `PadmaClothNormal.ush` (normal decode/blend), `PadmaClothLighting.ush` (diffuse/AO/rim/GGX) and `PadmaMatcap.ush` (UV/layers). See [Architecture](Architecture.md) for the complete file map.
- `PadmaNPRRuntime`: shader-directory registration at PostConfigInit, typed Cloth Profile, group-level slot overrides, explicit MID factory. No Editor or DreamOfPadma dependency.
- `PadmaNPREditor`: thin material authoring, existing Studio navigation, compile/ownership/GPU tests. No custom Renderer module is needed for this slice.

The master uses the traditional DefaultLit hybrid: artistic color goes to BaseColor, explicit emissive/specular/rim-highlight contributions go to Emissive, and metallic/specular/roughness/world normal go to their native outputs. Native lighting, exposure and tone mapping still apply. This does not inject a cel response into every engine light. It does not use UE 5.8 Toon BSDF. Substrate-specific migration remains separate.

## Assets and workflow

The explicit console command `PadmaNPR.CreateClothTemplates` creates a **new** set if none of its target packages exists:

- `/PadmaNPR/Materials/M_NPR_Cloth`
- `/PadmaNPR/Materials/MI_NPR_Cloth_Template`
- `/PadmaNPR/Profiles/DA_Cloth_Default`

It also creates three functions under `/PadmaNPR/Functions` and neutral masks under `/PadmaNPR/Textures`; independent texture packages prevent function-to-master dependency cycles. The master is one `MF_PadmaCloth` call into Material Attributes. Inside it, Bindings supplies resources and Parameters supplies the artistic controls to the HLSL Custom adapter. These are real rendering inputs, not preview-only nodes. Encapsulation improves reading, not GPU cost.

It never overwrites existing templates or assigns a character mesh. The normal/color fallbacks use engine resources. No source character assets are copied. If generation fails before save, fix the issue and restart to clear newly created in-memory packages. If a save partially succeeds, inspect the reported assets; do not remove user edits blindly or expect a retry to overwrite them. `PadmaNPR.OrganizeClothGraph` explicitly upgrades the supported original flat template; details and limits are in the architecture guide.

1. Duplicate the template MI for each clothing surface in your own project content.
2. Bind textures and encoding/channel settings in the MI.
3. Create a `PadmaClothProfile` Data Asset (or duplicate the default). Edit Defaults; add named SlotOverrides and enable only the groups that should differ.
4. Call **Create Cloth MID** with owner, texture-binding MI, Profile and exact material slot name. Inspect the returned error and null result. It resolves defaults + the matching enabled override groups, validates the schema, and writes all artistic parameters into a fresh MID.
5. The caller explicitly assigns the MID. The caller also owns keeping it alive, updating runtime light direction and restoring its original material when appropriate. No automatic tick, scene binding or component is provided yet.

Repeated calls create separate MIDs. Cache/reuse the returned MID in the caller instead of calling the factory every frame. Profile edits are snapshots: explicitly recreate/reapply through your authoring flow; there is no hidden hot binding.

## Authority and texture contract

Profile owns `NPR_Tint`, normal weights, Lam, AO, Rim, GGX colors/roughness and strengths, Matcap tints/strengths, surface metallic scale/specular/emissive weight. These vector parameters remain visible on a raw MI for shader experiments, but the MID factory overwrites them from the Profile. Editing them in both places is not a supported production workflow.

MI owns the following bindings:

| Input | Contract |
|---|---|
| NPR_BaseColor | Color texture; sRGB enabled where appropriate for authored color |
| NPR_Normal | UE normal-map compression and Normal sampler; already decoded by the material TextureSample |
| NPR_EncodedNormal | Linear Masks texture; RG mapped to [-1,1], optional Y flip, reconstructed positive Z |
| NPR_NormalEncoding | 0 = UE normal, 1 = encoded RG; no automatic filename inference |
| NPR_EncodedNormalFlipY | 1 flips Y for encoded RG only |
| NPR_PackedMask | Linear Masks texture; never infer ORM from `_P` |
| NPR_AOChannel / NPR_MetalChannel / NPR_RoughnessChannel | RGBA selector vectors; use one-hot vectors for channel selection |
| NPR_IsGlossiness | 1 converts selected gloss to `1-gloss`; 0 treats it as roughness |
| NPR_MatcapMasks | Linear Masks texture, R = layer 1, G = layer 2 |
| NPR_Matcap01 / 02 | Color textures; clamp addressing recommended at texture edges |
| NPR_UVScale | XY multiplier applied to UV0 for character textures |
| NPR_LightDirectionWS | Runtime input: normalized surface-to-key-light direction in world space; initially an explicit constant, not automatically linked to a scene light |
| NPR_Debug | 0 final; 1 band; 2 AO; 3 rim shadow; 4 rim highlight; 5 saturated NDF; 6 artistic world normal |

Packed defaults select metal R, AO B, gloss A; these are only template defaults. Roughness is `clamp(lerp(BaseRoughness,MetalRoughness,metallic)*resolvedRoughnessMask,0.04,1)`. AO is multiplied artistically into color once; native AO output is not simultaneously darkened.

## Source fidelity versus deliberate generalization

- Toon remap uses `saturate(x / smooth - offset)` and strength blends toward 1; ZMD adjust uses `saturate(x * smooth - offset)`. Diffuse Mode selects either, or a new threshold/feather smoothstep mode. In remap modes the field named Feather carries source smooth/scale, not a physical transition width.
- ColorBlend retains the nested Lerp convention. Blend color alpha selects multiply versus replace; it is not surface opacity.
- Reference GGX uses `normalize(N + L)`, as the engine function called by Toon does. It is an NDF-shaped artistic highlight, not full GGX Fresnel/visibility/energy conservation. Zero-length half directions use the back-facing limit; roughness has a 0.04 floor.
- ArtisticNormal sequentially blends mapped/geometric normal toward view and light directions. This is an explicit generalized control, not a claim to match every source normal branch.
- Matcap uses a view-aligned tangent basis with a camera-up vector and a defined degenerate fallback. The second layer blends after the first. It is not a universal match for every rotated/transformed source Matcap.
- AO, rim-highlight and art-normal branches are optional features here even where source graphs contain inactive paths. No inference of active source behavior is made from node presence alone.
- Native specular and explicit artistic NDF can both contribute; explicit NDF defaults to zero to avoid accidentally stacking highlights. Debug clears native metallic/specular contribution, but exposure, tone mapping and scene post effects still affect presentation.

## Limits and acceptance

The graph readability refactor was verified in a fresh UE process on 2026-09-22: 41 original master expressions became one function call; `GraphContract`, `ProfileOwnership` and `GPUMath` passed, with GPU RGB=(1,1,1). The original MI/DA file hashes and all 14 HLSL function signatures/bodies were unchanged. Evidence: `Artifacts/PadmaCloth/Refactor/reload.log`, `ReloadTests/index.json`, `preservation.json` and `build-modules.log`. PadmaNPR modules compiled successfully; a full host rebuild encountered an unrelated concurrent ACT duplicate-definition error. This refactor does not claim full host build acceptance or final character image parity.

Verified on 2026-09-22 with local UE 5.8: Editor native build; PCD3D_SM6 material compilation and new template saves; fresh-session template load; ProfileOwnership and GPUMath both passed. Actual GPU fixture readback was RGB=(1,1,1). Strict project validation and documentation audit passed. Evidence is in `Artifacts/PadmaCloth/build.log`, `verify-final.log` and `Tests/index.json` in the host project. The initial readback harness was corrected to use Unreal's standard DrawMaterialToRenderTarget path, including shader completion and resource update; the numeric assertions were retained.

No wet/rain, thin film, fur shells, separate emissive texture feature, custom shadow/light pass, inverse tone-map code, automatic scene lighting or automatic character conversion. No world/material assignment was changed for Chen. The mathematical core and stock-engine hybrid can be tested before any future Substrate implementation.

Compile and test commands target `PadmaNPR.Cloth`. ProfileOwnership checks per-slot isolation and unchanged parents. GPUMath runs the actual .ush in a transient material and reads back GPU pixels for remap/blend/normal/Matcap and GGX boundary fixtures. These are not character visual acceptance. Rotating-light Chen clothing comparison, second-character visual parity, performance profiling and Cook/package remain separate work.
