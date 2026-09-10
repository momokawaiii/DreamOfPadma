# ACT Character and Skill Authoring

- Chinese companion: ACTAuthoring.zh-CN.md
- Document ID: CONTENT-ACT-AUTHORING-001
- Version: 0.1
- Status: Authoring implementation; ACT gameplay pending
- Owner: TASK-036

## Editable assets

Open Content/Padma/MVP/Definitions/ACTCharacterCards in the Content Browser. Duplicate the templates before filling them. To create a new asset, choose Miscellaneous → Data Asset and the corresponding native class.

| Asset / native type | Fill in |
|---|---|
| DA_ACTCharacter_Template / PadmaACTCharacterDefinition | Stable DefinitionId, ContentVersion, DisplayName, skeletal Model soft reference, optional AnimationClass soft reference, ACT SkillTable soft reference, optional TraitIds and explicit TerrainRestrictions |
| DT_ACTSkills_Template / DataTable, PadmaACTSkillRow | Per row: SkillId, ActivationBindingId, soft Definition reference to an ACT skill asset |
| DA_ACTSkill_Template / PadmaACTSkillDefinition | Stable DefinitionId matching the row, DisplayName, ACT AbilityImplementationId; optional Montage soft reference |
| DA_ACTWeapon_Template / PadmaACTWeaponDefinition | Stable DefinitionId, DisplayName, exactly one StaticModel or SkeletalModel; optional AttachmentSocket |
| DA_ACTAuthoring_Template / PadmaACTAuthoringCatalog | Character/weapon soft references plus read-only authoring vocabulary for terrain, activation bindings and ability implementations |

IDs identify content independently of the display name, model path or asset filename. Do not recycle an ID for a different definition. ContentVersion records authored revisions; it is not an ability level or cost. These ACT character assets are not A/B/C cards.

The character model and animation Blueprint/montage must share a skeleton in this slice. Retarget incompatible assets explicitly before assigning them. An omitted AnimationClass permits a static authoring preview; animation and action readiness remain later runtime checks. A weapon socket is checked against the selected character during roster/runtime composition, not against an invented default character.

## Validation

Select the catalog asset and press **Validate Authoring** in Details. Inspect Output Log entries beginning **[ACT Authoring]**. Blueprints and automation can also call ValidateCatalog and inspect bValid / Errors.

The checker loads typed references, checks required IDs/display fields, nonempty ACT skill tables, matching skill identities, duplicate identities/bindings, declared executor/input IDs, skeleton compatibility and trait/terrain references. Empty templates intentionally fail until filled. Existing assets are preserved if the authoring scripts are rerun.

TerrainRestrictions names a declared TraitId and explicit DisallowedTerrainIds. It does not ban the whole character and it does not choose when battle rules apply. Those semantics belong to the approved ACT runtime/roster policy.

## What becomes usable after filling

The definitions become available to the authoring catalog and later ACT roster/loader. A table row describes how to find a skill and its logical trigger; it does not implement a new action. TASK-030 must register the corresponding native GAS ability executor, load the configured content and connect input/animation/effects. TASK-035 supplies battle settings and roster validation.

Reference-catalog IDs are declared authoring vocabulary, not proof that an executor is installed. Authoring success therefore does not mean a playable ACT battle. Damage, cost, cooldown, windows, weapon policy and all unaccepted values remain unset for the user to confirm. No Encounter table is used as an ACT fallback.

## Reproducible authoring

The checked-in Scripts/Editor/AuthorACTTemplates.py creates five project-owned templates via UE Editor Python. It preserves existing assets and checks that empty templates remain incomplete. Enable PythonScriptPlugin only for the commandlet process if it is not enabled in the project. Do not copy third-party content into these templates; reference user-imported assets or project wrappers instead.

Learning check: replace only Model and predict whether DefinitionId or the skill bindings change. Then inspect the asset. User evidence has not yet been provided.

## Scene preview integration

TASK-040 reads these same ACT character/weapon model fields and the selected character's SkillTable; there is no second copy. Character Mobility is new metadata only and does not enable sandbox deployment. The native model/scene preview guide is [ModelAuthoring](ModelAuthoring.md). ACT has no sourced description/numerical row schema yet; a readable row is not a registered GAS action.
