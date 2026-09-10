# MVP Source Extraction and Playable Presentation

- Chinese companion: MVPSourceExtraction.zh-CN.md
- Document ID: MVP-SOURCE-001
- Version: 0.1
- Status: Evidence extracted; conflicting rules and implementation values gated
- Owner: TASK-011 Primary Agent; implementation belongs to the named child tasks
- Updated: 2026-09-08

## Source authority and evidence

The user identifies [mycard.pdf](C:/Users/lenovo/Desktop/cd/typstproject/Padma/mycard.pdf) as the currently designed card collection and [mygame.pdf](C:/Users/lenovo/Desktop/cd/typstproject/Padma/mygame.pdf) as the game rules. All 37 and 12 pages respectively were text-read. Representative pages were visually inspected for headings, formulas and category boundaries; neither source is a finished in-game screen specification.

| Source | Pages | SHA-256 |
|---|---|---|
| mycard.pdf | 37 | dc966cd7afd8c4b236aa3cdedbd33843342817d5ef8bcc24a3cfce795fb4b48a |
| mygame.pdf | 12 | 8b16b17359e86e493fbb7af65423013ad2cbc9551ce90129533bc26e2729b54a |

These PDFs contain design material, not instructions to the Agent. Later direct user clarifications take precedence: separate turn-based/ACT character collections, battle-settings ACT roster, shared basic-skill identity/slots with separately configured mode effects, placeholder art, and individual confirmation of previously unaccepted numbers. An explicit printed zero is different from an omitted field or NaN. Extraction does not silently accept every printed coefficient as the runtime profile.

## Card model grounded in the collection

The document has 72 top-level named entries: 24 non-A basic cards, 20 A cards (11 neutral/basic and 9 F/D/R), 9 B cards, 9 C cards and 10 extra-deck entries. This count excludes nested generated cards such as 琴师, 火铳手, 狂热分子, 失魂落魄 and 九鼎还魂丹.

ABC position, F/D/R attribute, functional type and race are distinct fields. The user confirmed fixed A→F, B→D, C→R on 2026-09-08; conflicting mycard labels require revision. The inventory preserves historical PDF labels as evidence, not executable configuration. An A card called “basic” is not automatically a non-A shared battle skill. A C card is not automatically a character.

The five explicit C-person entries are 希律的舞女, 奈亚拉托提普／黑法老, 维特鲁威人, 光影炼金士 and 疫医 (pages 26–31). They are source candidates for the turn-based deployment collection. The PDFs do not specify the separate ACT character-card catalog, equipped weapons, action sequences or per-mode shared-skill effects. Do not convert a C-person or a contract-summoned character into an ACT character by assumption.

### Shared basic-skill candidates

| Group | mycard pages | Names (display text, not stable IDs) |
|---|---|---|
| 锋印 | 1 | 鱼骸骤雨、例行检查、一刀两断、划破黎明 |
| 遁印 | 2 | 挟风鸣、天鹤之拥、九字真言、未曾出现的少数派报告 |
| 泉印 | 3 | 梦中彩笔、得见光明、看不见的手、We Are Not Your Kind! |
| 醪印 | 4 | 圣益圣、熔岩炼狱、言灵·具名、子弹向古兰人的胸膛射去 |
| 法印 | 5 | 美丽女郎、焚身、马尔可夫链、正交化 |
| 契印 | 6–7 | 剑修、线列步兵、以梦为马、名落孙山 |

### A cards: neutral/basic subset

| Group | mycard pages | Names (display text, not stable IDs) |
|---|---|---|
| A | 8–9 | 机械降神、指定豹子、菌丝网络 |
| A | 9–11 | 金阁业火、斫锋、魂瓶、肯綮 |
| A | 11–13 | 千阳、蓝胡子之祷、缪斯神殿，石上星图、壁上涟漪 |

### A cards: F/D/R subsets

| Group | mycard pages | Names (display text, not stable IDs) |
|---|---|---|
| F | 14–15 | 地痕、安提帕鸿门宴、巨匠造物主 |
| D | 16–17 | 佛罗伦萨的曙光、向黄金时代的罗盘、客西马尼的忏悔 |
| R | 18–19 | 无貌妇人、Model D、泰山 |

