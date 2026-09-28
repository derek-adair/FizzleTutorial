// Copyright Fizzle. All Rights Reserved.
// TutorialComponent.h
//
// A UActorComponent added to the player character (or any Actor) that:
//   1. Listens to UTutorialSubsystem delegates and exposes them as per-actor
//      Blueprint events - so individual characters don't have to wire subsystem
//      delegates themselves.
//   2. Provides a clean per-character API for triggering steps from gameplay
//      code (actions, abilities, movement states).
//   3. Implements ITutorialTriggerInterface so volumes with bRequireInterface
//      will automatically accept its owner as a valid trigger source.
//
// Adding to ACharacter:
//   In ACharacter.h:
//     UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Tutorial")
//     TObjectPtr<UTutorialComponent> TutorialComponent;
//
//   In ACharacter.cpp constructor:
//     TutorialComponent = CreateDefaultSubobject<UTutorialComponent>(TEXT("TutorialComponent"));

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "TutorialTypes.h"
#include "TutorialTriggerInterface.h"
#include "TutorialComponent.generated.h"

class UTutorialSubsystem;

// -----------------------------------------------------------------------------
// Per-actor delegates (mirrors the subsystem delegates but fires locally)
// -----------------------------------------------------------------------------

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLocalStepActivated,  const FTutorialStepProgress&, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLocalStepCompleted,  const FTutorialStepProgress&, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLocalHintReady,      const FTutorialStepProgress&, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLocalTutorialDone,   const UTutorialDataAsset*,    TutorialAsset);

/**
 * UTutorialComponent
 *
 * Add this to your player character to receive tutorial events locally and
 * to fire tutorial step completions from gameplay code.
 */
UCLASS(ClassGroup = (Tutorial), meta = (BlueprintSpawnableComponent, DisplayName = "Tutorial Component"))
class FIZZLETUTORIAL_API UTutorialComponent : public UActorComponent, public ITutorialTriggerInterface
{
	GENERATED_BODY()

public:
	UTutorialComponent();

	// --- Lifetime -------------------------------------------------------------
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// --- C++ Trigger API -----------------------------------------------------

	/**
	 * Complete the tutorial step with the given tag.
	 * Call this from any gameplay code - actions, abilities, movement events.
	 *
	 * Example (in a jump action):
	 *   if (UTutorialComponent* TC = Owner->FindComponentByClass<UTutorialComponent>())
	 *       TC->NotifyStepTriggered(TAG_Tutorial_Step_FirstJump);
	 */
	UFUNCTION(BlueprintCallable, Category = "Tutorial", meta = (DisplayName = "Notify Step Triggered"))
	void NotifyStepTriggered(FGameplayTag StepTag);

	/**
	 * Convenience: skip the current active step from code.
	 */
	UFUNCTION(BlueprintCallable, Category = "Tutorial")
	void RequestSkipCurrentStep();

	/** Returns whether a tutorial is currently running in the subsystem. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tutorial")
	bool IsTutorialRunning() const;

	/** Returns the current step progress from the subsystem. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Tutorial")
	FTutorialStepProgress GetCurrentStepProgress() const;

	// --- ITutorialTriggerInterface --------------------------------------------
	virtual void TriggerTutorialStep_Implementation(FGameplayTag StepTag, AActor* Instigator) override;
	virtual bool CanTriggerTutorial_Implementation() const override;

	// --- Blueprint Events (override in BP for UI feedback) -------------------

	/**
	 * Called when a new tutorial step becomes active.
	 * Override in Blueprint to show/animate the tutorial UI widget.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Tutorial", meta = (DisplayName = "On Step Activated"))
	void BP_OnStepActivated(const FTutorialStepProgress& Progress);

	/**
	 * Called when the current step is completed.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Tutorial", meta = (DisplayName = "On Step Completed"))
	void BP_OnStepCompleted(const FTutorialStepProgress& Progress);

	/**
	 * Called when the hint for the current step should appear.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Tutorial", meta = (DisplayName = "On Hint Ready"))
	void BP_OnHintReady(const FTutorialStepProgress& Progress);

	/**
	 * Called when the entire tutorial sequence finishes.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Tutorial", meta = (DisplayName = "On Tutorial Completed"))
	void BP_OnTutorialCompleted(const UTutorialDataAsset* TutorialAsset);

	// --- Assignable delegates (for code-only wiring if BP isn't used) ---------

	UPROPERTY(BlueprintAssignable, Category = "Tutorial|Events")
	FOnLocalStepActivated OnStepActivated;

	UPROPERTY(BlueprintAssignable, Category = "Tutorial|Events")
	FOnLocalStepCompleted OnStepCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Tutorial|Events")
	FOnLocalHintReady OnHintReady;

	UPROPERTY(BlueprintAssignable, Category = "Tutorial|Events")
	FOnLocalTutorialDone OnTutorialCompleted;

	// --- Settings -------------------------------------------------------------

	/**
	 * If false, this component will not respond to or fire any tutorial events.
	 * Useful for AI-controlled pawns, spectators, etc.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	bool bParticipatesInTutorial = true;

private:
	// Subsystem delegate handles (so we can cleanly unsubscribe on EndPlay).
	UFUNCTION() void HandleStepActivated(const FTutorialStepProgress& Progress);
	UFUNCTION() void HandleStepCompleted(const FTutorialStepProgress& Progress);
	UFUNCTION() void HandleHintReady(const FTutorialStepProgress& Progress);
	UFUNCTION() void HandleTutorialCompleted(const UTutorialDataAsset* TutorialAsset);

	void BindToSubsystem();
	void UnbindFromSubsystem();

	UPROPERTY()
	TObjectPtr<UTutorialSubsystem> CachedSubsystem;
};
