// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintGraph/K2Node_SpawnPooledActorFromClass.h"
#include "KismetCompiler.h"
#include "K2Node_CallFunction.h"
#include "Interfaces/Poolable.h"
#include "Subsystems/ObjectPoolWorldSubsystem.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

struct FK2Node_SpawnPooledActorFromClassHelper
{
	static const FName ObjectPoolSubsystemPinName;
	static const FName SpawnTransformPinName;
	static const FName CollisionHandlingOverridePinName;
	static const FName TransformScaleMethodPinName;
	static const FName OwnerPinName;
};

const FName FK2Node_SpawnPooledActorFromClassHelper::ObjectPoolSubsystemPinName(TEXT("ObjecPoolSubsystem"));
const FName FK2Node_SpawnPooledActorFromClassHelper::SpawnTransformPinName(TEXT("SpawnTransform"));
const FName FK2Node_SpawnPooledActorFromClassHelper::CollisionHandlingOverridePinName(TEXT("CollisionHandlingOverride"));
const FName FK2Node_SpawnPooledActorFromClassHelper::TransformScaleMethodPinName(TEXT("TransformScaleMethod"));
const FName FK2Node_SpawnPooledActorFromClassHelper::OwnerPinName(TEXT("Owner"));

#define LOCTEXT_NAMESPACE "K2Node_SpawnPooledActorFromClass"

void UK2Node_SpawnPooledActorFromClass::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// Transform pin
	UScriptStruct* TransformStruct = TBaseStructure<FTransform>::Get();
	UEdGraphPin* TransformPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, TransformStruct, FK2Node_SpawnPooledActorFromClassHelper::SpawnTransformPinName);

	// Collision handling method pin
	UEnum* const CollisionMethodEnum = StaticEnum<ESpawnActorCollisionHandlingMethod>();
	UEdGraphPin* const CollisionHandlingOverridePin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Byte, CollisionMethodEnum, FK2Node_SpawnPooledActorFromClassHelper::CollisionHandlingOverridePinName);
	CollisionHandlingOverridePin->DefaultValue = CollisionMethodEnum->GetNameStringByValue(static_cast<int>(ESpawnActorCollisionHandlingMethod::Undefined));

	// Pin to set transform scaling behavior (ie whether to multiply scale with root component or to just ignore the root component default scale)
	UEnum* const ScaleMethodEnum = StaticEnum<ESpawnActorScaleMethod>();
	UEdGraphPin* const ScaleMethodPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Byte, ScaleMethodEnum, FK2Node_SpawnPooledActorFromClassHelper::TransformScaleMethodPinName);
	ScaleMethodPin->DefaultValue = ScaleMethodEnum->GetNameStringByValue(static_cast<int>(ESpawnActorScaleMethod::SelectDefaultAtRuntime));
	ScaleMethodPin->bAdvancedView = true;

	UEdGraphPin* OwnerPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, AActor::StaticClass(), FK2Node_SpawnPooledActorFromClassHelper::OwnerPinName);
	OwnerPin->bAdvancedView = true;
	if (ENodeAdvancedPins::NoPins == AdvancedPinDisplay)
	{
		AdvancedPinDisplay = ENodeAdvancedPins::Hidden;
	}
}

bool UK2Node_SpawnPooledActorFromClass::IsSpawnVarPin(UEdGraphPin* Pin) const
{
	UEdGraphPin* ParentPin = Pin->ParentPin;
	while (ParentPin)
	{
		if (ParentPin->PinName == FK2Node_SpawnPooledActorFromClassHelper::SpawnTransformPinName)
		{
			return false;
		}
		ParentPin = ParentPin->ParentPin;
	}

	return(Super::IsSpawnVarPin(Pin) &&
		Pin->PinName != FK2Node_SpawnPooledActorFromClassHelper::TransformScaleMethodPinName &&
		Pin->PinName != FK2Node_SpawnPooledActorFromClassHelper::CollisionHandlingOverridePinName &&
		Pin->PinName != FK2Node_SpawnPooledActorFromClassHelper::SpawnTransformPinName &&
		Pin->PinName != FK2Node_SpawnPooledActorFromClassHelper::OwnerPinName);
}

