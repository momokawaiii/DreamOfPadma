# Chen ACT and toon-rendering lab

Chinese summary: [ChenACTRenderLab.zh-CN.md](ChenACTRenderLab.zh-CN.md).

## Entry and ownership

Open `/Game/Sandbox/ACT/Training/Maps/L_ChenACT`.
This single map is both the existing three-target ACT training arena and the user's
hands-on toon-material study environment. Do not create a separate teaching map or
automatically transplant ZMDRender materials. The user builds character materials
and Blueprint connections step by step, copying individual source textures only
when needed. Existing combat, animation, showcase and recording behavior remains.

The user-selected experimental root is `Content/Sandbox/ACT`. Character rules,
animation and art now use the [canonical role-based layout](ACTAssetLayout.md).
Historical Actions/Attack01/Centimeter directories are retired; live first-attack
behavior and stable IDs remain. Weapon and training assets have separate owners.
This does not itself establish shipping/cook acceptance.

## Environment reference

- `ACT`: existing lab actor, three real damage targets, B analytics/showcase and F8 reset.
- `RenderLab` reconstructs the lighting, ground and background of
  `E:/2026ue/Toon/Content/Render/Map_Main.umap`, replacing the initial reference-sphere setup.
- Key light: movable, intensity 6, temperature 6500 K, rotation
  (Pitch -25.281711, Yaw -53.515924, Roll -10.599011).
- Skylight: stationary, specified `/Engine/EngineResources/GrayLightTextureCube`,
  intensity 0.2, indirect intensity 0.5, black lower hemisphere.
- Exposure: Manual, compensation 0, physical-camera exposure disabled; source
  environment GI/reflection overrides use Lumen. Source character rim-light PP is
  deliberately not imported: the user will build character shading separately.
- Background: ExponentialHeightFog at Z -30, density 0.02, height falloff 0,
  inscattering luminance (0.755208, 0.755208, 0.755208), opacity 1.
- Ground: source SM_Ground at scale 3 with M_Ground_Inst, over the engine Plane at
  scale 100 with M_Ground_01_Inst. Seven environment dependencies were copied and
  moved with UE AssetTools to `Training/Environment/ToonReference`; the source
  project was not modified. Source meshes/materials/instances/textures are retained.
- Both visual ground layers have NoCollision. The original training floor remains
  at its original transform with collision enabled and rendering disabled, preserving
  the arena's movement/landing surface. The visual extension does not expand the arena.
- The initial added fill light and reference spheres are removed from the map.
  Their three calibration material assets remain available, unreferenced by this map.
- B showcase retains its authored depth of field. Use the normal ACT camera when
  judging a no-showcase baseline; keep camera, light and exposure consistent between captures.

## Learning in this map

1. Open the map **without Play**. `ACT/Participants` contains the placed Chen Blueprint
   and three wooden targets. They already render in the editor viewport. Rotate the
   directional light or edit the character Mesh material overrides here and save normally.
   Enter PIE only to test combat, B showcase and F8.
2. In the Outliner, use the `RenderLab` folder to find the key light and exposure volume.
   Keep exposure fixed while rotating the key light to inspect the shadow boundary.
   Persist lighting edits outside PIE; PIE-only edits are discarded on stop.
3. Author character materials under `Art/Materials`, reusable functions under
   `Art/Materials/Functions`, and required source textures under `Art/Textures`.
   Create those folders when the first real asset is needed. Do not overwrite the
   original materials until the relevant learning step is verified.
4. Use the existing `Art/Meshes/SK_Chen_FullCharacter_CM` and inspect actual section
   assignments before binding a new material. Padma has nine slots, while the
   reference character has thirteen. Face/eyes/mouth/neck mapping needs a triangle/UV
   check before full reproduction; adding empty slots is not a fix.
5. Progress through base color, controlled light/shadow masks, face SDF, hair highlights,
   cloth/skin/eyes, geometry outlines and screen-space edge lighting on this same map.
   Finally check moving ACT and B turn/showcase transitions.

## Recipe and evidence

### Wooden training targets

The three placed targets use `Content/Sandbox/ACT/Training/WoodenDummy`.
`Art/Meshes/SK_WoodenDummy` and `Animation/Sequences/AS_WoodenDummy_Hit` are project copies of
Wjgz's `Me_02/受ji/木头人` and `SQ_hit` (0.6667 seconds, matching skeleton).
The project material uses the source color/normal textures with roughness 0.85;
it avoids the source demonstration's shared dissolve collection. Source files remain unchanged.

