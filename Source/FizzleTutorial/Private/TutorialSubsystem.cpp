// Copyright Fizzle. All Rights Reserved.

#include "TutorialSubsystem.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

// ─────────────────────────────────────────────────────────────────────────────
// USubsystem Interface
// ─────────────────────────────────────────────────────────────────────────────

void UTutorialSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogTemp, Log, TEXT("[FizzleTutorial] Subsystem initialized."));
}

void UTutorialSubsystem::Deinitialize()
{
	ClearTimers();
	Super::Deinitialize();
}

// ─────────────────────────────────────────────────────────────────────────────
// Public API
// ─────────────────────────────────────────────────────────────────────────────

void UTutorialSubsystem::StartTutorial(UTutorialDataAsset* DataAsset, int32 StartIndex)
{
	if (!DataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FizzleTutorial] StartTutorial called with null DataAsset."));
		return;
	}

	if (DataAsset->GetStepCount() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FizzleTutorial] DataAsset '%s' has no steps."), *DataAsset->GetName());
		return;
	}

	// Reset any running tutorial first.
	if (bTutorialActive)
	{
		ResetTutorial();
	}

	ActiveTutorial = DataAsset;
	bTutorialActive = true;

	const int32 ClampedStart = FMath::Clamp(StartIndex, 0, DataAsset->GetStepCount() - 1);
	ActivateStep(ClampedStart);
}

void UTutorialSubsystem::CompleteTutorialStep(FGameplayTag OptionalStepTag)
{
	if (!bTutorialActive || !ActiveTutorial)
	{
		return;
	}

	// If a tag filter was provided, make sure the current step matches.
	// @TODO - potentially remove the current step check for out of order tutorial completion
	//		   e.g. RPG collection quest or something
	if (OptionalStepTag.IsValid())
	{
		const FTutorialStepData* StepData = ActiveTutorial->GetStepPtr(CurrentStepProgress.StepIndex);
		if (!StepData || StepData->StepTag != OptionalStepTag)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[FizzleTutorial] CompleteTutorialStep: tag '%s' doesn't match active step '%s'. Ignored."),
				*OptionalStepTag.ToString(),
				StepData ? *StepData->StepTag.ToString() : TEXT("(none)"));
			return;
		}
	}

	// Mark completed.
	CurrentStepProgress.State = ETutorialStepState::Completed;
	ClearTimers();

	UE_LOG(LogTemp, Log, TEXT("[FizzleTutorial] Step %d completed: '%s'"),
		CurrentStepProgress.StepIndex,
		*CurrentStepProgress.StepData.Title.ToString());

	OnStepCompleted.Broadcast(CurrentStepProgress);

	// Determine whether to auto-advance.
	if (ActiveTutorial->bAutoAdvance)
	{
		const float Delay = ActiveTutorial->StepTransitionDelay;
		if (Delay > 0.f)
		{
			UWorld* World = GetGameInstance()->GetWorld();
			if (World)
			{
				World->GetTimerManager().SetTimer(TransitionTimerHandle, this, &UTutorialSubsystem::AdvanceToNextStep, Delay, false);
				return;
			}
		}

		// No delay, advance immediately
		AdvanceToNextStep();
	}
}

void UTutorialSubsystem::SkipCurrentStep()
{
	if (!bTutorialActive || !ActiveTutorial)
	{
		return;
	}

	const FTutorialStepData& Step = CurrentStepProgress.StepData;
	if (Step.bMandatory)
	{
		UE_LOG(LogTemp, Warning, TEXT("[FizzleTutorial] Cannot skip mandatory step '%s'."), *Step.Title.ToString());
		return;
	}

	CurrentStepProgress.State = ETutorialStepState::Skipped;
	ClearTimers();

	UE_LOG(LogTemp, Log, TEXT("[FizzleTutorial] Step %d skipped: '%s'"), CurrentStepProgress.StepIndex, *Step.Title.ToString());

	OnStepCompleted.Broadcast(CurrentStepProgress);
	AdvanceToNextStep();
}

