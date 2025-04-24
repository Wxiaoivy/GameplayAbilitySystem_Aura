// Copyright Ivy

using System.IO;
using UnrealBuildTool;

public class Aura : ModuleRules
{
	public Aura(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore","EnhancedInput", "GameplayAbilities", "Niagara" });

		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayTags","GameplayTasks","NavigationSystem","AIModule" });

        //迁移了文件后（也迁移在Public/Private及其子目录下） 必须要加这两句话让VS能识别到源文件的地址（我在Public下面加了个Character这个文件夹
        PublicIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "public", "Character") });
        PrivateIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "private", "Character") });

       //PublicIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "public", "UserSettings") });
       //PrivateIncludePaths.AddRange(new string[] { Path.Combine(ModuleDirectory, "private", "UserSettings") });



        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
