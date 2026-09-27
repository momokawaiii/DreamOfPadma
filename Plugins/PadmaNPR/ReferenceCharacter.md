# Reference character shading

This is the source-derived variant of [Character](Character.md), not a claim that the earlier generic prototype matched ZMDRender. ZMD's Chen live material outputs are the visual baseline; Toon supplies the previously studied mask/color/normal vocabulary. No engine modification, UE 5.8 Toon BSDF, custom lighting injection or Profile Atlas is used. [Chinese learning guide](ReferenceCharacter.zh-CN.md).

## Assets and ownership

Nine `M_NPR_<Type>_Reference` masters cover Skin, Cloth, HairLine, Brow, EyeShadow, Face, Hair, HairShadow and Eye. HairLine corresponds to Chen cloth_02. The five larger masters call `MF_Padma<Type>ReferenceVisual`; resource sampling conventions, native coordinate dependencies and Custom adapters live there. Small supporting masters retain the necessary native scene-depth/color and vertex-stage nodes.

`Shaders/Private/Padma{Hair,Face,Skin,HairLine}Reference.ush` and `PadmaClothGraphReference.ush` contain the reachable source graph arithmetic. These source-node-commented SSA files are an auditable reconstruction baseline, not yet a manually optimized teaching library. `PadmaReferenceCommon.ush` owns RG normal decoding, RNM, the source GGX distribution and Chen's color ramp. `PadmaReferenceSurfaces.ush` owns small eye/brow/shadow responses, face filtering and optional bang opacity. Some explanatory helpers duplicate native graph operations; their presence alone does not prove an active call.

`Tools/lower_reference_graph.py` is a development translator for the audited graph subset. It rejects unsupported live nodes. It is not a general Unreal graph compiler and its source audit is not a runtime dependency. Sorted signatures and matching Custom inputs are one ABI: regenerate and reconnect them together. The Face shader additionally has hand-maintained calibration inputs; preserve those and its helper calls when regenerating. Cloth specializes Chen's audited static switches. Thin-film and some disabled branches are excluded, but source rain arithmetic remains: the original instance's RainStrengh=1 was an active wet response, not a disabled weather system.

The saved project example uses `DA_Chen_NPR_Reference` in `/Game/Sandbox/ACT/Character/ChenQianyu/Art/Profiles`. The existing nine slots of the placed Chen in `L_ChenACT` receive persistent generated MICs. Project textures and the angle curve are copied into project-owned experimental content; originals remain untouched.

The Padma Face profile overrides `Matcap_Color` to black: isolated actual-mesh captures show the unmodified source value creates a sharp white nose patch, and disabling only Matcap removes it while preserving SDF. This is an explicit target-mesh calibration, not a change to the reference formula. The precise mesh attribute responsible is not established; do not claim vertex-color parity between the two meshes.

- Source MI: textures, sampler/encoding and inherited baseline.
- DA slot: `Reference Response = true`, matching Type, source MI, authoritative `Reference Scalars` and `Reference Vectors` overrides. These retain source parameter spelling and semantics; generic typed controls do not also apply.
- Apply: clears previous generated overrides, writes current DA values and binds the generated MIC. Removing a DA override restores source-MI inheritance.
- Runtime: private MID per configured slot; the same maps are applied. No Python import is needed by users or the game.
- Component: per-mesh light/head data. CPD 0..19 remains reserved; index 7 stores the signed SDF curve value, index 11 its validity. A missing curve falls back to the authored `SDF_Location` parameter.

New reference-only surface enum values are rejected in generic mode. The legacy `UPadmaClothProfile` interface remains separate because its packed parameter meanings differ.

## Calibrated dry baseline

Chen Skin and Cloth now explicitly set `RainStrengh=0`. This removes the inherited wet normal/roughness/Matcap contribution; it does not implement runtime weather. Cloth02/HairLine uses its own simpler path and has no such control. Other slot parameters retain their independent bindings; there is no blanket roughness or highlight override.