void UK2Node_SpawnPooledActorFromClass::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	// make sure that if scale method needs to be given a default value, it gets it
	FixupScaleMethodPin();

	static const FName GetWorldSubsystemFuncName = GET_FUNCTION_NAME_CHECKED(USubsystemBlueprintLibrary, GetWorldSubsystem);
	static const FName ContextObjectParamName(TEXT("ContextObject"));
	static const FName SubsystemClassParamName(TEXT("Class"));

	static const FName BeginSpawningBlueprintFuncName = GET_FUNCTION_NAME_CHECKED(UObjectPoolWorldSubsystem, BeginDeferredPooledActorSpawnFromClass);
	static const FName ActorClassParamName(TEXT("ActorClass"));
	static const FName WorldContextParamName(TEXT("WorldContextObject"));

	static const FName FinishSpawningFuncName = GET_FUNCTION_NAME_CHECKED(UObjectPoolWorldSubsystem, FinishSpawningPooledActor);
	static const FName ActorParamName(TEXT("Actor"));
	static const FName TransformParamName(TEXT("SpawnTransform"));
	static const FName CollisionHandlingOverrideParamName(TEXT("CollisionHandlingOverride"));
	static const FName OwnerParamName(TEXT("Owner"));

	static const FName ObjectParamName(TEXT("Object"));
	static const FName ValueParamName(TEXT("Value"));
	static const FName PropertyNameParamName(TEXT("PropertyName"));

	UK2Node_SpawnPooledActorFromClass* SpawnNode = this;
	UEdGraphPin* SpawnNodeExec = SpawnNode->GetExecPin();
	UEdGraphPin* SpawnNodeTransform = SpawnNode->GetSpawnTransformPin();
	UEdGraphPin* SpawnNodeCollisionHandlingOverride = GetCollisionHandlingOverridePin();
	UEdGraphPin* SpawnNodeScaleMethod = TryGetScaleMethodPin();
	UEdGraphPin* SpawnWorldContextPin = SpawnNode->GetWorldContextPin();
	UEdGraphPin* SpawnClassPin = SpawnNode->GetClassPin();
	UEdGraphPin* SpawnNodeOwnerPin = SpawnNode->GetOwnerPin();
	UEdGraphPin* SpawnNodeThen = SpawnNode->GetThenPin();
	UEdGraphPin* SpawnNodeResult = SpawnNode->GetResultPin();

	// Cache the class to spawn. Note, this is the compile time class that the pin was set to or the variable type it was connected to. Runtime it could be a child.
	UClass* ClassToSpawn = GetClassToSpawn();

	UClass* SpawnClass = (SpawnClassPin != NULL) ? Cast<UClass>(SpawnClassPin->DefaultObject) : NULL;
	UClass* InterfaceClass = UPoolable::StaticClass();
	if (!SpawnClassPin || ((0 == SpawnClassPin->LinkedTo.Num()) && (NULL == SpawnClass)))
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("SpawnActorNodeMissingClass_Error", "Spawn node @@ must have a @@ specified.").ToString(), SpawnNode, SpawnClassPin);
		// we break exec links so this is the only error we get, don't want the SpawnActor node being considered and giving 'unexpected node' type warnings
		SpawnNode->BreakAllNodeLinks();
		return;
	}

	if (!SpawnClass->ImplementsInterface(InterfaceClass))
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("SpawnActorNodeMissingInterface_Error", "Spawn node @@: @@ does not implement @@.").ToString(), SpawnNode, SpawnClass, InterfaceClass);
		// we break exec links so this is the only error we get, don't want the SpawnActor node being considered and giving 'unexpected node' type warnings
		SpawnNode->BreakAllNodeLinks();
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// create 'get world subsystem' call node
	UK2Node_CallFunction* CallGetWorldSubsystemNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(SpawnNode, SourceGraph);
	CallGetWorldSubsystemNode->FunctionReference.SetExternalMember(GetWorldSubsystemFuncName, USubsystemBlueprintLibrary::StaticClass());
	CallGetWorldSubsystemNode->AllocateDefaultPins();

	UEdGraphPin* CallGetWorldSubsystemWorldContextPin = CallGetWorldSubsystemNode->FindPinChecked(ContextObjectParamName);
	UEdGraphPin* CallGetWorldSubsystemClassPin = CallGetWorldSubsystemNode->FindPinChecked(SubsystemClassParamName);

	UEdGraphPin* CallGetWorldSubsystemResult = CallGetWorldSubsystemNode->GetReturnValuePin();

	// Copy the world context connection from the spawn node to 'get world subsystem' if necessary
	if (SpawnWorldContextPin)
	{
		CompilerContext.MovePinLinksToIntermediate(*SpawnWorldContextPin, *CallGetWorldSubsystemWorldContextPin);
	}

	CallGetWorldSubsystemClassPin->DefaultObject = UObjectPoolWorldSubsystem::StaticClass();
	CallGetWorldSubsystemResult->PinType.PinSubCategoryObject = UObjectPoolWorldSubsystem::StaticClass();

	//////////////////////////////////////////////////////////////////////////
	// create 'begin spawn' call node
	UK2Node_CallFunction* CallBeginSpawnNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(SpawnNode, SourceGraph);
	CallBeginSpawnNode->FunctionReference.SetExternalMember(BeginSpawningBlueprintFuncName, UObjectPoolWorldSubsystem::StaticClass());
	CallBeginSpawnNode->AllocateDefaultPins();

	UEdGraphPin* CallBeginExec = CallBeginSpawnNode->GetExecPin();
	UEdGraphPin* CallBeginSelfPin = CallBeginSpawnNode->FindPinChecked(UEdGraphSchema_K2::PN_Self);
	UEdGraphPin* CallBeginWorldContextPin = CallBeginSpawnNode->FindPinChecked(WorldContextParamName);
	UEdGraphPin* CallBeginActorClassPin = CallBeginSpawnNode->FindPinChecked(ActorClassParamName);
	UEdGraphPin* CallBeginTransform = CallBeginSpawnNode->FindPinChecked(TransformParamName);
	UEdGraphPin* CallBeginScaleMethod = CallBeginSpawnNode->FindPinChecked(FK2Node_SpawnPooledActorFromClassHelper::TransformScaleMethodPinName);
	UEdGraphPin* CallBeginCollisionHandlingOverride = CallBeginSpawnNode->FindPinChecked(CollisionHandlingOverrideParamName);

	UEdGraphPin* CallBeginOwnerPin = CallBeginSpawnNode->FindPinChecked(FK2Node_SpawnPooledActorFromClassHelper::OwnerPinName);
	UEdGraphPin* CallBeginResult = CallBeginSpawnNode->GetReturnValuePin();

	// Make exec connection between 'then' on 'get world subsystem' and 'begin spawn'
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeExec, *CallBeginExec);

	CallGetWorldSubsystemResult->MakeLinkTo(CallBeginSelfPin);

	if (SpawnClassPin->LinkedTo.Num() > 0)
	{
		// Copy the 'blueprint' connection from the spawn node to 'begin spawn'
		CompilerContext.MovePinLinksToIntermediate(*SpawnClassPin, *CallBeginActorClassPin);
	}
	else
	{
		// Copy blueprint literal onto begin spawn call 
		CallBeginActorClassPin->DefaultObject = SpawnClass;
	}

	// Copy the world context connection from the 'get world subsystem' to 'begin spawn' if necessary
	if (SpawnWorldContextPin)
	{
		CompilerContext.CopyPinLinksToIntermediate(*CallGetWorldSubsystemWorldContextPin, *CallBeginWorldContextPin);
	}

	if (SpawnNodeOwnerPin != nullptr)
	{
		CompilerContext.MovePinLinksToIntermediate(*SpawnNodeOwnerPin, *CallBeginOwnerPin);
	}

	// Copy the 'transform' connection from the spawn node to 'begin spawn'
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeTransform, *CallBeginTransform);

	// Copy the 'CollisionHandlingOverride' connection from the spawn node to 'begin spawn'
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeCollisionHandlingOverride, *CallBeginCollisionHandlingOverride);

	// Copy the 'ScaleMethod' connection from the spawn node to 'begin spawn'
	if (SpawnNodeScaleMethod)
	{
		CompilerContext.MovePinLinksToIntermediate(*SpawnNodeScaleMethod, *CallBeginScaleMethod);
	}
	else
	{
		// if for some reason we couldn't find scale method pin on the spawn node, defer it's value to runtime
		const UEnum* const ScaleMethodEnum = StaticEnum<ESpawnActorScaleMethod>();
		CallBeginScaleMethod->DefaultValue = ScaleMethodEnum->GetNameStringByValue(static_cast<int>(ESpawnActorScaleMethod::SelectDefaultAtRuntime));
	}

	//////////////////////////////////////////////////////////////////////////
	// create 'finish spawn' call node
	UK2Node_CallFunction* CallFinishSpawnNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(SpawnNode, SourceGraph);
	CallFinishSpawnNode->FunctionReference.SetExternalMember(FinishSpawningFuncName, UObjectPoolWorldSubsystem::StaticClass());
	CallFinishSpawnNode->AllocateDefaultPins();

	UEdGraphPin* CallFinishExec = CallFinishSpawnNode->GetExecPin();
	UEdGraphPin* CallFinishThen = CallFinishSpawnNode->GetThenPin();
	UEdGraphPin* CallFinishSelfPin = CallFinishSpawnNode->FindPinChecked(UEdGraphSchema_K2::PN_Self);
	UEdGraphPin* CallFinishActor = CallFinishSpawnNode->FindPinChecked(ActorParamName);
	UEdGraphPin* CallFinishTransform = CallFinishSpawnNode->FindPinChecked(TransformParamName);
	UEdGraphPin* CallFinishScaleMethod = CallFinishSpawnNode->FindPinChecked(FK2Node_SpawnPooledActorFromClassHelper::TransformScaleMethodPinName);
	UEdGraphPin* CallFinishResult = CallFinishSpawnNode->GetReturnValuePin();

	CompilerContext.CopyPinLinksToIntermediate(*CallBeginSelfPin, *CallFinishSelfPin);

	// Move 'then' connection from spawn node to 'finish spawn'
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeThen, *CallFinishThen);

	// Copy transform connection
	CompilerContext.CopyPinLinksToIntermediate(*CallBeginTransform, *CallFinishTransform);

	// Copy 'ScaleMethod' connection
	CompilerContext.CopyPinLinksToIntermediate(*CallBeginScaleMethod, *CallFinishScaleMethod);

	// Connect output actor from 'begin' to 'finish'
	CallBeginResult->MakeLinkTo(CallFinishActor);

	// Move result connection from spawn node to 'finish spawn'
	CallFinishResult->PinType = SpawnNodeResult->PinType; // Copy type so it uses the right actor subclass
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeResult, *CallFinishResult);

	//////////////////////////////////////////////////////////////////////////
	// create 'set var' nodes

	// Get 'result' pin from 'begin spawn', this is the actual actor we want to set properties on
	UEdGraphPin* LastThen = FKismetCompilerUtilities::GenerateAssignmentNodes(CompilerContext, SourceGraph, CallBeginSpawnNode, SpawnNode, CallBeginResult, ClassToSpawn);

	// Make exec connection between 'then' on last node and 'finish'
	LastThen->MakeLinkTo(CallFinishExec);

	// Break any links to the expanded node
	SpawnNode->BreakAllNodeLinks();
}

