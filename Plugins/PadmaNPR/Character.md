# Character NPR workflow

This page describes the **generic prototype**. The placed Chen now uses the separate [nine-slot reference variant](ReferenceCharacter.md), including source-derived Cloth/Skin, SDF curve, hair ramp and scene-color shadow behavior. The surface table below is not the contract for `_Reference` masters. Both variants share the component and Apply/Restore lifecycle.

This is a plugin-only, single-key-light artistic renderer. It does not alter UE, use UE 5.8 Toon materials, implement a new Substrate BSDF, or intercept UE per-light shadow evaluation. Existing Cloth stays on its DefaultLit hybrid path; Face/Skin/Hair/Eye use stock Unlit output with exposure compensation. HairShadow is translucent helper geometry. Ordinary Substrate remains a possible future backend, not an implemented custom BSDF. [Chinese guide](Character.zh-CN.md).

## Ownership and data flow

```text
Source MI: textures and sampler conventions
      + UPadmaNPRProfile: typed per-slot artistic values and head axes
      -> Studio Apply: persistent derived MIC under Profile folder / Generated
      -> UPadmaNPRComponent: selected mesh + explicit directional light
           Editor: dynamic per-mesh Custom Primitive Data
           Game: private per-slot MID + dynamic per-mesh data
      -> M_NPR_<Type> (one function call)
      -> MF_Padma<Type> (resources / parameters / Custom adapter)
      -> modular .ush -> Material Attributes -> stock UE material output
```

The source MI is never changed by applying a Profile. Generated MICs are output caches; do not edit their artistic parameters. Edit the DA and apply again. MIDs are runtime-only and are never serialized into editor mesh assignments. Opening Studio does nothing to the level; Apply/Restore are explicit transactions. Save the DA, generated MIs and level normally after applying.

The editor generated path is deterministic per Profile and slot. Actors sharing a Profile share its static artistic settings; create a distinct Profile to tune one independently. Runtime MIDs and dynamic head/light data remain independent. Restore preserves a material replaced by another system. Reapplying records the latest external replacement as its new restore baseline. Runtime Apply validates first; invalid data and explicit Restore stop frame-data writes. OnUnregister restores owned resources.

`ToonType`, `MaterialId`, `MaterialRegionId`, `ProfileId` are packed into `NPR_Identity`. These are configuration identity metadata, not GBuffer channels or GPU Atlas lookup indices. No Profile Atlas, renderer registration or per-pixel ID attachment is implemented. Typed C++ structs currently pack named float4 parameters, not a custom Substrate payload.

### Per-mesh frame-data contract

Custom Primitive Data floats **0..19 are reserved** on the target mesh: light direction/validity at 0, head forward at 4, right at 8, up at 12, head origin at 16. Do not attach another writer to that range. Other indices are preserved. One NPR component owns one skeletal mesh. On release the reserved range is restored. Component tick runs in PostUpdateWork with a mesh prerequisite. Light direction is surface-to-light (`-DirectionalLight forward`). Head axes come from the configured bone/socket world transform multiplied by `HeadAxisCorrection`.

There is no automatic light search. Choose the key light in the component; otherwise the Profile fallback direction is used. Changing actor yaw or animating the head updates the frame basis. This does not make the Unlit paths respond to every point light, engine shadow map or Lumen bounce.

## Daily use: no Python required

1. Create source MIs with parent `M_NPR_Face`, `M_NPR_Skin`, `M_NPR_Hair`, `M_NPR_Eye`, or `M_NPR_HairShadow` in plugin content. Bind resources.
2. Create a **PadmaNPRProfile** Data Asset. Add slots using the exact mesh material-slot names. Set Type and source MI for each. Configure head bone and correction.
3. Select the DA in the Content Browser. Open **Window > Padma NPR Studio**, click **Read Selected**. Character DA fields can be edited in its Details view.
4. Select the character in the level and click **Apply to Character**. The first skeletal mesh is used; with multiple meshes call the native Blueprint editor library with an explicit mesh or configure the component explicitly.
5. Set `PadmaNPRComponent.KeyLight`, inspect the result in the level, and save the changed assets/level. After changing artistic DA values, click Apply again. Runtime callers use `ApplyProfile` after changing the Profile.
6. **Restore Character** restores only the plugin-owned assignments and stops the binding. Undo is available through the editor transaction.

Cloth retains its existing `UPadmaClothProfile` and reference-cloth workflow; this character DA does not overwrite the completed clothing slot. See [ReferenceCloth](ReferenceCloth.md). No manual `import padma_cloth` is needed for the new character workflow. Python scripts in Artifacts are reproducible development configuration/capture tools, not runtime dependencies.

## Surface contracts

