// Copyright Fizzle. All Rights Reserved.
// TutorialTypes.h - Shared enums, structs, and forward declarations for the FizzleTutorial system.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "TutorialTypes.generated.h"

// -----------------------------------------------------------------------------
// Enums
// -----------------------------------------------------------------------------

/** How a tutorial step is completed. */
UENUM(BlueprintType)
enum class ETutorialStepCompletionType : uint8
{
	/** Completed when the player enters a trigger volume tagged to this step. */
	OverlapVolume		UMETA(DisplayName = "Overlap Volume"),

	/** Completed when C++ code explicitly calls CompleteTutorialStep(). */
	CodeTriggered		UMETA(DisplayName = "Code Triggered"),

	/** Completed when a Gameplay Event with a matching tag is broadcast. */
	GameplayEvent		UMETA(DisplayName = "Gameplay Event"),

	/** Completed automatically after a set delay once the step becomes active. */
	TimedAuto			UMETA(DisplayName = "Timed Auto-Complete"),
};

/** Current state of a single tutorial step at runtime. */
UENUM(BlueprintType)
enum class ETutorialStepState : uint8
{
	Inactive	UMETA(DisplayName = "Inactive"),
	Active		UMETA(DisplayName = "Active"),
	Completed	UMETA(DisplayName = "Completed"),
	Skipped		UMETA(DisplayName = "Skipped"),
};

// -----------------------------------------------------------------------------
// Structs
// -----------------------------------------------------------------------------

/**
 * Data that describes one step in a tutorial sequence.
 * Stored inside UTutorialDataAsset and referenced by the subsystem at runtime.
 */
USTRUCT(BlueprintType)
struct FIZZLETUTORIAL_API FTutorialStepData
{
	GENERATED_BODY()

	/** Unique tag that identifies this step. Used for event-based completion. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Step")
	FGameplayTag StepTag;

	/** Display title shown in UI. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Step")
	FText Title;

	/** Description / instruction text shown to the player. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Step")
	FText Description;

	/** Optional hint text revealed after HintDelaySeconds. Leave empty to skip. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Step")
	FText HintText;

	/** Seconds before the hint is shown. 0 = never show automatically. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Step", meta = (ClampMin = "0.0"))
	float HintDelaySeconds = 10.f;

	/** How this step should be completed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Step")
	ETutorialStepCompletionType CompletionType = ETutorialStepCompletionType::CodeTriggered;

	/**
	 * Only used when CompletionType == TimedAuto.
	 * Step auto-completes this many seconds after becoming active.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Step", meta = (EditCondition = "CompletionType == ETutorialStepCompletionType::TimedAuto", ClampMin = "0.1"))
	float AutoCompleteDelay = 3.f;

	/**
	 * Optional icon shown in the UI.
	 * Assign a Texture2D or Material in the Data Asset.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Step")
	TSoftObjectPtr<UTexture2D> StepIcon;

	/** If true, the player cannot skip this step. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tutorial|Step")
	bool bMandatory = false;
};

/**
 * Runtime snapshot of a step's progress, broadcast through delegates.
 */
USTRUCT(BlueprintType)
struct FIZZLETUTORIAL_API FTutorialStepProgress
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Tutorial")
	int32 StepIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Tutorial")
	FTutorialStepData StepData;

	UPROPERTY(BlueprintReadOnly, Category = "Tutorial")
	ETutorialStepState State = ETutorialStepState::Inactive;
};