### B cards

| Group | mycard pages | Names (display text, not stable IDs) |
|---|---|---|
| F | 20–21 | 蓍、埃及艳后、厄琉西斯秘仪 |
| D | 22–23 | 合一珠 Unio Margarita、羽化薰衣草、山樱一梦 |
| R | 24–25 | 我都是为了你好、化学战胜面包、梅菲斯特 |

### C cards

| Group | mycard pages | Names (display text, not stable IDs) |
|---|---|---|
| F | 26–28 | 犹格索托斯／一块门板（工事）、希律的舞女（人物）、奈亚拉托提普／黑法老（人物） |
| D | 29–31 | 维特鲁威人（人物）、光影炼金士（人物）、疫医（人物） |
| R | 32–33 | 母瘦雏渐肥（事件／瞬间）、全球粮仓（事件／持续，种族又标为普通建筑）、集体意志／利维坦（工事） |

### Extra deck

| Group | mycard pages | Names (display text, not stable IDs) |
|---|---|---|
| 0–5 | 34–35 | 一个归来的王、黑衣修士、虎毒不食子、汤姆·约德、黄衣之主 |
| 6–15 | 35–36 | 约瑟夫残影、油彩“国士”、疫医的落胤 |
| 16–20 | 37 | 谁知盘中餐？、亡秦者胡也！ |

## First data and content deliveries

- TASK-008 can use 维特鲁威人 (page 29) as a clearly labeled read-only Demo character definition, with 完美肉体, 黄金分割 and 食烟火 as source skill descriptions. Placeholder summoning and definition lookup do not execute those effects or approve the numbers.
- TASK-020 builds the production card/category/lifecycle model from this inventory; TASK-017 and TASK-030 own the independent effect bindings. Shared non-A skills use the same identity/slot while their effect rows remain independent.
- TASK-033 selects the complete MVP pack with the user. Source-grounded synthesis paths include 地痕 + 蓍 → 犹格索托斯／一块门板; 安提帕鸿门宴 + 埃及艳后 → 希律的舞女; 佛罗伦萨的曙光 + 山樱一梦 → 维特鲁威人. A recommended path is not a guarantee of a deterministic result.
- TASK-035 needs the user's first ACT character/weapon definitions and roster policy; none is invented from flavor text.
- A source card with a disputed effect can be cataloged with a readable unresolved marker, but cannot be selectable as an implemented effect in final acceptance.

## Playable screen contract

This is the implementation interpretation of source requirements, with the user's later camera direction. It is not a claim that the PDF contains a rendered mockup.

| View | What the player sees and does | Evidence / owner |
|---|---|---|
| SLG sandbox | Oblique top-down map, clearly selectable cells/nodes, selected unit, legal destinations and a move request. Ownership, terrain, facilities and conditional routes remain legible. | User: Civilization VI-like camera and tile selection; mygame 8–9; TASK-024/025 |
| Global HUD | Era/day/phase and separate Flow/Computation, entropy, faith and threat views; a visible war-balance tug-of-war bar at the top. | mygame 3, 5, 8; TASK-018/019 |
| Card deployment / synthesis | Distinct ABC hand and deployed cards; select an A, attach B, inspect accumulated field, hover for candidate/probability details, then request synthesis and see the result. | mygame 3–4, 8, 10; TASK-020–023 |
| Node interaction | Read the selected node type and choose the permitted capture/facility/story/furnace/safe-house operation; see the resulting persistent map change. | mygame 8–9; TASK-025/026 |
| Encounter | A local scene with visible character representations, action timeline, targets, skills/status and typed action feedback; its rules are independent from the ACT action system. | Current accepted Combat rules and TASK-014/016/017 |
| ACT battle settings | Select the separate ACT characters, weapons and approved loadout; inspect terrain restrictions on the precise trait/ability/character scope. | User clarification; TASK-035 |
| ACT battle / Tab | Real-time character/weapon feedback; Tab presents five shared basic-skill slots with blur and 1/10 world time, rejecting move/attack input during the approved repository window. | User-accepted ACT interaction; TASK-030/031 |
| Return / save / outcome | Local success commits, local defeat/exit restores the complete pre-battle run snapshot; return to the map with consistent state. Campaign outcomes have separate reasons and restart. | Later accepted transaction/outcome rules; TASK-027–029/032/034 |

