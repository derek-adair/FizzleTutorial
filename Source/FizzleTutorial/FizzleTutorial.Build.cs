// Copyright Fizzle. All Rights Reserved.

using UnrealBuildTool;

public class FizzleTutorial : ModuleRules
{
	public FizzleTutorial(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"GameplayTags",   // FGameplayTag
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"InputCore",
		});

		// Expose plugin public headers to dependent modules.
		PublicIncludePaths.AddRange(new string[]
		{
			ModuleDirectory + "/Public"
		});
	}
}
