// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintGraph/K2Node_AddPooledComponentByClass.h"
#include "Interfaces/Poolable.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"
#include "Subsystems/ObjectPoolWorldSubsystem.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"

#define LOCTEXT_NAMESPACE "K2Node_AddPooledActorComponentByClass"

struct FK2Node_AddComponentByClassHelper
{
	static const FName OwnerPinName;
	static const FName ClassPinName;
	static const FName TransformPinName;
	static const FName ManualAttachmentPinName;
	static const FName DeferredFinishPinName;
	static const FName ComponentClassPinName;
	static const FName ActorComponentPinName;
};

const FName FK2Node_AddComponentByClassHelper::OwnerPinName(TEXT("Owner"));
const FName FK2Node_AddComponentByClassHelper::ClassPinName(TEXT("Class"));
const FName FK2Node_AddComponentByClassHelper::TransformPinName(TEXT("RelativeTransform"));
const FName FK2Node_AddComponentByClassHelper::ManualAttachmentPinName(TEXT("bManualAttachment"));
const FName FK2Node_AddComponentByClassHelper::DeferredFinishPinName(TEXT("bDeferredFinish"));
const FName FK2Node_AddComponentByClassHelper::ComponentClassPinName(TEXT("Class"));
const FName FK2Node_AddComponentByClassHelper::ActorComponentPinName(TEXT("Component"));

UK2Node_AddPooledComponentByClass::UK2Node_AddPooledComponentByClass(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeTooltip = LOCTEXT("NodeTooltip", "Adds a pooled component to an actor");
}

