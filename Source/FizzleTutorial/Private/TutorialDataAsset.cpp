// Copyright Fizzle. All Rights Reserved.

#include "TutorialDataAsset.h"

const FTutorialStepData* UTutorialDataAsset::GetStep(int32 Index) const
{
	if (Steps.IsValidIndex(Index))
	{
		return &Steps[Index];
	}
	return nullptr;
}

int32 UTutorialDataAsset::FindStepIndexByTag(FGameplayTag StepTag) const
{
	for (int32 i = 0; i < Steps.Num(); ++i)
	{
		if (Steps[i].StepTag == StepTag)
		{
			return i;
		}
	}
	return INDEX_NONE;
}

FPrimaryAssetId UTutorialDataAsset::GetPrimaryAssetId() const
{
	// Asset type "TutorialData" lets the Asset Manager load them by type.
	return FPrimaryAssetId(TEXT("TutorialData"), GetFName());
}