`Scripts/Editor/ConfigureChenWoodenDummies.py` binds these assets to the existing
lab and preserves its legacy combat profiles and environment. In the placed map,
edit each target's Mesh component, material slots and transform directly; edit its
`Combat / Scene / Scene Spec` for health, defense and other combat values. The lab's
Training Dummy Hit Animation selects the shared hit reaction. The old Training Dummy
Mesh/Transform and Training Targets fields are bootstrap/legacy-spawn settings, not
per-play overrides of the placed actors.
The presentation component reacts only to the target's own damage receipts, restarts
on repeated hits and returns to the clip's first pose after playback. It does not
change settlement or capsule movement. F8 resets the same actors and restores their
initial actor transforms, without replacing authored body meshes/materials. Raw provenance,
configuration, build and regression evidence are under `Artifacts/TrainingDummy`.

### Placed character ownership and editable fields

- Player Blueprint: `Blueprints/BP_ChenACTPlaced`, derived from the existing `BP_ACT_CHEN`.
- Target Blueprint: `/Game/Sandbox/ACT/Training/WoodenDummy/Blueprints/BP_ACTWoodenDummy`.
- Lab `Scene Player` and `Scene Targets` reference the exact actors in the level;
  `Require Scene Participants` is enabled. Missing/invalid references are errors, not
  permission to spawn replacements. Scene Spec on the player owns the ACT definition.
- Appearance: actor Mesh/Materials/Animation Class, capsule and component transforms.
  Use compatible skeletons for the existing action Montages. Scene-tagged weapon
  components retain their authored meshes/materials and mount corrections; draw/sheath
  and dissolve are still runtime presentation behavior.
- Rules: Character/Skill DA and skill table; windows remain editable Montage AN/ANS.
  Activation, GAS effects, movement, hit queries and reset lifecycle remain native C++.
- `Apply Scene Presentation` explicitly initializes from the Scene Spec's UE assets.
  It resets model/ABP/default mesh offsets and weapon presentation; do not press it
  merely to save hand-edited overrides. Normal PIE/reset does not call it.

`Scripts/Editor/ConfigureChenSceneParticipants.py` seeds missing Blueprint instances
from existing Content and saves this map. Reruns preserve existing participants and
their appearance/layout. `VerifyChenSceneParticipants.py` reloads the map read-only,
checks actor/default/weapon persistence and traverses Asset Registry package references.
Use the same commandlet invocation as the environment recipe below, changing the script.
Evidence and the original map backup: `Artifacts/ChenSceneParticipants`.

### Artifacts is not a runtime dependency

Opening, editing and playing this map uses native code and UE Content only. Daily
`ConfigureChenCombatIdle.py`, `ConfigureChenWeaponLifecycle.py`, `ConfigureChenLocomotion.py`
and `BindChenActionFX.py`
require already-authored Content assets, not Unity bake reports or FBX caches. Missing
assets produce explicit errors. FX Duration reads versioned
`Scripts/Editor/ChenActionFXEmitterDurations.tsv`, not an Artifacts report.
Existing material graphs and lifecycle tuning are retained by these configuration paths.
The combat-idle, weapon-lifecycle and FX-binding scripts accept `-PadmaValidateOnly`
on the UE command line to validate required UE assets/references without authoring.
Reports are written under `Saved/ChenAuthoring`. Locomotion remains an explicit graph/
root-motion authoring recipe, not a no-op validator or a general tuning-preserving sync.
Binding/authoring recipes remain deliberate editing operations, not automatic startup
steps; there is no need to run them to preview or play. Explicit offline extraction/
reimport tools may still consume source files. Historical import-source metadata is
provenance, not a file read required by the runtime. Artifacts backups/evidence are not deleted.

For an editor-world D3D12 capture without PIE, launch UnrealEditor with
`-ExecCmds="py <absolute VerifyChenSceneParticipants.py path>" -PadmaCapturePreview -RenderOffscreen`.
It captures the unsaved preview view and exits automatically. The saved evidence
`Artifacts/ChenSceneParticipants/editor-preview.png` confirms visible placed models;
it does not establish final toon-shader parity.

`Scripts/Editor/ConfigureChenACTRenderLab.py` configures the existing map. Run with
UE 5.8 `UnrealEditor-Cmd <uproject> -EnablePlugins=PythonScriptPlugin,EditorScriptingUtilities
-run=pythonscript -script=<absolute script path> -unattended -nop4 -NullRHI`.
It requires the seven environment dependencies already present in the agreed
destination. It is repeatable, preserves character-definition/target settings,
and replaces only known actors from the superseded setup. Re-running restores
the reference lighting baseline; it does not rebuild the user's character materials.

Directory-move backup, asset/reference inventory and verification reports are in
`Artifacts/ChenACTLayout`; the Toon scene export, copied-dependency hashes, previous
map backup, fresh verification and GPU screenshot are in its `ToonReference` subfolder.
The raw ZMDRender audit is in `Artifacts/ZMDRenderAudit`;
its earlier migration recommendation is superseded by the manual-learning decision above.

Headless authoring and load checks are not visual acceptance. Actual rendering,
exposure suitability and material parity require the visible UE viewport. Current
delivery evidence and remaining acceptance belong to TASK-056.
