#pragma once

#include "CoreMinimal.h"
#include "Core/Content/PadmaContentTypes.h"
#include "PadmaRunRules.generated.h"

USTRUCT()
struct DREAMOFPADMA_API FPadmaCalendarState
{
    GENERATED_BODY()
    UPROPERTY() int32 Chapter = 1;
    UPROPERTY() int32 Era = 1;
    UPROPERTY() int32 Day = 1;
    UPROPERTY() int32 Phase = 0;
    UPROPERTY() int32 AbsoluteDay = 1;
};

USTRUCT()
struct DREAMOFPADMA_API FPadmaResourceState
{
    GENERATED_BODY()
    UPROPERTY() double Flow = 0;
    UPROPERTY() double Calc = 0;
    UPROPERTY() double Faith = 0;
    UPROPERTY() double Entropy = 0;
};

USTRUCT()
struct DREAMOFPADMA_API FPadmaCardInstance
{
    GENERATED_BODY()
    UPROPERTY() FName Id;
    UPROPERTY() FName DefId;
    UPROPERTY() FName Zone;
    UPROPERTY() FName Node;
    UPROPERTY() FName AttachedTo;
    UPROPERTY() FVector2D Field = FVector2D::ZeroVector;
    UPROPERTY() double HP = 1;
    UPROPERTY() double Attack = 0;
    UPROPERTY() int32 DeployedDay = 0;
    UPROPERTY() int32 LastAccumulatedDay = 0;
    UPROPERTY() FString ActiveEra;
};

USTRUCT()
struct DREAMOFPADMA_API FPadmaWorldNodeState
{
    GENERATED_BODY()
    UPROPERTY() FName Owner;
    UPROPERTY() bool bUsed = false;
    UPROPERTY() bool bVisited = false;
};

USTRUCT()
struct DREAMOFPADMA_API FPadmaCardMobilityPolicy
{
    GENERATED_BODY()
    UPROPERTY() FName Mode = TEXT("movable");
    UPROPERTY() double MinFaith = 50;
    UPROPERTY() FString ForbiddenTerrain;
};

USTRUCT()
struct DREAMOFPADMA_API FPadmaPreparationState
{
    GENERATED_BODY()
    UPROPERTY() FName ACTCharacter = TEXT("traveller");
    UPROPERTY() FName ACTWeapon = TEXT("blade");
    UPROPERTY() double ACTHP = 50;
    UPROPERTY() FName HomeCharacter = TEXT("keeper-a");
    UPROPERTY() FName HomeOutfit = TEXT("travel");
    UPROPERTY() FName HomeWeapon = TEXT("sword");
    UPROPERTY() FName HomePose = TEXT("idle");
    UPROPERTY() FName FPSCharacter = TEXT("fps-vanguard");
    UPROPERTY() FName FPSWeapon = TEXT("rifle");
};

USTRUCT()
struct DREAMOFPADMA_API FPadmaSynthesisCandidate
{
    GENERATED_BODY()
    UPROPERTY() FName Id;
    UPROPERTY() double Score = 0;
    UPROPERTY() double Probability = 0;
    UPROPERTY() double Distance = 0;
    UPROPERTY() double Affinity = 0;
    UPROPERTY() double Entropy = 0;
    UPROPERTY() double Faith = 0;
    UPROPERTY() double RawSuccess = 0;
    UPROPERTY() double Success = 0;
};

USTRUCT()
struct DREAMOFPADMA_API FPadmaSynthesisRecord
{
    GENERATED_BODY()
    UPROPERTY() FName A;
    UPROPERTY() FName B;
    UPROPERTY() FName Node;
    UPROPERTY() FVector2D Field = FVector2D::ZeroVector;
    UPROPERTY() TArray<FPadmaSynthesisCandidate> Candidates;
    UPROPERTY() FName Selected;
    UPROPERTY() FName CreatedId;
    UPROPERTY() double Flow = 0;
    UPROPERTY() double Calc = 0;
    UPROPERTY() double SelectionRoll = 0;
    UPROPERTY() double SuccessRoll = 0;
    UPROPERTY() bool bSuccess = false;
};

USTRUCT()
struct DREAMOFPADMA_API FPadmaRunLogEntry
{
    GENERATED_BODY()
    UPROPERTY() int32 Id = 0;
    UPROPERTY() FString Text;
    UPROPERTY() FName Kind = TEXT("info");
};

USTRUCT()
struct DREAMOFPADMA_API FPadmaBattleRecord
{
    GENERATED_BODY()
    UPROPERTY() FName Mode;
    UPROPERTY() FName Node;
    UPROPERTY() int32 Actions = 0;
    UPROPERTY() double Seconds = 0;
};

