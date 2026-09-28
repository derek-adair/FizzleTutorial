// Copyright Fizzle. All Rights Reserved.
// FizzleTutorial.h - Module public header (included by dependent modules via PCH).

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FFizzleTutorialModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
