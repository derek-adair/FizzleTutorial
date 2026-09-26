// Copyright Fizzle. All Rights Reserved.

#include "FizzleTutorial.h"
#include "Modules/ModuleManager.h"

void FFizzleTutorialModule::StartupModule()
{
	UE_LOG(LogTemp, Log, TEXT("[FizzleTutorial] Module started."));
}

void FFizzleTutorialModule::ShutdownModule()
{
	UE_LOG(LogTemp, Log, TEXT("[FizzleTutorial] Module shutdown."));
}

IMPLEMENT_MODULE(FFizzleTutorialModule, FizzleTutorial)
