# Padma NPR Studio

The [manual authoring interface](ManualAuthoring.md) is now implemented: semantic slot inputs, actual skeletal preview, **生成 NPR 角色**, saved recipes and Profile application. The reference workflow remains available through the top-right existing-DA action.

Historical interface proposal: [Character workflow v1](CharacterWorkflow.md): current usage, Shader/C++/Studio ownership, and the proposed manual mesh/semantic-texture generation interface. Manual authoring precedes automatic matching, outline and second-character validation.

The saved Chen example now uses the nine-slot [reference character variant](ReferenceCharacter.md). Its DA maps preserve source parameter semantics; the earlier generic character templates below remain available. See the [Chinese learning order](ReferenceCharacter.zh-CN.md) for the implemented equations and their limits.

The plugin now includes the [minimal character workflow](Character.md), alongside the existing [Cloth core](Cloth.md).
The separate [reference Cloth variant](ReferenceCloth.md) adds a source-derived masked response and the saved Chen cloth_01 configuration; legacy templates remain available.
The [architecture guide](Architecture.md) lists files, material functions and project integration; see also its [Chinese summary](Architecture.zh-CN.md).
Open **Window > Padma NPR Studio** in the Level Editor. Chinese summary: [README.zh-CN.md](README.zh-CN.md).

## Available

- The existing-DA inspector retains five workspace tabs: overview, character configuration, material study, preview/debug, and tools. Native AppStyle toolbar, left preview placeholder/workspace splitter, right Details inspector and collapsible category headers follow the material-instance editor's visual organization. The placeholder is not a rendered scene.
- Read a Content Browser selection; edit character DAs in Details.
- Open the inspected asset in its native editor; clear the inspector without changing the asset.
- Clear descriptions of pending capabilities, without inactive shader controls disguised as working features.

Studio Apply/Restore explicitly changes only configured slots on the selected level actor. The separate explicit Cloth authoring command creates new plugin templates; the runtime factory writes resolved artistic parameters into a newly created MID. Opening an asset editor allows normal intentional edits.

## Extension boundary

`PadmaNPREditor` depends on `PadmaNPRRuntime` and engine editor modules. Runtime owns shader registration, Cloth and character Profiles, the MID factory and character component. Neither depends on DreamOfPadma gameplay. Character component binding is implemented; custom passes remain unimplemented. The new manual interface has a real skeletal preview scene.

The inspector edits character Profiles and browses other assets. Cloth Profiles currently use the native Data Asset editor. Add a Renderer module only when a real pass requires it. Ordinary Substrate is permitted; UE 5.8 Toon BSDF and its built-in Toon profile/atlas are excluded.

The plugin can be copied to another compatible UE project's Plugins directory; enable it and compile. The UI does not hardcode local Toon/ZMDRender disk paths. Cross-project installation and packaged builds are not yet validated.

## Checks

Native module changes require an editor restart; dynamic module reload is disabled. UE 5.8 Editor build, StudioLifecycle automation with all five page captures, strict project validation and documentation audit passed on 2026-09-22. Selection/open/clear interactions still need a manual usability pass. Cloth has separate compile/GPU/ownership checks described in its guide. Character visual acceptance is still pending.

Build the host's Editor target. Run `PadmaNPR.Editor.StudioLifecycle` through Session Frontend automation to check tab registration, reuse, close and reopen. Optional command-line flag `-PadmaNPRCapture` also cycles five pages in a test window and writes real Slate screenshots to `Artifacts/PadmaNPRStudio`; use a rendering RHI for that mode. The test does not save assets or maps.
