# Model and Node Presentation Authoring

The standalone model preview map and its placeholder catalog were retired. Configure the definitions used by L_PadmaWorld or Chen L_ChenACT directly; see [retained scope](../Production/ProjectCleanup.md).

## Main-map models

Use the presentation catalog referenced by the playable content catalog. Entries retain domain-qualified stable IDs and typed model/ACT character/weapon references. Bind these identities to node scene slots rather than treating a placed actor or display name as gameplay identity. Use a general model definition for scenery and buildings; do not invent an ACT skill table for non-combat scenery.

Validate the catalog and the node scene definition after edits. Check duplicate identities, unresolved definitions, slot compatibility and required models. Scene presentation owns visuals; runtime services own placement and combat rules. Test replacements in L_PadmaWorld with a fresh run.

## Chen

Edit `/Game/Sandbox/ACT/Character/ChenQianyu/AbilitySystem/DA_ACTCharacter_CHEN` and its equipment profile. Models, skeletons, animation classes and weapon sockets must agree. Test appearance, action transitions, weapon visibility and the training panel in L_ChenACT. See [ACT authoring](ACTAuthoring.md) and [render lab](ChenACTRenderLab.md).

The reusable native presentation/preview types remain because the main map and editor tools use them. An isolated preview being removed does not authorize removing these shared types. Preview validity, animation validity and playable combat are separate checks.
