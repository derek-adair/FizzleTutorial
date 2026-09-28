// Copyright Fizzle. All Rights Reserved.
// TutorialTriggerInterface.h
//
// A UInterface that any actor / component can implement to opt-in to the
// tutorial trigger system from C++.
//
// Typical use cases:
//   • A ACharacter implements this so the volume knows it's the player.
//   • A AAction (jump, dash, shoot) calls TriggerTutorialStep() directly to
//     complete a "CodeTriggered" step without needing a volume in the level.
//   • An ability or movement class signals tutorial progress mid-action.
//
// Blueprint implementable: Yes. Both the C++ and BP layers can implement
// or call these functions.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "TutorialTriggerInterface.generated.h"

// The empty UObject marker required by UE's reflection system.
UINTERFACE(MinimalAPI, Blueprintable,
	meta = (DisplayName = "Tutorial Trigger Interface"))
class UTutorialTriggerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * ITutorialTriggerInterface
 *
 * Implement on any actor or component that needs to:
 *   1. Notify the tutorial system it's eligible to trigger volumes
 *      (bRequireInterface on ATutorialTriggerVolume).
 *   2. Proactively trigger a tutorial step from code.
 *
 * --- C++ usage example (in ACharacter.cpp) ----------------------------------
 *
 *   // Complete the "Tutorial.Step.FirstJump" step when the player jumps.
 *   void ABCharacter::OnJumped_Implementation()
 *   {
 *       Super::OnJumped_Implementation();
 *       TriggerTutorialStep(TAG_Tutorial_Step_FirstJump);
 *   }
 *
 * --- Blueprint usage ---------------------------------------------------------
 *
 *   Implement the interface on your BP Character, override TriggerTutorialStep,
 *   and call it from any event graph node.
 *
 * -----------------------------------------------------------------------------
 */
class FIZZLETUTORIAL_API ITutorialTriggerInterface
{
	GENERATED_BODY()

public:
	/**
	 * Call this to complete the tutorial step identified by StepTag.
	 * The default C++ implementation simply forwards to UTutorialSubsystem.
	 * Override in your actor if you need pre/post logic (e.g., play a sound).
	 *
	 * @param StepTag  The step to complete. Must match FTutorialStepData.StepTag.
	 * @param Instigator  The actor triggering the step (usually 'this').
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Tutorial", meta = (DisplayName = "Trigger Tutorial Step"))
	void TriggerTutorialStep(FGameplayTag StepTag, AActor* Instigator);
	virtual void TriggerTutorialStep_Implementation(FGameplayTag StepTag, AActor* Instigator);

	/**
	 * Override to customize whether this actor is currently allowed to trigger
	 * tutorial volumes / steps. Return false to suppress triggering temporarily
	 * (e.g., during a cutscene, spectator mode, loading screen).
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Tutorial", meta = (DisplayName = "Can Trigger Tutorial"))
	bool CanTriggerTutorial() const;
	virtual bool CanTriggerTutorial_Implementation() const { return true; }
};
