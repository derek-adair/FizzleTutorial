// Copyright Fizzle. All Rights Reserved.

#include "TutorialTriggerVolume.h"
#include "TutorialSubsystem.h"
#include "TutorialTriggerInterface.h"
#include "Components/BrushComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

ATutorialTriggerVolume::ATutorialTriggerVolume()
{
	// Volumes are static by default - no tick needed.
	PrimaryActorTick.bCanEverTick = false;

	// Generate overlap events on the brush component.
	if (UBrushComponent* BrushComp = GetBrushComponent())
	{
		BrushComp->SetGenerateOverlapEvents(true);
		BrushComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	}

	// Visually distinguish from generic trigger volumes in-editor.
	bColored = true;
	BrushColor = EditorColor;
}

void ATutorialTriggerVolume::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &ATutorialTriggerVolume::HandleActorOverlap);
}

void ATutorialTriggerVolume::HandleActorOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (bOneShot && bHasTriggered)
	{
		return;
	}

	if (!StepTag.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[FizzleTutorial] TriggerVolume '%s' has no StepTag set. Ignored."), *GetName());
		return;
	}

	if (!IsValidTriggerActor(OtherActor))
	{
		return;
	}

	UTutorialSubsystem* Subsystem = UTutorialSubsystem::GetTutorialSubsystem(this);
	if (!Subsystem || !Subsystem->IsTutorialActive())
	{
		return;
	}

	// Check that the currently active step expects volume completion.
	// @TODO - consider just checking if this is the current step to be completed

	const FTutorialStepProgress& Progress = Subsystem->GetCurrentStepProgress();
	if (Progress.StepData.CompletionType != ETutorialStepCompletionType::OverlapVolume)
	{
		return;
	}

	Subsystem->CompleteTutorialStep(StepTag);

	bHasTriggered = true;

	// Disable collision so no further overlaps fire (one-shot).
	if (bOneShot)
	{
		if (UBrushComponent* BrushComp = GetBrushComponent())
		{
			BrushComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	// Notify Blueprint layer.
	OnTutorialStepTriggered(OtherActor);
}

bool ATutorialTriggerVolume::IsValidTriggerActor(AActor* Actor) const
{
	if (!Actor)
	{
		return false;
	}

	// Class filter.
	if (RequiredActorClass && !Actor->IsA(RequiredActorClass))
	{
		return false;
	}

	// Default: accept any locally-controlled pawn.
	if (!RequiredActorClass)
	{
		const APawn* Pawn = Cast<APawn>(Actor);
		if (!Pawn || !Pawn->IsLocallyControlled())
		{
			return false;
		}
	}

	// Interface filter.
	if (bRequireInterface && !Actor->Implements<UTutorialTriggerInterface>())
	{
		return false;
	}

	return true;
}
