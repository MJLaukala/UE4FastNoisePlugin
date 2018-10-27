// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FastNoisePluginEditor : ModuleRules
{
	public FastNoisePluginEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"FastNoisePluginEditor/Public",
                "FastNoisePlugin/Public",
                "AssetTools/Public",
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
                "FastNoisePluginEditor/Private",
                "Settings",
                "UnrealEd",
                "PropertyEditor"
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "UnrealEd",
                "FastNoisePlugin",
                "AssetTools",
                "Analytics",
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "AppFramework",
                "Core",
                "CoreUObject",
                "InputCore",
                "Engine",
                "RenderCore",
                "ShaderCore",
                "RHI",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "UnrealEd",
                "PropertyEditor",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
