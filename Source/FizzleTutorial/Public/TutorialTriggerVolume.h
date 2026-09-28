// Copyright Fizzle. All Rights Reserved.
// TutorialTriggerVolume.h
//
// AVolume subclass that designers drop into a level.
// When an overlapping actor (player pawn / character) enters it,
// the volume tells the TutorialSubsystem to complete the step
// identified by StepTag.
//
// Editor workflow:
//   1. Place a TutorialTriggerVolume in the level.
//   2. Set StepTag to match the FTutorialStepData.StepTag you want to complete.
//   3. Optionally restrict completion to pawns that implement ITutorialTriggerInterface.
//   4. Press Play - walking through the volume completes the step.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "GameplayTagContainer.h"
#include "TutorialTypes.h"
#include "TutorialTriggerVolume.generated.h"

/**
 * ATutorialTriggerVolume
 *
 * A volume that listens for actor overlaps and calls
 * UTutorialSubsystem::CompleteTutorialStep() on match.
 *
 * - Renders a tinted wireframe in the editor for easy identification.
 * - Can be One-shot (disabled after first trigger) or repeatable.
 * - Optionally filters so only specific Actor classes trigger it.
 */
UCLASS(HideCategories = (Replication, Input, LOD, Actor, Cooking), meta = (DisplayName = "Tutorial Trigger Volume"))
class FIZZLETUTORIAL_API ATutorialTriggerVolume : public AVolume
{
	GENERATED_BODY()

public:
	ATutorialTriggerVolume();

	// --- Designer Properties --------------------------------------------------

	/**
	 * The step tag this volume completes.
	 * Must match FTutorialStepData.StepTag in your UTutorialDataAsset.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	FGameplayTag StepTag;

	/**
	 * If true the volume disables its collision after the first successful trigger.
	 * Set false if the same volume should be able to re-trigger (e.g. for replay).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	bool bOneShot = true;

	/**
	 * If set, only actors of this class (or subclasses) can trigger this volume.
	 * Leave as None to accept any Actor that has a PlayerController.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	TSubclassOf<AActor> RequiredActorClass;

	/**
	 * If true, overlapping actor must implement ITutorialTriggerInterface
	 * in addition to the class check. Allows fine-grained per-actor opt-in.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutorial")
	bool bRequireInterface = false;

	/**
	 * Editor-only: color used to tint the volume wireframe.
	 * Helps distinguish tutorial volumes from other trigger volumes in a busy level.
	 */
	UPROPERTY(EditAnywhere, Category = "Tutorial|Display")
	FColor EditorColor = FColor(0, 220, 255, 180);   // cyan

	// --- Blueprint Events -----------------------------------------------------

	/**
	 * Called in BP after this volume successfully triggers the subsystem.
	 * Override to play a sound, spawn a particle, etc.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Tutorial", meta = (DisplayName = "On Tutorial Step Triggered"))
	void OnTutorialStepTriggered(AActor* TriggeringActor);

	// --- AVolume / AActor Interface -------------------------------------------
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual FColor GetWireframeColor() const { return EditorColor; }
#endif

private:
	UFUNCTION()
	void HandleActorOverlap(AActor* OverlappedActor, AActor* OtherActor);

	bool IsValidTriggerActor(AActor* Actor) const;

	bool bHasTriggered = false;
};
