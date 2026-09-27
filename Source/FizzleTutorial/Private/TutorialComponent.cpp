// Copyright Fizzle. All Rights Reserved.

#include "TutorialComponent.h"
#include "TutorialSubsystem.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

UTutorialComponent::UTutorialComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// -----------------------------------------------------------------------------
// Lifetime
// -----------------------------------------------------------------------------

void UTutorialComponent::BeginPlay()
{
	Super::BeginPlay();

	if (bParticipatesInTutorial)
	{
		BindToSubsystem();
	}
}

void UTutorialComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindFromSubsystem();
	Super::EndPlay(EndPlayReason);
}

// -----------------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------------

void UTutorialComponent::NotifyStepTriggered(FGameplayTag StepTag)
{
	if (!bParticipatesInTutorial)
	{
		return;
	}

	if (!CachedSubsystem)
	{
		CachedSubsystem = UTutorialSubsystem::GetTutorialSubsystem(this);
	}

	if (CachedSubsystem)
	{
		CachedSubsystem->CompleteTutorialStep(StepTag);
	}
}

void UTutorialComponent::RequestSkipCurrentStep()
{
	if (!bParticipatesInTutorial)
	{
		return;
	}

	if (!CachedSubsystem)
	{
		CachedSubsystem = UTutorialSubsystem::GetTutorialSubsystem(this);
	}

	if (CachedSubsystem)
	{
		CachedSubsystem->SkipCurrentStep();
	}
}

bool UTutorialComponent::IsTutorialRunning() const
{
	return CachedSubsystem && CachedSubsystem->IsTutorialActive();
}

FTutorialStepProgress UTutorialComponent::GetCurrentStepProgress() const
{
	if (CachedSubsystem)
	{
		return CachedSubsystem->GetCurrentStepProgress();
	}
	return FTutorialStepProgress();
}

// -----------------------------------------------------------------------------
// ITutorialTriggerInterface
// -----------------------------------------------------------------------------

void UTutorialComponent::TriggerTutorialStep_Implementation(FGameplayTag StepTag, AActor* Instigator)
{
	NotifyStepTriggered(StepTag);
}

bool UTutorialComponent::CanTriggerTutorial_Implementation() const
{
	return bParticipatesInTutorial;
}

// -----------------------------------------------------------------------------
// Subsystem binding
// -----------------------------------------------------------------------------

void UTutorialComponent::BindToSubsystem()
{
	CachedSubsystem = UTutorialSubsystem::GetTutorialSubsystem(this);
	if (!CachedSubsystem)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[FizzleTutorial] TutorialComponent could not find TutorialSubsystem for '%s'."),
			*GetOwner()->GetName());
		return;
	}

	CachedSubsystem->OnStepActivated.AddDynamic(this,     &UTutorialComponent::HandleStepActivated);
	CachedSubsystem->OnStepCompleted.AddDynamic(this,     &UTutorialComponent::HandleStepCompleted);
	CachedSubsystem->OnHintReady.AddDynamic(this,         &UTutorialComponent::HandleHintReady);
	CachedSubsystem->OnTutorialCompleted.AddDynamic(this, &UTutorialComponent::HandleTutorialCompleted);
}

void UTutorialComponent::UnbindFromSubsystem()
{
	if (!CachedSubsystem)
	{
		return;
	}

	CachedSubsystem->OnStepActivated.RemoveDynamic(this,     &UTutorialComponent::HandleStepActivated);
	CachedSubsystem->OnStepCompleted.RemoveDynamic(this,     &UTutorialComponent::HandleStepCompleted);
	CachedSubsystem->OnHintReady.RemoveDynamic(this,         &UTutorialComponent::HandleHintReady);
	CachedSubsystem->OnTutorialCompleted.RemoveDynamic(this, &UTutorialComponent::HandleTutorialCompleted);

	CachedSubsystem = nullptr;
}

// -----------------------------------------------------------------------------
// Subsystem delegate handlers - bridge to per-actor delegates + BP events
// -----------------------------------------------------------------------------

void UTutorialComponent::HandleStepActivated(const FTutorialStepProgress& Progress)
{
	OnStepActivated.Broadcast(Progress);
	BP_OnStepActivated(Progress);
}

void UTutorialComponent::HandleStepCompleted(const FTutorialStepProgress& Progress)
{
	OnStepCompleted.Broadcast(Progress);
	BP_OnStepCompleted(Progress);
}

void UTutorialComponent::HandleHintReady(const FTutorialStepProgress& Progress)
{
	OnHintReady.Broadcast(Progress);
	BP_OnHintReady(Progress);
}

void UTutorialComponent::HandleTutorialCompleted(const UTutorialDataAsset* TutorialAsset)
{
	OnTutorialCompleted.Broadcast(TutorialAsset);
	BP_OnTutorialCompleted(TutorialAsset);
}
