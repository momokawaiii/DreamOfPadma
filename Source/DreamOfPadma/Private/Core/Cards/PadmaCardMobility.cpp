#include "Core/Cards/PadmaCardMobility.h"

bool FPadmaCardMobilityDefinition::Validate(FText& OutFailure) const
{
	if (!bConfigured)
	{
		OutFailure = FText::FromString(TEXT("Movement policy is not configured."));
		return false;
	}
	if (Policy != EPadmaCardMobility::Movable && Policy != EPadmaCardMobility::Immovable
		&& Policy != EPadmaCardMobility::Conditional)
	{
		OutFailure = FText::FromString(TEXT("Unknown movement policy."));
		return false;
	}
	if ((Policy == EPadmaCardMobility::Conditional) == ConditionId.IsNone())
	{
		OutFailure = FText::FromString(TEXT("Conditional movement requires ConditionId; other policies must leave it empty."));
		return false;
	}
	OutFailure = FText::GetEmpty();
	return true;
}
