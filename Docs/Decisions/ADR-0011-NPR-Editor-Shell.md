# ADR-0011: Independent NPR Editor Shell

- Date: 2026-09-22
- Status: Accepted for the editor shell; rendering architecture remains proposed
- Chinese summary: [ADR-0011-NPR-Editor-Shell.zh-CN.md](ADR-0011-NPR-Editor-Shell.zh-CN.md)

## Decision

Start PadmaNPR as a project plugin with one Editor-only module, independent of the DreamOfPadma module. Register a dockable Slate workspace through the Window menu. The initial UI supports read-only inspection of an explicitly selected asset and opening its native editor. It does not apply configuration, change materials or worlds, or automatically import reference projects.

Keep rendering algorithms, runtime profile schemas, GPU transport and custom passes unimplemented until the user learns and validates the corresponding feature. Pending pages explain their future responsibilities without presenting functional-looking parameter controls. Ordinary Substrate is allowed; UE 5.8's new Toon BSDF and associated built-in profile/atlas are excluded.

## Consequences

The user can learn algorithms independently while the editor entry point already exists. The generic inspector does not establish an editable Profile contract. Future typed configuration must distinguish shared style, character overrides, binding and runtime state, with a single authority for each parameter. Adding runtime/render modules requires a later concrete dependency decision. The plugin currently has no shipping functionality or gameplay dependency.

The module does not support dynamic reloading. Shutdown clears any live Studio tab content before requesting closure, including when the tab is locked. Restart the editor after native module changes.

Usage and verification instructions live in [the plugin README](../../Plugins/PadmaNPR/README.md).