| Surface | Implemented response | Required resource / limitation |
|---|---|---|
| Face | local head basis, mirrored SDF B, 3x3 blur, angular threshold, overhead stabilization | NPR_SDF is linear B data. New angular threshold is not ZMD's SDFCurveBase reproduction. Calibrate head axes and thresholds per asset. |
| Skin | cel shading, warm shadow/transmission term, fill and rim | Artistic approximation; no physical subsurface diffusion/thickness tracing. Body slot can contain clothing regions, so keep warmth conservative. |
| Hair | tangent-space Y transformed to world, shifted wide/narrow Kajiya-Kay lobes, noise, backlight, AO | NPR_Normal uses UE Normal sampling (already decoded); PackedMask.G highlight, B AO. No assumption of ORM. |
| Eye | independent NoH highlight, Fresnel, Matcap, optional small iris UV depth offset | Uses existing iris geometry; no separate corneal/eye-white layer or octahedral normal encoding. Matcap is optional artistic extension, not established ZMD Eye behavior. |
| Bangs | temporal dither inside explicit red region mask, front-view weighting, head-height limit | TAA/TSR needed for smooth appearance; can produce temporal noise/ghosting. This is not alpha-blended whole-hair sorting. |
| Forehead shadow | existing helper geometry, translucent tint, light-facing opacity | An artistic overlay; no actual hair depth projection, scene-color HSV reconstruction or RDG shadow pass. |

For bangs, white RegionMask.R permits transparency and black is opaque. Default is black. Chen's mask is baked from `1 - Hair_P.R` (ZMD's front mask), then clipped above `BangMaxHeight` relative to the nose bone with a 2 cm transition. Source front-mask coverage includes the crown, so the height clip is essential. Different hairstyles should use an authored region mask. `BakeInvertedRedMask` is an editor utility for BGRA8 texture source data, refuses existing output packages, and never edits its input.

Common controls: Tint, shadow color, threshold/feather, AO weight, fill color/intensity, rim, exposure compensation. Surface controls live in Face/Skin/Hair/Eye structs. Debug values: 0 final, 1 diffuse band, 2 SDF, 3 world normal, 4 effective bang region. Debug is a DA value and follows the same Apply workflow.

## Current Chen binding

Root: `/Game/Sandbox/ACT/Character/ChenQianyu/Art`.

- DA: `Profiles/DA_Chen_NPR`; source MIs: `Materials/MI_Chen_NPR_*`; generated MIs: `Profiles/Generated`.
- Face -> `M_actor_chen_face_01`; Skin -> `M_actor_chen_body_01`; Hair -> `M_actor_chen_hair_01`; Eye -> `M_actor_chen_iris_01`; HairShadow -> `M_hairshadow_common_03`.
- Mesh has nine slots, not ZMD's thirteen. Cloth, cloth trim, brow and eye-shadow assignments are preserved. No synthetic empty eye/neck slots were added.
- The existing placed actor in `L_ChenACT` owns the component. Gameplay, animations, mesh geometry and the shared skeletal mesh's defaults are unchanged.
- Added copied source SDF/shift textures and a derived bang mask are confined to the character lab. Reference projects remain unmodified.

## Source and test evidence

Local Toon Face/Hair/Skin exports: `Artifacts/PadmaNPRCharacter/Toon`. Toon Face is Unlit; Hair/Skin are DefaultLit and contain KK / remap / color blend branches. The staged Toon project did not contain `M_Common_Eye`; no matching export was claimed. ZMD evidence is in `Artifacts/ZMDRenderAudit`: B-channel face SDF, hair shifts and masks, separate eye geometry and helper shadow path informed the implementation.

`PadmaNPR.Character.Binding` tests persistent editor materials, restore/external edits, invalid duplicate slots, parent-cycle rejection, runtime MID isolation and independent frame data. `PadmaNPR.Character.GPU` executes SDF directional/overhead, anisotropic lobe and eye-highlight checks on the GPU. Existing Cloth/Studio tests remain applicable. D3D12 material compilation and offscreen front/side-light/debug captures are in `Artifacts/PadmaNPRCharacter`. These checks are not packaging, performance, motion-quality or final artistic acceptance.

## Explicit remaining scope

Custom Substrate lighting, real engine shadow-mask integration, Profile Atlas allocation/lookup, advanced face local masks, exact source response curves, corneal geometry, Charlie/Neubelt Cloth, projected hair shadow, JFA/Hybrid outline, dedicated Studio preview scene and clean-project packaged validation remain separate work. None is represented by placeholder controls as a completed feature.

### Verified checkout result (2026-09-27)

Module build succeeded. All eight `PadmaNPR` tests passed in `Artifacts/PadmaNPRCharacter/FinalTests`; a fresh editor process reported `PADMA_CHARACTER_RELOAD_OK`, with persistent MICs and the four untouched slots verified. Final screenshots are `character_front.png`, `character_light_side.png`, and `character_bang_mask.png` in the same evidence directory. AuditDocs and strict project validation passed. The editor used for verification was closed after testing. Visual parity with the reference and animation/temporal quality remain unaccepted.