The profile's `Reference Planar SDF` defaults off for compatibility and is on for Chen. It projects the light onto the calibrated head plane before evaluating the angle curve, and uses the head Up axis for left/right sign. Near vertical light falls back to head forward instead of normalizing a zero vector.

The Face vector `ReferenceFaceControls` is `(Feather, ThresholdBias, OverheadWeight, Debug)`. Chen uses `(.035,0,1,0)` with SDF_Color `(.67,.62,.61,1)`. Feather is a half-width in sampled SDF value space. Values below `1e-5` use the original hard step; otherwise `smoothstep(field-feather,field+feather,threshold+bias)` provides a bounded transition. OverheadWeight fades SDF shadow as dot(L,headUp) approaches 1 above .8; it is an artistic stabilization, not physical visibility. Debug 1 displays the selected filtered field and 2 displays final shadow coverage. Set Debug back to 0 for final rendering. Changing BlurIntensity changes sample spacing, not threshold feather.

Reference controls all zero and Planar SDF off retain the audited source response over its valid threshold range. The generic Face struct fields do not drive these reference controls. Apply the DA after edits. A shader source edit alone does not rebuild an incompatible material Custom signature.

Chen's `ReferenceBangOpacity` is now `.88`, evaluated only inside the front/height/packed-mask region. The master default remains 1. Setting the DA override to 1 restores opaque hair. A short turn/light sweep used .88 without whole-hair disappearance; this is not exhaustive TSR stability or gameplay animation acceptance. Native regression evaluates actual idle poses and verifies that CPD follows the resulting head basis.

| Target slot | Current calibration / retained response |
|---|---|
| body / Skin | Explicit dry baseline; native lit response and source artistic normal controls retained |
| cloth_01 / Cloth | Dry normals/roughness; independent GGX, two Matcap contributions, masking retained |
| cloth_02 / HairLine | Separate line texture, AO/Lam and translucent source response; not assigned Cloth01 shading |
| Face | Planar curve, feather/bias, top-light stabilization, warm shadow; incompatible nose Matcap disabled in this profile |
| Hair | Directional shifted lobe, masks and color ramp; localized .88 bang opacity |
| iris / Eye | Source powered/tinted eye texture, separate binding; no invented POM branch |
| brow / Brow | Source depth fade, opacity and camera-offset geometry behavior retained |
| eye shadow / EyeShadow | Source tint/opacity and shading override retained |
| hair shadow / HairShadow | Source scene-color/HSV/depth overlay retained; no projected shadow pass |

## Actual mathematics, in learning order

