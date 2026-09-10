#include "Game/WorldMap/PadmaWorldMapAsset.h"
#include "Misc/DataValidation.h"
#include "Engine/DataTable.h"

void UPadmaWorldMapAsset::ImportCheckpointTables()
{
	FPadmaMapLayout Candidate = Template;
	Candidate.Checkpoints.Reset();
	for (const auto& Ref : CheckpointTables)
	{
		const auto* Table = Ref.LoadSynchronous();
		if (!Table || Table->GetRowStruct() != FPadmaStoryCheckpointRow::StaticStruct())
		{
			bLastValidationSucceeded = false;
			ValidationError = TEXT("Checkpoint table missing or has the wrong row type.");
			return;
		}
		TArray<FPadmaStoryCheckpointRow*> Rows;
		Table->GetAllRows(TEXT("World map checkpoint import"), Rows);
		for (const auto* Row : Rows) Candidate.Checkpoints.Add(*Row);
	}
	bLastValidationSucceeded = FPadmaWorldMapGenerator::Validate(Candidate, ValidationError);
	if (!bLastValidationSucceeded) return;
	Candidate.Checkpoints.Sort([](const auto& A, const auto& B) { return A.Id.LexicalLess(B.Id); });
	Modify();
	Template = MoveTemp(Candidate);
	bUseGenerated = false;
	MarkPackageDirty();
}

void UPadmaWorldMapAsset::Generate()
{
	FPadmaMapLayout Candidate;
	bLastValidationSucceeded = FPadmaWorldMapGenerator::Generate(Template, GenerationSettings, Seed, Candidate, ValidationError);
	if (!bLastValidationSucceeded)
	{
		UE_LOG(LogTemp, Warning, TEXT("[World Map] Generation rejected for %s: %s"), *GetPathName(), *ValidationError);
		return;
	}
	Modify();
	GeneratedLayout = MoveTemp(Candidate);
	bUseGenerated = true;
	MarkPackageDirty();
	UE_LOG(LogTemp, Display, TEXT("[World Map] Generated %s seed=%d nodes=%d edges=%d signature=%s"),
		*GetPathName(), Seed, GeneratedLayout.Nodes.Num(), GeneratedLayout.Edges.Num(), *FPadmaWorldMapGenerator::Signature(GeneratedLayout));
}

void UPadmaWorldMapAsset::Validate()
{
	bLastValidationSucceeded = bUseGenerated
		? FPadmaWorldMapGenerator::ValidateGenerated(Template, GeneratedLayout, ValidationError)
		: FPadmaWorldMapGenerator::Validate(Template, ValidationError);
	if (!bLastValidationSucceeded)
	{
		UE_LOG(LogTemp, Warning, TEXT("[World Map] Invalid %s: %s"), *GetPathName(), *ValidationError);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("[World Map] Valid %s: %s"), *GetPathName(), *FPadmaWorldMapGenerator::Signature(GetActiveLayout()));
	}
}

void UPadmaWorldMapAsset::RestoreTemplate()
{
	Modify();
	GeneratedLayout = Template;
	bUseGenerated = false;
	MarkPackageDirty();
	Validate();
}

#if WITH_EDITOR
EDataValidationResult UPadmaWorldMapAsset::IsDataValid(FDataValidationContext& Context) const
{
	const EDataValidationResult BaseResult = Super::IsDataValid(Context);
	FString Error;
	const bool bValid = bUseGenerated
		? FPadmaWorldMapGenerator::ValidateGenerated(Template, GeneratedLayout, Error)
		: FPadmaWorldMapGenerator::Validate(Template, Error);
	if (!bValid) { Context.AddError(FText::FromString(Error)); }
	return !bValid || BaseResult == EDataValidationResult::Invalid ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}
#endif
