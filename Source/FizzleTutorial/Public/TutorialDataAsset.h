// Copyright Fizzle. All Rights Reserved.
// TutorialDataAsset.h — Data asset that holds the ordered list of tutorial steps.
//
// Usage in editor:
//   Right-click Content Browser → Miscellaneous → Data Asset → TutorialDataAsset
//   Fill in the Steps array. Assign this asset to UTutorialSubsystem::StartTutorial().

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TutorialTypes.h"
#include "TutorialDataAsset.generated.h"

/**
 * UTutorialDataAsset
 *
 * A primary data asset that defines an ordered sequence of tutorial steps.
 * Designers create one asset per tutorial (e.g., "DA_Tutorial_Movement",
 * "DA_Tutorial_Combat") and assign it to the subsystem at runtime.
 *
 * Loaded via soft reference so the subsystem can stream it asynchronously
 * without blocking.
 */
UCLASS(BlueprintType, meta = (DisplayName = "Tutorial Data Asset"))
class FIZZLETUTORIAL_API UTutorialDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// ─── Asset Metadata ───────────────────────────────────────────────────────

	/** Human-readable name for this tutorial sequence (shown in logs / UI). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	FText TutorialName;

	/** Short description used on a "Tutorial Select" screen if you build one. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial")
	FText TutorialDescription;

	// ─── Steps ────────────────────────────────────────────────────────────────

	/**
	 * Ordered list of tutorial steps.
	 * The subsystem advances through these in array order (index 0 → N-1).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial", meta = (TitleProperty = "Title"))
	TArray<FTutorialStepData> Steps;

	// ─── Options ──────────────────────────────────────────────────────────────

	/**
	 * If true, the tutorial can be re-played by the player from the pause menu
	 * (or however your game exposes it). The subsystem resets and starts over.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Options")
	bool bAllowReplay = true;

	/**
	 * If true, the system auto-advances to the next step immediately after a step
	 * is completed, without waiting for any confirmation input from the player.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Options")
	bool bAutoAdvance = true;

	/**
	 * Delay in seconds between a step completing and the next step becoming active.
	 * Only relevant when bAutoAdvance is true.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Options", meta = (EditCondition = "bAutoAdvance", ClampMin = "0.0"))
	float StepTransitionDelay = 0.5f;

	// ─── Helpers ──────────────────────────────────────────────────────────────

	/** Returns the step at the given index, or nullptr if out of range. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tutorial")
	const FTutorialStepData* GetStep(int32 Index) const;

	/** Total number of steps in this tutorial. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tutorial")
	int32 GetStepCount() const { return Steps.Num(); }

	/** Returns the index of the step with the matching tag, or INDEX_NONE. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tutorial")
	int32 FindStepIndexByTag(FGameplayTag StepTag) const;

	// UPrimaryDataAsset interface
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