void UK2Node_AddPooledComponentByClass::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	UK2Node_ConstructObjectFromClass::ExpandNode(CompilerContext, SourceGraph);

	static const FName ContextObjectParamName(TEXT("ContextObject"));
	static const FName SubsystemClassParamName(TEXT("Class"));

	const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();

	UEdGraphPin* SpawnNodeExec = GetExecPin();
	UEdGraphPin* SpawnOwnerPin = K2Schema->FindSelfPin(*this, EGPD_Input);
	UEdGraphPin* SpawnClassPin = GetClassPin();
	UEdGraphPin* SpawnTransformPin = GetRelativeTransformPin();
	UEdGraphPin* SpawnManualAttachmentPin = GetManualAttachmentPin();

	UEdGraphPin* SpawnNodeThen = GetThenPin();
	UEdGraphPin* SpawnNodeResult = GetResultPin();

	UClass* SpawnClass = (SpawnClassPin ? Cast<UClass>(SpawnClassPin->DefaultObject) : nullptr);
	UClass* InterfaceClass = UPoolable::StaticClass();

	if (SpawnClassPin == nullptr || ((SpawnClass == nullptr) && (SpawnClassPin->LinkedTo.Num() == 0)))
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("AddComponentByClassNodeMissingClass_Error", "Spawn node @@ must have a class specified.").ToString(), this);
		// we break exec links so this is the only error we get, don't want the AddComponentByClass node being considered and giving 'unexpected node' type warnings
		BreakAllNodeLinks();
		return;
	}

	if (SpawnClass != nullptr && !SpawnClass->ImplementsInterface(InterfaceClass))
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("AddComponentByClassNodeMissingInterface_Error", "Spawn node @@: @@ does not implement @@.").ToString(), this, SpawnClass, InterfaceClass);
		// we break exec links so this is the only error we get, don't want the AddComponentByClass node being considered and giving 'unexpected node' type warnings
		BreakAllNodeLinks();
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// create 'get world subsystem' call node
	UK2Node_CallFunction* CallGetWorldSubsystemNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallGetWorldSubsystemNode->SetFromFunction(USubsystemBlueprintLibrary::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(USubsystemBlueprintLibrary, GetWorldSubsystem)));
	CallGetWorldSubsystemNode->AllocateDefaultPins();

	UEdGraphPin* CallGetWorldSubsystemWorldContextPin = CallGetWorldSubsystemNode->FindPinChecked(ContextObjectParamName);
	UEdGraphPin* CallGetWorldSubsystemClassPin = CallGetWorldSubsystemNode->FindPinChecked(SubsystemClassParamName);

	UEdGraphPin* CallGetWorldSubsystemResult = CallGetWorldSubsystemNode->GetReturnValuePin();

	if (SpawnOwnerPin)
	{
		CompilerContext.MovePinLinksToIntermediate(*SpawnOwnerPin, *CallGetWorldSubsystemWorldContextPin);
	}

	CallGetWorldSubsystemClassPin->DefaultObject = UObjectPoolWorldSubsystem::StaticClass();
	CallGetWorldSubsystemResult->PinType.PinSubCategoryObject = UObjectPoolWorldSubsystem::StaticClass();

	//////////////////////////////////////////////////////////////////////////
	// create 'add pooled actor component by class' call node
	UK2Node_CallFunction* CallAddComponentByClassNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallAddComponentByClassNode->SetFromFunction(UObjectPoolWorldSubsystem::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UObjectPoolWorldSubsystem, AddPooledComponentByClass)));
	CallAddComponentByClassNode->AllocateDefaultPins();

	// store off the class to spawn before we mutate pin connections:
	UClass* ClassToSpawn = GetClassToSpawn();

	UEdGraphPin* CallAddComponentByClassExec = CallAddComponentByClassNode->GetExecPin();
	UEdGraphPin* CallAddComponentByClassSelfPin = K2Schema->FindSelfPin(*CallAddComponentByClassNode, EGPD_Input); // Self is ObjectPoolWorldSubsystem
	UEdGraphPin* CallAddComponentByClassOwnerPin = CallAddComponentByClassNode->FindPinChecked(FK2Node_AddComponentByClassHelper::OwnerPinName); // Owner is the component owner actor
	UEdGraphPin* CallAddComponentByClassTypePin = CallAddComponentByClassNode->FindPinChecked(FK2Node_AddComponentByClassHelper::ComponentClassPinName);
	UEdGraphPin* CallAddComponentByClassTransformPin = CallAddComponentByClassNode->FindPinChecked(FK2Node_AddComponentByClassHelper::TransformPinName);
	UEdGraphPin* CallAddComponentByClassManualAttachmentPin = CallAddComponentByClassNode->FindPinChecked(FK2Node_AddComponentByClassHelper::ManualAttachmentPinName);
	UEdGraphPin* CallAddComponentByClassResult = CallAddComponentByClassNode->GetReturnValuePin();

	// set properties on relative transform pin to allow it to be unconnected
	CallAddComponentByClassTransformPin->bDefaultValueIsIgnored = true;
	CallAddComponentByClassTransformPin->PinType.bIsReference = false;

	// Move result connection from 'get world subsystem' to 'add pooled actor by class' self pin
	CallAddComponentByClassSelfPin->MakeLinkTo(CallGetWorldSubsystemResult);

	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeExec, *CallAddComponentByClassExec);
	CompilerContext.MovePinLinksToIntermediate(*SpawnClassPin, *CallAddComponentByClassTypePin);
	CompilerContext.MovePinLinksToIntermediate(*SpawnOwnerPin, *CallAddComponentByClassOwnerPin);
	CompilerContext.MovePinLinksToIntermediate(*SpawnTransformPin, *CallAddComponentByClassTransformPin);
	CompilerContext.MovePinLinksToIntermediate(*SpawnManualAttachmentPin, *CallAddComponentByClassManualAttachmentPin);

	// Move result connection from spawn node to specific AddComponent function
	CallAddComponentByClassResult->PinType = SpawnNodeResult->PinType; // Copy type so it uses the right actor subclass
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeResult, *CallAddComponentByClassResult);

	//////////////////////////////////////////////////////////////////////////
	// create 'set var' nodes

	UEdGraphPin* LastThen = FKismetCompilerUtilities::GenerateAssignmentNodes(CompilerContext, SourceGraph, CallAddComponentByClassNode, this, CallAddComponentByClassResult, ClassToSpawn);

	if (LastThen != CallAddComponentByClassNode->GetThenPin())
	{
		UEdGraphPin* CallAddComponentByClassDeferredFinishPin = CallAddComponentByClassNode->FindPinChecked(FK2Node_AddComponentByClassHelper::DeferredFinishPinName);
		CallAddComponentByClassDeferredFinishPin->DefaultValue = TEXT("true");

		UK2Node_CallFunction* CallRegisterComponentNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
		CallRegisterComponentNode->SetFromFunction(UObjectPoolWorldSubsystem::StaticClass()->FindFunctionByName(GET_FUNCTION_NAME_CHECKED(UObjectPoolWorldSubsystem, FinishAddPooledComponent)));
		CallRegisterComponentNode->AllocateDefaultPins();

		// Links execution from last assignment to 'RegisterComponent '
		LastThen->MakeLinkTo(CallRegisterComponentNode->GetExecPin());

		// Link the pins to RegisterComponent node
		UEdGraphPin* CallRegisterComponentByClassSelfPin = K2Schema->FindSelfPin(*CallRegisterComponentNode, EGPD_Input); // Self is ObjectPoolWorldSubsystem
		UEdGraphPin* CallRegisterComponentByClassOwnerPin = CallRegisterComponentNode->FindPinChecked(FK2Node_AddComponentByClassHelper::OwnerPinName); // Owner is the component owner actor
		UEdGraphPin* CallRegisterComponentByClassComponentPin = CallRegisterComponentNode->FindPinChecked(FK2Node_AddComponentByClassHelper::ActorComponentPinName);
		UEdGraphPin* CallRegisterComponentByClassTransformPin = CallRegisterComponentNode->FindPinChecked(FK2Node_AddComponentByClassHelper::TransformPinName);
		UEdGraphPin* CallRegisterComponentByClassManualAttachmentPin = CallRegisterComponentNode->FindPinChecked(FK2Node_AddComponentByClassHelper::ManualAttachmentPinName);

		// set properties on relative transform pin to allow it to be unconnected
		CallRegisterComponentByClassTransformPin->bDefaultValueIsIgnored = true;
		CallRegisterComponentByClassTransformPin->PinType.bIsReference = false;

		CompilerContext.CopyPinLinksToIntermediate(*CallAddComponentByClassSelfPin, *CallRegisterComponentByClassSelfPin);
		CompilerContext.CopyPinLinksToIntermediate(*CallAddComponentByClassOwnerPin, *CallRegisterComponentByClassOwnerPin);
		CompilerContext.CopyPinLinksToIntermediate(*CallAddComponentByClassTransformPin, *CallRegisterComponentByClassTransformPin);
		CompilerContext.CopyPinLinksToIntermediate(*CallAddComponentByClassManualAttachmentPin, *CallRegisterComponentByClassManualAttachmentPin);

		CallRegisterComponentByClassComponentPin->MakeLinkTo(CallAddComponentByClassResult);

		LastThen = CallRegisterComponentNode->GetThenPin();
	}

	// Move 'then' connection from AddComponent node to the last 'then'
	CompilerContext.MovePinLinksToIntermediate(*SpawnNodeThen, *LastThen);

	// Break any links to the expanded node
	BreakAllNodeLinks();
}

FText UK2Node_AddPooledComponentByClass::GetBaseNodeTitle() const
{
	return LOCTEXT("AddPooledComponent_BaseTitle", "Add Pooled Component by Class");
}

FText UK2Node_AddPooledComponentByClass::GetDefaultNodeTitle() const
{
	return GetBaseNodeTitle();
}

FText UK2Node_AddPooledComponentByClass::GetNodeTitleFormat() const
{
	return LOCTEXT("AddPooledComponent", "Add Pooled {ClassName}");
}

#undef LOCTEXT_NAMESPACE