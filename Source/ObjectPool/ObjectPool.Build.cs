using UnrealBuildTool;

public class ObjectPool: ModuleRules
{
    public ObjectPool(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core", 
                "CoreUObject", 
                "Engine",
                "KismetCompiler",
                "UnrealEd",
            }
        );
    }
}