/** Stable values only. Never contains actors, widgets, transient combat state or object references. */
USTRUCT()
struct DREAMOFPADMA_API FPadmaRunState
{
    GENERATED_BODY()
    UPROPERTY() int32 Version = 1;
    /** Frozen definition for this run; regeneration never changes a running map. */
    UPROPERTY() FPadmaMapLayout MapLayout;
    UPROPERTY() TArray<FName> SeenNPCs;
    UPROPERTY() TArray<FName> CompletedNPCs;
    /** Once-only checkpoint decisions, not a live reevaluation of later NPC visits. */
    UPROPERTY() TMap<FName, bool> StoryCheckpoints;
    UPROPERTY() TMap<FName, double> Profile;
    UPROPERTY() uint32 RNG = 731;
    UPROPERTY() int32 Sequence = 0;
    UPROPERTY() int32 NextId = 1;
    UPROPERTY() FPadmaCalendarState Calendar;
    UPROPERTY() FPadmaResourceState Resources;
    UPROPERTY() double RulerFaith = 60;
    UPROPERTY() double PlayerCoreHP = 30;
    UPROPERTY() double RulerCoreHP = 30;
    UPROPERTY() double War = 0;
    UPROPERTY() FName Outcome;
    UPROPERTY() TArray<FPadmaCardInstance> Cards;
    UPROPERTY() TArray<FName> Deck;
    UPROPERTY() TArray<FName> Hand;
    UPROPERTY() TArray<FName> Discard;
    UPROPERTY() TMap<FName, FPadmaWorldNodeState> Nodes;
    UPROPERTY() TArray<FName> Unlocks;
    UPROPERTY() bool bStoryMet = false;
    UPROPERTY() TArray<FPadmaSynthesisRecord> SynthesisHistory;
    UPROPERTY() TArray<FPadmaBattleRecord> BattlesWon;
    UPROPERTY() TArray<FPadmaRunLogEntry> Log;
    UPROPERTY() TArray<FName> SkillSlots;
    UPROPERTY() TArray<FName> SkillsAvailable;
    UPROPERTY() TArray<FName> SkillsDiscard;
    UPROPERTY() int32 SkillCycles = 0;
    UPROPERTY() FPadmaPreparationState Preparation;
    UPROPERTY() TMap<FName, FPadmaCardMobilityPolicy> Mobility;
};

enum class EPadmaRunCommandEffect : uint8 { None, WorldChanged, BattleRequested, BattleCommitted, BattleRolledBack, DialogueChanged };

struct DREAMOFPADMA_API FPadmaRunCommandResult
{
    bool bOk = false;
    FString Text;
    EPadmaRunCommandEffect Effect = EPadmaRunCommandEffect::None;
};

struct DREAMOFPADMA_API FPadmaCardMoveCheck
{
    FName Id;
    bool bAllowed = false;
    double Calc = 0;
    FString Reason;
};

struct DREAMOFPADMA_API FPadmaCardMovePreview
{
    FName Source;
    FName Target;
    TArray<FName> Ids;
    TArray<FPadmaCardMoveCheck> Checks;
    double Calc = 0;
    bool bEnemy = false;
};

struct DREAMOFPADMA_API FPadmaBattleRequest
{
    FName Mode;
    FName Node;
    FName Source;
    TArray<FName> Ids;
    double MovementCalc = 0;
};

struct DREAMOFPADMA_API FPadmaDialogueState
{
    FName Node;
    FName Line;
    FName DialogueId;
};

/** Authoritative native port of the explicitly implemented HTML subset. Commands are atomic. */
class DREAMOFPADMA_API FPadmaRunRules
{
public:
    /** Tutorial callers pass the approved authored layout retained before cosmetic/tutorial replacement. */
    void Initialize(const FPadmaContentSnapshot& InContent, const FPadmaMapLayout* AuthoredReference = nullptr);
    FPadmaRunCommandResult Start(const TMap<FName, double>& Overrides = {});
    bool HasRun() const { return bHasRun; }
    const FPadmaRunState& Run() const { return State; }
    const FPadmaContentSnapshot& Content() const { return Catalog; }
    const FPadmaCardInstance* Card(FName Id) const;
    double P(FName Key) const;
    uint64 Revision() const { return StateRevision; }
    const FString& Notice() const { return LastNotice; }
    bool IsBattle() const { return !Battle.Mode.IsNone(); }
    const FPadmaBattleRequest& PendingBattle() const { return Battle; }
    bool IsDialogue() const { return !DialogueState.Node.IsNone(); }
    const FPadmaDialogueState& Dialogue() const { return DialogueState; }
    FName HomeNode() const { return Catalog.MapLayout.MapId.IsNone() ? FName(TEXT("home")) : Catalog.MapLayout.HomeNode; }
    FName BossNode() const { return Catalog.MapLayout.MapId.IsNone() ? FName(TEXT("boss")) : Catalog.MapLayout.BossNode; }
    FName DialogueForNode(FName Node) const;

