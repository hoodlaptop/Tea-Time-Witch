// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TeaTimeWitch : ModuleRules
{
	public TeaTimeWitch(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		IWYUSupport = IWYUSupport.Full;
		bUseUnity = false;
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"Paper2D",
			"UMG",
			"Slate",
			"SlateCore",
			"EnhancedInput",
			"CommonUI",
			"CommonInput",
			"GameplayTags"
		});
		
		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"RenderCore",
			"RHI",
			"Niagara",
			"AIModule",
			"Json",
			"JsonUtilities"
		});

		PublicIncludePaths.AddRange(new string[]
		{
			"TeaTimeWitch",
			"TeaTimeWitch/Core",
			"TeaTimeWitch/Battle",
			"TeaTimeWitch/TeaCraft",
			"TeaTimeWitch/Dialogue",
			"TeaTimeWitch/UI"
		});

	}
}