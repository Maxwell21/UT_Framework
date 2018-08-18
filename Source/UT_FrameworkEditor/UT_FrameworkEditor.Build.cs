// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UT_FrameworkEditor : ModuleRules
{
	public UT_FrameworkEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				// ... add other public dependencies that you statically link with here ...
			}
			);


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "InputCore",
                "Engine",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "KismetWidgets",
                "UnrealEd",
                "Kismet",
                "GraphEditor",
                "BlueprintGraph",
                "AIModule",
                "AssetTools",
                "Projects",
                "LevelEditor",
                "KismetCompiler",
                "Paper2D",
                "Sequencer",
                "MovieScene",
                "MovieSceneTracks",
                "MovieSceneTools",
                "UT_Framework",
				// ... add private dependencies that you statically link with here ...	
			}
			);

        if (Target.bBuildDeveloperTools || (Target.Configuration != UnrealTargetConfiguration.Shipping && Target.Configuration != UnrealTargetConfiguration.Test))
        {
            PrivateDependencyModuleNames.Add("GameplayDebugger");
            PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=1");
        }
        else
        {
            PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=0");
        }

        DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