    FPadmaRunCommandResult Advance();
    FPadmaRunCommandResult Deploy(FName Id, FName Node, FName Anchor = NAME_None);
    FPadmaRunCommandResult PreviewSynthesis(FName Id, FPadmaSynthesisRecord& Out) const;
    FPadmaRunCommandResult Synthesize(FName Id);
    FPadmaRunCommandResult PreviewMove(const TArray<FName>& Ids, FName Node, FPadmaCardMovePreview& Out) const;
    FPadmaRunCommandResult Move(const TArray<FName>& Ids, FName Node, FName Mode = NAME_None);
    /** Shared faction/terrain/edge/occupancy legality; AI accepts neutral only, player may take empty hostile wilderness. */
    FPadmaRunCommandResult PreviewWildernessOccupation(FName Source, FName Node, FName Faction) const;
    FPadmaRunCommandResult Operation(FName Node);
    FPadmaRunCommandResult DialogueNext(FName Choice = NAME_None);
    FPadmaRunCommandResult DialogueCancel();
    FPadmaRunCommandResult Ability(FName Id, bool bReverse = false);
    FPadmaRunCommandResult ConfigurePreparation(const FPadmaPreparationState& Preparation);
    FPadmaRunCommandResult ConfigureMobility(FName DefId, const FPadmaCardMobilityPolicy& Policy);

    // Battle owner validates targets/action window before these atomic resource operations.
    bool CanPay(double Flow, double Calc = 0) const;
    FPadmaRunCommandResult TryPay(double Flow, double Calc = 0);
    // Pays authored cost and discards/recycles one stable basic skill identity atomically.
    FPadmaRunCommandResult ConsumeBasicSkill(FName SkillId);
    FPadmaRunCommandResult ChangeFaith(double PlayerDelta, double RulerDelta = 0);
    // Defeat/exit restores the exact pre-movement snapshot, including RNG and log. Revision never rolls back.
    FPadmaRunCommandResult FinishBattle(bool bWon, const TMap<FName, double>& UnitHP = {}, double ACTHP = -1, int32 Actions = 0, double Seconds = 0);

    FPadmaRunCommandResult CheckSaveBoundary() const;
    FPadmaRunCommandResult ValidateState(const FPadmaRunState& Candidate) const;
    FPadmaRunCommandResult Restore(const FPadmaRunState& Candidate);
    static double FaithBonus(double Faith);

private:
    FPadmaContentSnapshot Catalog;
    // Trusted authored reference stays fixed while active saves switch their frozen layout.
    // One-argument initialization also supports a standalone trusted tutorial configuration.
    FPadmaMapLayout ConfiguredMapLayout;
    FPadmaRunState State;
    FPadmaBattleRequest Battle;
    TOptional<FPadmaRunState> BattleSnapshot;
    FPadmaDialogueState DialogueState;
    FString LastNotice;
    uint64 StateRevision = 0;
    bool bHasRun = false;
    bool bCounterLimitReached = false;
    FPadmaRunCommandResult Atomic(TFunctionRef<FPadmaRunCommandResult()> Command);
    FPadmaRunCommandResult World(bool bNoon = false, bool bAllowDialogue = false) const;
    FPadmaCardInstance* MutableCard(FName Id);
    FName CreateCard(FName DefId, FName Zone, FName Node = NAME_None);
    void SetZone(FPadmaCardInstance& Card, FName Zone);
    void OnPlayed(FName Id);
    void Emit(const FString& Text, FName Kind = TEXT("info"));
    void Gain(double Flow, double Calc = 0);
    double Random();
    void Capture(FName Node);
    void ApplyWildernessOccupation(FName Node, FName Faction);
    void AdvanceWildernessFrontier();
    void ResolveStoryCheckpoint(FName Node);
    bool BeginNodeDialogue(FName Node);
    FPadmaRunCommandResult ValidateStoryState(const FPadmaRunState& Candidate) const;
    void EvaluateOutcome();
    FPadmaRunCommandResult ValidatePreparation(const FPadmaPreparationState& Value) const;
};