void UTutorialSubsystem::ResetTutorial()
{
	ClearTimers();
	bTutorialActive = false;
	ActiveTutorial = nullptr;
	CurrentStepProgress = FTutorialStepProgress();

	OnTutorialReset.Broadcast();
	UE_LOG(LogTemp, Log, TEXT("[FizzleTutorial] Tutorial reset."));
}

// ─────────────────────────────────────────────────────────────────────────────
// Static Getter
// ─────────────────────────────────────────────────────────────────────────────

UTutorialSubsystem* UTutorialSubsystem::GetTutorialSubsystem(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UGameInstance* GI = UGameplayStatics::GetGameInstance(WorldContextObject);
	if (!GI)
	{
		return nullptr;
	}

	return GI->GetSubsystem<UTutorialSubsystem>();
}

// ─────────────────────────────────────────────────────────────────────────────
// Internal Helpers
// ─────────────────────────────────────────────────────────────────────────────

void UTutorialSubsystem::ActivateStep(int32 Index)
{
	if (!ActiveTutorial || !ActiveTutorial->Steps.IsValidIndex(Index))
	{
		FinishTutorial();
		return;
	}

	CurrentStepProgress.StepIndex = Index;
	CurrentStepProgress.StepData  = ActiveTutorial->Steps[Index];
	CurrentStepProgress.State     = ETutorialStepState::Active;

	UE_LOG(LogTemp, Log, TEXT("[FizzleTutorial] Step %d activated: '%s'"), Index, *CurrentStepProgress.StepData.Title.ToString());

	OnStepActivated.Broadcast(CurrentStepProgress);

	UWorld* World = GetGameInstance()->GetWorld();
	if (!World)
	{
		return;
	}

	// ── Hint timer ──────────────────────────────────────────────────────────
	const float HintDelay = CurrentStepProgress.StepData.HintDelaySeconds;
	if (HintDelay > 0.f && !CurrentStepProgress.StepData.HintText.IsEmpty())
	{
		World->GetTimerManager().SetTimer(HintTimerHandle, this, &UTutorialSubsystem::HandleHintTimer, HintDelay, false);
	}

	// ── Timed auto-complete ─────────────────────────────────────────────────
	if (CurrentStepProgress.StepData.CompletionType == ETutorialStepCompletionType::TimedAuto)
	{
		const float AutoDelay = FMath::Max(0.1f, CurrentStepProgress.StepData.AutoCompleteDelay);
		World->GetTimerManager().SetTimer(AutoCompleteTimerHandle, this, &UTutorialSubsystem::HandleTimedAutoComplete, AutoDelay, false);
	}
}

void UTutorialSubsystem::AdvanceToNextStep()
{
	if (!ActiveTutorial)
	{
		return;
	}

	const int32 NextIndex = CurrentStepProgress.StepIndex + 1;
	if (NextIndex >= ActiveTutorial->GetStepCount())
	{
		FinishTutorial();
	}
	else
	{
		ActivateStep(NextIndex);
	}
}

void UTutorialSubsystem::FinishTutorial()
{
	UTutorialDataAsset* Finished = ActiveTutorial;
	bTutorialActive = false;
	ActiveTutorial  = nullptr;
	CurrentStepProgress = FTutorialStepProgress();

	UE_LOG(LogTemp, Log, TEXT("[FizzleTutorial] Tutorial completed: '%s'"), Finished ? *Finished->TutorialName.ToString() : TEXT("(null)"));

	OnTutorialCompleted.Broadcast(Finished);
}

void UTutorialSubsystem::HandleTimedAutoComplete()
{
	CompleteTutorialStep(FGameplayTag::EmptyTag);
}

void UTutorialSubsystem::HandleHintTimer()
{
	if (bTutorialActive)
	{
		OnHintReady.Broadcast(CurrentStepProgress);
	}
}

void UTutorialSubsystem::ClearTimers()
{
	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		return;
	}

	UWorld* World = GI->GetWorld();
	if (!World)
	{
		return;
	}

	FTimerManager& TM = World->GetTimerManager();
	TM.ClearTimer(AutoCompleteTimerHandle);
	TM.ClearTimer(HintTimerHandle);
	TM.ClearTimer(TransitionTimerHandle);
}
