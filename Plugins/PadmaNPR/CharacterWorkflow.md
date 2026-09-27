# Character authoring workflow v1

Historical interface proposal. The manual interface and real viewport are now implemented; use [ManualAuthoring](ManualAuthoring.md) for current supported behavior. The audit below predates that delivery; richer adapters and in-place regeneration remain deferred.

Status: current implementation audit and next-interface specification, 2026-09-27. [Chinese user guide](CharacterWorkflow.zh-CN.md).

The next delivery is a manual semantic asset interface, not automatic asset discovery. Users select a skeletal mesh, classify its slots, assign textures and their encodings, validate, and generate materials plus a Profile. Automatic matching, outline and a second-character validation follow this interface. Weather and a dedicated preview viewport remain deferred. No engine fork or UE 5.8 Toon BSDF is required.

## What works today

Studio opens at Window > Padma NPR Studio. Select a prepared character Profile in the Content Browser, read the selected asset into Studio, select the intended level actor, and Apply to Actor. Edit Profile values and apply again; save changed assets and the level. Restore Actor restores plugin-owned bindings without deliberately overwriting unrelated user material edits.

The current input is an already configured Profile with source MIs, not a raw mesh plus semantic texture form. The proposed Generate NPR Character button below does not exist yet. Do not describe Apply as texture discovery or full character generation.

Chen uses DA_Chen_NPR_Reference in /Game/Sandbox/ACT/Character/ChenQianyu/Art/Profiles. Its nine reference surfaces use source MIs for textures and DA Reference Scalars/Vectors for artistic overrides. Generic typed fields do not affect Reference Response slots. The legacy PadmaClothProfile is a separate API, not an additional authority for those slots.

The mesh asset retains default materials. Apply writes generated MIC overrides to the actor's skeletal mesh component and those bindings persist with the level. In a game world PadmaNPRComponent creates its own MIDs from the source MIs and writes the same Profile overrides. Changing only a generated MIC is therefore not a reliable way to author a runtime change.

Recent verified corrections: Brow source MI uses the texture paired with the Padma mesh, not the differently packed ZMD face atlas; all nine reference masters and their MI chains have skeletal usage enabled. An actual fresh PIE session checked nine MIDs and produced no missing-skeletal-usage/default-material warning. This is not packaged-game or performance acceptance.

## Existing source responsibilities

| Source | Actual responsibility | Refactoring direction |
|---|---|---|
| Source/PadmaNPREditor/Private/PadmaNPREditorModule.cpp | Slate shell, selected asset/actor actions, DA Details view; dedicated preview is a placeholder | Keep widget presentation and selection here; do not embed asset generation or shader formulas in widget callbacks |
| Source/PadmaNPREditor/Private/PadmaNPRStudioLibrary.cpp | Apply/Restore, generated MIC creation, source-parent cycle checks, parameter copying; separate inverted-red-mask utility | Reuse Apply; add a separate generation service and preflight result instead of growing the widget |
| Source/PadmaNPRRuntime/Public/PadmaNPRProfile.h and Private/PadmaNPRProfile.cpp | Surface settings, slot records, reference maps, validation and parameter packing | One explicit shader variant per slot; expose supported typed controls through a variant adapter |
| Source/PadmaNPRRuntime/Private/PadmaNPRComponent.cpp | Bound mesh, source restoration, runtime MID lifetime, head/light CPD updates | Consume generated assets and resolved parameters; never scan folders, bake textures or depend on Slate |
| Shaders/Private/Padma*Reference.ush and PadmaClothGraphReference.ush | Reachable reference graph arithmetic | Preserve reference output while extracting named math functions; change one surface at a time |
| Shaders/Private/PadmaReferenceCommon.ush and PadmaReferenceSurfaces.ush | Shared reference math and supporting surfaces, face calibration and bangs | Document spaces, encodings and ranges; distinguish live functions from explanatory helpers |
| Shaders/Private/PadmaMath.ush, PadmaClothNormal.ush, PadmaClothLighting.ush, PadmaMatcap.ush | Existing reusable Cloth math | Reuse only after confirming mathematical and encoding equivalence to the reference path |
| Content/Materials and Content/Functions | Saved masters, native material features, resource sampling and Custom adapters | Preserve Custom signature/input ABI and material usage flags when changing HLSL |

The generic Face/Skin/Hair/Eye and legacy Cloth paths are compatibility/prototype paths. Do not silently replace reference slots with them. Reference SSA variable names and source parameter maps are useful evidence but unsuitable as the public artist interface. A typed UI must map to parameters actually consumed by its selected backend; unsupported controls must be disabled or omitted.

## Proposed manual interface

### 1. Character and destination

Select Skeletal Mesh, character/output name, project-owned output folder, and optionally a level actor plus its exact mesh component. For multiple mesh components, require an explicit choice. Read existing slot names from the mesh; never create sections or modify topology. If no actor is chosen, generate assets only and report that placement/application remains required.

### 2. Explicit slot classification

For every slot choose Keep Original, Cloth, Skin, Face, Hair, Eye, Brow, EyeShadow or HairShadow. HairLine remains an advanced reference-specific variant rather than the presumed meaning of a second clothing slot. Display the chosen master/variant and its requirements. Leave unconfigured slots unchanged and report them as such, not as converted.

Initial generator adapters must declare their supported inputs. Current Chen-specialized reference materials are not an arbitrary-PBR backend: a shader that expects custom packed channels or masks must reject incompatible inputs or use an explicitly implemented fallback. A semantic picker alone does not make the shader universal.

### 3. Assign resources by meaning

