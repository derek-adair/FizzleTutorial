// Copyright Fizzle. All Rights Reserved.

#include "TutorialTriggerInterface.h"
#include "TutorialSubsystem.h"
#include "GameFramework/Actor.h"

void ITutorialTriggerInterface::TriggerTutorialStep_Implementation(FGameplayTag StepTag, AActor* Instigator)
{
	const UObject* ContextObject = Instigator ? Instigator : Cast<UObject>(this);
	UTutorialSubsystem* Subsystem = UTutorialSubsystem::GetTutorialSubsystem(ContextObject);
	if (Subsystem)
	{
		Subsystem->CompleteTutorialStep(StepTag);
	}
}