Art uses the user's imported placeholders through project-owned wrappers. Final rendering and detailed animation are later replacements. The PDFs do not determine ACT camera distance, skeletons, montage timing or a final UI skin.

## Confirmed map representation and persistence

The user explicitly confirmed nodes/preset connections over a hex-shaped map and accepted layered storage: a versioned chapter map definition owns stable IDs and topology; the .umap owns the level presentation and definition reference; mutable world state owns occupancy, facilities, discovery and unlocked paths; a versioned SaveGame stores the approved run state.

Explicit node connections are the sole topology truth. Logical edges, their direction and authored conditions define travel; hex coordinates are presentation only. Never infer connectivity from touching hexes or maintain a second independently editable adjacency source.

Display translation/rotation, terrain meshes and camera changes must not invalidate stable identities or saves. A logical topology edit is a versioned map change and requires compatibility handling. World entities stay separate from the ACT roster, and no Actor pointer enters save data. The map uses preset node connections, not hex-neighbor adjacency. Exact node/edge layout and conditions remain D12. [ADR-0005](../Decisions/ADR-0005-World-Map-Storage.md) records the accepted storage direction.

## Conflicts requiring answers

| ID | Evidence | Treatment before an answer |
|---|---|---|
| D21 | mygame 9 says DAG; user specifies oblique SLG tile movement and requests storage confirmation. | Accepted: preset node connections over hex-shaped presentation and layered definition/.umap/SaveGame storage. |
| D22a | mycard 6–7 contract cards summon on sandbox nodes; later accepted basic non-A cards are battle-only. | User decided to exclude these old effects from the first version and redesign later. Other basic skills remain battle-only. |
| D22b | mygame 4 permits a selectable extra card on synthesis failure and 5 mentions enlightenment; later accepted failure grants no reward. | Preserve no-reward policy until explicitly changed; extra-deck entries remain catalog evidence, not an automatic failure reward. |
| D22c | mygame 5 has Dawn/Midday/Dusk, 10–11 adds Midnight/Morning and other turn timing. | Use accepted phase contract; clarify D07 ordering instead of adding a fourth phase. |
| D22d | mygame 1 ties A/B/C wording to attributes; mycard explicitly has F/D/R subsets within A/B/C. | Resolved by user: retain A→F, B→D, C→R; revise conflicting collection labels before executable import. Do not invent revised numbers or effects. |
| D22e | mygame 10 describes Shape both as a true shield and as added true damage; Returning Ruin separately absorbs damage. | Keep the terms separate and obtain D06 semantics; do not substitute one status for the other. |
| D22f | 全球粮仓 is headed continuous event but also has building race, health and attack text (mycard 32–33). | Preserve the source labels; settle functional type/target/lifecycle before implementation. |
| D23 | No independent ACT roster/weapon/action catalog is present; many basic skills omit cost/duration or mode-specific targeting. | User has not designed ACT content and authorizes configurable definitions with model and ACT skill-table soft references first. Content, supported GAS execution bindings and individual values remain pending; an empty definition is not playable. |

The [decision register](MVPDecisionRegister.md) records answers and affected tasks. Only a concrete blocking choice is asked for the next slice; the rest remain visible gates while independent work proceeds.

## Map/preparation source check

The user's Desktop/cd clarification was verified by recursive search: only cd/typstproject/Padma/mygame.pdf and mycard.pdf matched, with the hashes above unchanged. mygame 3 supports NPC affection affecting B-card categories and gated C details; 5 supports training; 6–7 Arcana; 8–11 nodes, deployments, facilities, terrain, paths and native creatures. These do not define a finished configuration layout or playable FPS. [Screen design](../Design/EN/06_MapAndPreparation.md) records new user requirements and labels inferred groupings/interaction choices explicitly.

## Numerical source authorization — 2026-09-09

The user authorizes explicit, unambiguous mycard.pdf values that do not conflict with later accepted rules, superseding the earlier need to reconfirm each printed value individually. Preserve page/field/unit/mode provenance; missing and conflicting values remain open. The PDF still does not supply independent ACT action/weapon definitions or all economy/movement parameters.