| Input | User specifies | Default or missing behavior |
|---|---|---|
| BaseColor | Texture or explicit constant tint; UV channel/transform | White only when a constant-color surface is intentional |
| Normal | Tangent-space texture, encoding (standard RGB versus custom RG), green-channel convention, strength, UV | Flat tangent normal when absent; never double-decode a Normal sampler |
| Roughness | Separate texture/channel or packed texture/channel, or constant | Explicit preset constant; Glossiness input requires declared inversion |
| Metallic | Texture/channel or constant | Constant 0 for a nonmetal preset |
| AO | Texture/channel or constant | Constant 1 |
| Opacity/clip mask | Texture/channel, opaque/masked/helper variant | Opaque unless an actual compatible transparency variant is selected |
| Face SDF | Texture/channel, orientation/mirroring convention and threshold mapping | Disable SDF and use a declared basic face fallback, or block a variant requiring SDF |
| Hair direction and shift | Mesh tangent convention or direction texture/space, optional shift/noise mask | Documented tangent-based fallback only where implemented |
| Matcap | Texture plus contribution mask and strength | Disabled when missing |
| Bangs/helper regions | Region mask and head-local constraints; helper mesh/section if required | Feature off when prerequisites are absent |

No fixed ORM assumption. Record independent texture/channel selections even when all three properties share a texture. BaseColor is normally sRGB; scalar masks are linear. Validate source import settings without silently changing a shared user texture. Offer a project-owned converted copy when necessary. Any actual channel repack must define resolution, filtering, encoding and output ownership and preserve separate UV semantics or reject the conversion.

PBR assets produce a basic stylized response only after a compatible generic adapter exists. They cannot supply authored face SDF, correct hair flow, region segmentation or exact reference-character likeness automatically.

### 4. Character frame and style

Select head bone/socket, head forward/up correction, key directional light or fallback direction. Show only relevant requirements: a future cloth-only binding should not need a head bone, whereas the current component validates a head socket for all profiles. Supporting conditional requirements is part of the interface work, not present behavior.

Show Common, Diffuse, Specular, Rim and type-specific groups with units and safe ranges. For reference variants map typed inputs explicitly to source parameters, with raw maps retained under advanced compatibility controls. Never write both a generic control and a contradictory reference override to the same effective setting.

### 5. Validate and Generate NPR Character

The button performs authoring, not lightmap baking. Real-time highlights, SDF evaluation and animation remain dynamic.

Preflight all slots before changing anything: valid assets and output names, collisions/ownership, supported variants, channels and UVs, encodings, required masks, head frame, skeletal usage, and parent/reference cycles. Show blocking errors separately from declared feature fallbacks. Missing inputs must identify their slot and remedy.

Create project source MIs, write texture/encoding bindings, create a character Profile with authoritative artistic values, then generate applied MICs through the existing service. If an actor was selected, bind its component and light. Compile, save only generated/owned assets and explicitly changed scene data, and return exact output paths plus status. Report generation, compilation and scene application separately. On partial failure report completed outputs and keep the previous actor binding until the new set validates; an editor transaction alone does not roll back files already saved.

Suggested output structure: Character/Materials/MI_<Character>_<Slot> as resource sources; Character/Profiles/DA_<Character>_NPR as configuration; Character/Profiles/Generated/ for applied MICs. Do not overwrite originals. Default regeneration reuses only recorded outputs owned by this recipe; unowned collisions require a new destination. Do not manufacture a Blueprint subclass or alter the source SKM by default.

## Parameter authority and service boundary

The proposed editor-only authoring recipe records semantic asset choices, shader adapter version, destination and generated-output identities. It owns regeneration inputs; source MIs are compiled outputs for resources. The character Profile owns artistic parameters. Runtime MIDs own transient overrides such as a flash effect. Recipe edits trigger resource regeneration; Profile edits trigger parameter reapply, not texture rebuild. Generated outputs should link back to the recipe and warn against direct edits that regeneration will replace.

Existing projects without a recipe continue to use source MI texture ownership. Migration must explicitly adopt MI bindings into a recipe before treating them as generated. Do not introduce two competing authorities silently.

Proposed services (not current classes): an editor-only CharacterRecipe and surface input structures; a SurfaceAdapter registry mapping semantics to material parameters and capability requirements; CharacterAuthoringService for Validate/Generate; shared parameter resolution used by editor MICs and runtime MIDs. Studio delegates to those services. Runtime contains no editor recipe dependency. Record the actual serialized-schema decision in an ADR when implemented; this document is the proposed contract, not a migration.

## Delivery order and acceptance

1. Freeze the manual input/output contract and current usage guide (this document).
2. Implement the manual recipe, compatible material adapters, validation and panel interaction; organize Shader functions with visual regression checks. No automatic filename matching yet.
3. Verify a user can select mesh/resources, classify all intended slots, generate, reopen, enter PIE and edit/reapply without Python or material graph work. Regeneration preserves unowned data and removed overrides return to baseline. Capture lighting/camera changes, not only static metadata.
4. Add optional automatic matching on top of the same manual recipe; guesses remain reviewable.
5. Add outline; then validate a second character and clean-project/package behavior. Weather and dedicated viewport are separate deferred work.

## Anisotropy learning note

Anisotropy means the specular response depends on direction along versus across the surface fibers. With light L, view V, half vector H=normalize(L+V) and strand direction T, a simplified strand lobe uses pow(sqrt(saturate(1-dot(T,H)^2)),p). This illustration assumes normalized inputs and is not an instruction to normalize legacy reference intermediates and thereby change their output. Light or view changes normally change the highlight. A fixed direction, painted highlight or Matcap can make a highlight appear more stable; that stability alone is not the definition of anisotropy.
