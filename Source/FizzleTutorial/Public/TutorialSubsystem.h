// Copyright Fizzle. All Rights Reserved.
// TutorialSubsystem.h — UGameInstanceSubsystem that manages tutorial state at runtime.
//
// Lifetime:  Lives for the entire game instance lifetime (not tied to a level).
// Access:    UGameInstance::GetSubsystem<UTutorialSubsystem>()
//            Or use the static helper GetTutorialSubsystem(WorldContext).

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TutorialTypes.h"
#include "TutorialDataAsset.h"
#include "TutorialSubsystem.generated.h"

// ─────────────────────────────────────────────────────────────────────────────
// Dynamic delegates exposed to Blueprints
// ─────────────────────────────────────────────────────────────────────────────

/** Fires when a tutorial step becomes active (UI should show the step). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTutorialStepActivated, const FTutorialStepProgress&, StepProgress);

/** Fires when a step is completed. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTutorialStepCompleted, const FTutorialStepProgress&, StepProgress);

/** Fires when the hint text should appear (after HintDelaySeconds). */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTutorialHintReady, const FTutorialStepProgress&, StepProgress);

/** Fires when the entire tutorial sequence is finished. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTutorialCompleted, const UTutorialDataAsset*, TutorialAsset);

/** Fires when a tutorial is aborted / reset before completing. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTutorialReset);

// ─────────────────────────────────────────────────────────────────────────────
// Subsystem
// ─────────────────────────────────────────────────────────────────────────────

/**
 * UTutorialSubsystem
 *
 * Central manager for the tutorial system.
 *
 * Responsibilities:
 *   - Load and hold a reference to the active UTutorialDataAsset
 *   - Advance through steps sequentially
 *   - Accept completion signals from: overlap volumes, C++ code, gameplay events, timers
 *   - Broadcast delegates so the UI and gameplay systems can react
 *   - Track completion state (persisted through BGameInstance if desired)
 */
UCLASS()
class FIZZLETUTORIAL_API UTutorialSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// ─── USubsystem Interface ─────────────────────────────────────────────────
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// ─── Public API (Blueprints + C++) ────────────────────────────────────────

	/**
	 * Begin a tutorial sequence from the given data asset.
	 * If a tutorial is already running it will be reset first.
	 * @param DataAsset  The tutorial to start. Must not be null.
	 * @param StartIndex Optional step to begin at (default 0).
	 */
	UFUNCTION(BlueprintCallable, Category = "Tutorial", meta = (DisplayName = "Start Tutorial"))
	void StartTutorial(UTutorialDataAsset* DataAsset, int32 StartIndex = 0);

	/**
	 * Mark the current active step as complete.
	 * Safe to call from any C++ class (actions, movements, game systems).
	 * No-op if no tutorial is running or the current step doesn't match.
	 *
	 * @param OptionalStepTag  If set, only completes the step if its tag matches.
	 *                         Leave as empty tag to always complete the current step.
	 */
	UFUNCTION(BlueprintCallable, Category = "Tutorial", meta = (DisplayName = "Complete Current Step"))
	void CompleteTutorialStep(FGameplayTag OptionalStepTag);

	/**
	 * Skip the current step (only works on non-mandatory steps).
	 */
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void SkipCurrentStep();

	/**
	 * Abort and reset the running tutorial.
	 */
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void ResetTutorial();

	/**
	 * Returns true if a tutorial is currently running and we're waiting on a step.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tutorial")
	bool IsTutorialActive() const { return bTutorialActive; }

	/**
	 * Returns the current step's runtime progress struct.
	 * Valid only while IsTutorialActive() is true.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tutorial")
	FTutorialStepProgress GetCurrentStepProgress() const { return CurrentStepProgress; }

	/** Returns the active data asset, or null if no tutorial is running. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tutorial")
	UTutorialDataAsset* GetActiveTutorial() const { return ActiveTutorial; }

	/**
	 * Static convenience getter. Returns null if no GameInstance exists.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tutorial", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Tutorial Subsystem"))
	static UTutorialSubsystem* GetTutorialSubsystem(const UObject* WorldContextObject);

	// ─── Delegates ────────────────────────────────────────────────────────────

	UPROPERTY(BlueprintAssignable, Category = "Tutorial|Events")
	FOnTutorialStepActivated OnStepActivated;

	UPROPERTY(BlueprintAssignable, Category = "Tutorial|Events")
	FOnTutorialStepCompleted OnStepCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Tutorial|Events")
	FOnTutorialHintReady OnHintReady;

	UPROPERTY(BlueprintAssignable, Category = "Tutorial|Events")
	FOnTutorialCompleted OnTutorialCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Tutorial|Events")
	FOnTutorialReset OnTutorialReset;

private:
	// ─── Internal Helpers ─────────────────────────────────────────────────────

	void ActivateStep(int32 Index);
	void AdvanceToNextStep();
	void FinishTutorial();

	void HandleTimedAutoComplete();
	void HandleHintTimer();

	void ClearTimers();

	// ─── State ────────────────────────────────────────────────────────────────

	UPROPERTY()
	TObjectPtr<UTutorialDataAsset> ActiveTutorial = nullptr;

	FTutorialStepProgress CurrentStepProgress;

	bool bTutorialActive = false;

	FTimerHandle AutoCompleteTimerHandle;
	FTimerHandle HintTimerHandle;
	FTimerHandle TransitionTimerHandle;
};