void UK2Node_SpawnPooledActorFromClass::FixupScaleMethodPin()
{
	// if this node is being diffed, don't fix up anything. Keep the legacy pins
	const UPackage* Package = GetPackage();
	if (Package && Package->HasAnyPackageFlags(PKG_ForDiffing))
	{
		return;
	}

	// For the sake of cook determinism, leave the scale method as ESpawnActorScaleMethod::SelectDefaultAtRuntime when cooking.
	// This shouldn't impact runtime behavior.
	if (IsRunningCookCommandlet())
	{
		return;
	}

	if (GetLinkerCustomVersion(FUE5MainStreamObjectVersion::GUID) < FUE5MainStreamObjectVersion::SpawnActorFromClassTransformScaleMethod)
	{
		if (UEdGraphPin* const ScaleMethodPin = TryGetScaleMethodPin())
		{
			const UEdGraphPin* const ClassPin = FindPin(TEXT("Class"));
			const UEnum* const ScaleMethodEnum = StaticEnum<ESpawnActorScaleMethod>();
			if (const UClass* Class = Cast<UClass>(ClassPin->DefaultObject))
			{
				if (const AActor* ActorCDO = Cast<AActor>(Class->ClassDefaultObject))
				{
					if (const USceneComponent* Root = ActorCDO->GetRootComponent()) // native root component
					{
						ScaleMethodPin->DefaultValue = ScaleMethodEnum->GetNameStringByValue(static_cast<int>(ESpawnActorScaleMethod::MultiplyWithRoot));
					}
					else
					{
						ScaleMethodPin->DefaultValue = ScaleMethodEnum->GetNameStringByValue(static_cast<int>(ESpawnActorScaleMethod::OverrideRootScale));
					}
				}
			}
			else
			{
				// if the class can't be determined during compile time, defer the default value to runtime
				ScaleMethodPin->DefaultValue = ScaleMethodEnum->GetNameStringByValue(static_cast<int>(ESpawnActorScaleMethod::SelectDefaultAtRuntime));
			}
		}
		else
		{
			UE_LOG(LogBlueprint, Warning, TEXT("Blueprint Node '%s' is missing '%s' pin. Proceeding as if it's default is SelectDefaultAtRuntime"),
				*GetNodeTitle(ENodeTitleType::FullTitle).ToString(),
				*FK2Node_SpawnPooledActorFromClassHelper::TransformScaleMethodPinName.ToString()
			)
		}
	}
}

