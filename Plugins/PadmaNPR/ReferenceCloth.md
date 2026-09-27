# Reference Cloth and Chen integration

Chinese guide: [ReferenceCloth.zh-CN.md](ReferenceCloth.zh-CN.md).

## Implemented slice

`M_NPR_Cloth_Reference_Masked` is a separate DefaultLit, two-sided, alpha-cutout variant. Legacy Cloth assets and their 16-vector contract remain available. No UE 5.8 Toon shading model or Toon atlas is used.

`PadmaClothReference.ush` owns the reference response: division-free ZMD mask adjustment; independent branch normal controls; colored Lam/AO; source-style rim multiplication; GGX with the intermediate direction `normalize(L + V * CameraWeight)`; texture roughness independent of native surface roughness; WorldUp Matcap coordinates with scale/subtract-offset and wrap sampling; additive Matcap; inverse film tone mapping and base-color power. Final composed color feeds both BaseColor and the scaled Emissive output. Native `EyeAdaptationInverse` nodes compensate both outputs.

`PadmaClothReferenceAuthoring.cpp` creates three reference functions and the one-call master. It clones the existing binding/parameter functions into new packages, adds alpha and atmosphere-light input, and refuses to overwrite existing targets. The active atmosphere light at index 0 supplies the artistic key direction, so rotating that light does not require a Blueprint parameter update. The current project has one directional light; authoring enables its atmosphere-light role without changing rotation or intensity.

## Profile authority

`FPadmaClothReference` extends the existing Profile with explicitly enabled reference controls. Slot overrides can replace the Reference group. Disabled reference response preserves legacy packing. The MID factory rejects a Profile/material variant mismatch instead of leaving stale defaults silently active.

In the reference variant, `Diffuse.Feather` and the Rim `Feather` fields carry multiplication scales, and Threshold carries subtraction offset. `Specular.RemapSmooth` is a multiplication scale. AO and shadow colors use direct multiplication/lerp; their alpha does not select the blend operation. GGX and Rim colors retain the reference multiply/replace alpha convention. Matcap colors multiply the samples before addition; their alpha is unused. Native metallic/roughness are owned by Reference settings; legacy MetallicScale and Base/MetalRoughness do not control these outputs. `Specular.Strength` still enables the artistic GGX contribution and `Surface.Specular/EmissiveWeight` still control the native outputs.

The Chen configuration uses zero Camera/Light normal weights, mapped normal detail 1, Lam scale 10/offset -2.037126, GGX scale .272/offset .085334, native metallic 0, native specular .666667 and emissive weight .5. Matcap 05 uses the reference UV adjustment and wrap addressing. This is a source-derived starting configuration, not a claim that every scene matches the ZMDRender screenshot.

## Saved project assets

Under `/Game/Sandbox/ACT/Character/ChenQianyu/Art`:

- `Materials/MI_Chen_Cloth_NPR_Reference` binds the existing D/N/P textures and copied Matcap 05.
- `Profiles/DA_Chen_Cloth_NPR` owns the artistic settings.
- `Textures/T_actor_common_matcap_05_D` is the project-owned reference texture. The source ZMDRender asset was not edited.

In `/Game/Sandbox/ACT/Training/Maps/L_ChenACT`, only `BP_ChenACTPlaced_C_0 / CharacterMesh0 / M_actor_chen_cloth_01` is reassigned. The other eight slots, including the separate cloth_02 trim material, retain their bindings. Original clothing instances remain available. No gameplay/animation assets or mesh section assignments are changed.

The placed actor uses an explicitly baked MI snapshot for edit-time visibility. After changing the DA, use the UE Python console:

```python
import padma_cloth
padma_cloth.apply_profile(
    '/Game/Sandbox/ACT/Character/ChenQianyu/Art/Profiles/DA_Chen_Cloth_NPR',
    '/Game/Sandbox/ACT/Character/ChenQianyu/Art/Materials/MI_Chen_Cloth_NPR_Reference',
    'M_actor_chen_cloth_01')
```

The helper validates the variant/schema, applies only the artistic vectors in a transaction and saves the MI. It does not alter texture bindings, mesh assignments or the world. Runtime callers can continue using Create Cloth MID and assigning the returned object. There is no implicit DA polling.

## Validation and boundaries

The PadmaNPR module build and five `PadmaNPR.Cloth` tests passed, including actual GPU readback for the reference equations. A fresh-session reload checks all nine slot bindings, parent, Matcap reference and explicit Profile application. Evidence is under `Artifacts/PadmaCloth/V2`: `configured.json`, `reload.json`, `Tests`, `ReloadTests`, and three scene captures. The same-camera before/after capture changes the clothing MI; a third capture rotates the key light. Temporary capture actors and test light rotations are not saved. These offscreen scene captures are not calibrated matches to the user's viewport exposure/GI or a full ZMDRender A/B render.

The .04 GGX roughness floor and Matcap pole fallback are deliberate numerical safeguards. Arbitrary Camera/Light normal weights are not proven equivalent to the reference angle-corrected tangent-normal blend. RoughnessScale=.3 is a fixed finish multiplier inspired by the audited rain-roughness branch; animated rain, wet masks and rain normals are not implemented. Extra E/M features, thin film, fur, screen-space edge lighting and full character parity are outside this garment-slot slice. The reference relies on UE's FilmToneMapInverse implementation, not a universal inverse of every post-process/OCIO configuration.