1. **Linear color, UV and masks.** Texture color must be distinguished from linear numeric mask data. A packed texture is not automatically ORM. `m=saturate(x*s-o)` is an affine remap followed by clamping; increasing `s` narrows the transition. Source ColorBlend is `lerp(base,lerp(base*tint.rgb,tint.rgb,tint.a),mask)`. Alpha here controls multiplication versus replacement, not transparency.
2. **Vector spaces.** Dot products measure directional agreement, cross products build perpendicular axes, normalization separates direction from magnitude. Native tangent/world transforms must be declared in the material so UE supplies the required basis. World-to-tangent and tangent-to-world multiplication order differs. Hair texture RG decodes to `xy=(2*RG-1)*(1,-1)`, `z=sqrt(saturate(1-dot(xy,xy)))`; this is not UE Normal sampler output and must not be decoded twice.
3. **Normal composition.** Source artistic normals blend camera/light directions toward `(0,0,1)` in tangent space. FlattenNormal is a lerp, not an extra normalize. RNM uses `t=base+(0,0,1)`, `u=detail*(-1,-1,1)`, `normalize(t*dot(t,u)-u*t.z)`. This reorients detail instead of merely averaging normals.
4. **Cel and AO.** Source Lam commonly uses `saturate(dot(N,L)*smooth-offset)` and interpolates shade/base colors. AO has its own mask remap. These artistic bands are not a physically complete BRDF, and their direct-light direction does not supply UE shadow-map visibility.
5. **View rim.** A remapped `dot(N,V)`, inverted and masked, controls edge color. Face additionally gates it by local masks and SDF. This is a stylized rim; do not equate it to a full dielectric Fresnel model or a constant-width screen-space outline.
6. **Face SDF.** Sample B at original and horizontally mirrored UVs with a 3x3 box filter; compare the selected field with the signed angular threshold. The CPU evaluates the copied weighted `UCurveFloat`: `a=acos(clamp(dot(L,headForward),-1,1))/pi`; the sign follows `cross(-L,headForward).z`. Curve keys/tangents, head-axis calibration, sign and mask channel all matter. Source dither controls filter spacing. This is an authored angular shadow field, not ray-traced face occlusion.
7. **Hair anisotropy.** The live source path uses a shifted direction derived from `cross(KKDirection,N)`, a texture shift and view weighting. With half direction H, the lobe is `pow(sqrt(1-dot(T,H)^2),power)` with source masks/limits and a color ramp. Larger power makes a narrower band. Source T is not normalized at that point; silently normalizing changes the reference. Kajiya-Kay-inspired directional response is not the same as isotropic GGX. The BA normal branch in this source is disconnected and is deliberately not presented as active.
8. **GGX distribution.** `D=a2/(pi*((1-NoH^2)+NoH^2*a2)^2)` with `a2=roughness^4`. Source first builds `Q=WorldToTangent(normalize(L+CameraStrength*V))`, then uses the artistic half direction `normalize(N+Q)`. Here N is tangent-space normal, V is world-space view direction, and CameraStrength is `GGX_Camera_Strengh`; this differs from the general physical `normalize(V+L)` half vector. This is the NDF term, not a complete microfacet BRDF: full models also need Fresnel, geometry/visibility and normalization by incident/outgoing cosines. The hybrid cloth/skin material additionally receives stock UE lighting, so double-looking highlights must be diagnosed by layer, not solely roughness.
9. **Matcap.** Build a camera-relative basis, project the normal onto it, then scale/offset into texture UV. The lookup paints a view-dependent response; it is not a real environment convolution. Cloth has two independently masked contributions. Face uses vertex-color masking, making mesh attribute parity part of appearance parity.
10. **Exposure and tone mapping.** Some source paths inverse-map film tone response before artistic operations. EyeAdaptationInverse multiplies by `exposure^(-weight)`. Capture exposure, physical camera options and post process must be controlled before comparing images. A dark render is not necessarily bad shader arithmetic.
11. **Supporting geometry and compositing.** Eye's active path is powered/tinted base texture, not a new Matcap/POM model. Brow uses depth fade and camera-direction WPO. EyeShadow preserves the source MI shading override. HairShadow transforms sampled scene color through HSV and uses background-depth-dependent opacity; it is an overlay, not a projected hair shadow map.
12. **Optional temporal bang opacity.** `1-P.r`, a head-local height gate and front-view weight localize opacity. Native temporal dither converts it to masked coverage. Default opacity 1 preserves the opaque source. TAA/TSR and motion validation are required before opting into lower opacity. This does not implement physically layered hair transmission.

## Scope and acceptance boundaries

ZMD's mesh has 13 slots while Padma's existing combined mesh has nine. Missing independent eye-white, iris-highlight, neck and mouth sections cannot be recreated by changing a scalar. No topology or section changes are made. Existing helper geometry, UVs, normals and vertex colors affect the result.

The reference hair color ramp is currently the audited Chen curve encoded in HLSL. Other characters needing a different ramp need a resource/algorithm variant; this is not yet a fully generic ramp atlas. Custom RDG shadows, Charlie sheen, octahedral corneal normals, POM and post-process/JFA outlines are not implied by this implementation.

Visual comparisons and process logs are under `Artifacts/PadmaNPRVisual`; the owning acceptance record is `Docs/Production/Tasks/TASK-057-NPR-Visual-Reconstruction.md`. Compilation, controlled source-geometry parity, actual Padma presentation, animation stability and packaged performance are distinct checks.