UEdGraphPin* UK2Node_SpawnPooledActorFromClass::GetSpawnTransformPin() const
{
	UEdGraphPin* Pin = FindPinChecked(FK2Node_SpawnPooledActorFromClassHelper::SpawnTransformPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_SpawnPooledActorFromClass::GetCollisionHandlingOverridePin() const
{
	UEdGraphPin* const Pin = FindPinChecked(FK2Node_SpawnPooledActorFromClassHelper::CollisionHandlingOverridePinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_SpawnPooledActorFromClass::GetScaleMethodPin() const
{
	UEdGraphPin* const Pin = FindPinChecked(FK2Node_SpawnPooledActorFromClassHelper::TransformScaleMethodPinName);
	check(Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_SpawnPooledActorFromClass::TryGetScaleMethodPin() const
{
	return FindPin(FK2Node_SpawnPooledActorFromClassHelper::TransformScaleMethodPinName, EGPD_Input);
}

UEdGraphPin* UK2Node_SpawnPooledActorFromClass::GetOwnerPin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_SpawnPooledActorFromClassHelper::OwnerPinName);
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

FText UK2Node_SpawnPooledActorFromClass::GetBaseNodeTitle() const
{
	return NSLOCTEXT("K2Node", "SpawnPooledActor_BaseTitle", "SpawnPooledActor from Class");
}

FText UK2Node_SpawnPooledActorFromClass::GetDefaultNodeTitle() const
{
	return NSLOCTEXT("K2Node", "SpawnPooledActor_Title_NONE", "SpawnPooledActor NONE");
}

FText UK2Node_SpawnPooledActorFromClass::GetNodeTitleFormat() const
{
	return NSLOCTEXT("K2Node", "SpawnPooledActor", "SpawnPooledActor {ClassName}");
}

FText UK2Node_SpawnPooledActorFromClass::GetTooltipText() const
{
	return NSLOCTEXT("K2Node", "SpawnPooledActor_Tooltip", "Attempts to get an actor from pool. Spawns a new actor with specified transform if pool is empty.");
}

UClass* UK2Node_SpawnPooledActorFromClass::GetClassPinBaseClass() const
{
	return AActor::StaticClass();
}

#undef LOCTEXT_NAMESPACE