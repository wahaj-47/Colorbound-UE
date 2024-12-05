// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintGraph/K2Node_ReturnComponentToPool.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"
#include "K2Node_CallFunction.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Subsystems/ObjectPoolWorldSubsystem.h"


struct FK2Node_ReturnComponentToPoolHelper
{
	static FName GetWorldContextPinName()
	{
		return TEXT("WorldContextObject");
	}

	static FName GetActorComponentPinName()
	{
		return TEXT("Pooled Component");
	}

};

#define LOCTEXT_NAMESPACE "K2Node_ReturnComponentToPool"

bool UK2Node_ReturnComponentToPool::UseWorldContext() const
{
	UBlueprint* BP = GetBlueprint();
	const UClass* ParentClass = BP ? BP->ParentClass : nullptr;
	return ParentClass ? ParentClass->HasMetaDataHierarchical(FBlueprintMetadata::MD_ShowWorldContextPin) != nullptr : false;
}

void UK2Node_ReturnComponentToPool::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_ReturnComponentToPool::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Gameplay);
}

FText UK2Node_ReturnComponentToPool::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NSLOCTEXT("K2Node", "ReturnComponentToPool", "Return Actor Component To Pool");
}

FText UK2Node_ReturnComponentToPool::GetTooltipText() const
{
	return NSLOCTEXT("K2Node", "ReturnToPool_Tooltip", "Returns the Actor To Pool if Poolable otherwise destroys it");
}

void UK2Node_ReturnComponentToPool::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// Add execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// If required add the world context pin
	if (UseWorldContext())
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), FK2Node_ReturnComponentToPoolHelper::GetWorldContextPinName());
	}

	UEdGraphPin* ActorComponentPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UActorComponent::StaticClass(), FK2Node_ReturnComponentToPoolHelper::GetActorComponentPinName());
}

void UK2Node_ReturnComponentToPool::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	static const FName GetWorldSubsystemFuncName = GET_FUNCTION_NAME_CHECKED(USubsystemBlueprintLibrary, GetWorldSubsystem);
	static const FName ContextObjectParamName(TEXT("ContextObject"));
	static const FName SubsystemClassParamName(TEXT("Class"));

	static const FName ReturnToPoolFuncName = GET_FUNCTION_NAME_CHECKED(UObjectPoolWorldSubsystem, ReturnComponentToPool);
	static const FName ActorComponentParamName(TEXT("ActorComponent"));

	UK2Node_ReturnComponentToPool* ReturnNode = this;
	UEdGraphPin* ReturnNodeExec = ReturnNode->GetExecPin();
	UEdGraphPin* ReturnNodeActorComponentPin = ReturnNode->GetActorComponentPin();
	UEdGraphPin* ReturnNodeWorldContextPin = ReturnNode->GetWorldContextPin();
	UEdGraphPin* ReturnNodeThen = ReturnNode->GetThenPin();

	if (!ReturnNodeActorComponentPin || ((0 == ReturnNodeActorComponentPin->LinkedTo.Num())))
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("ReturnComponentToPoolNodeMissingActor_Error", "Return to Pool node @@ must have a @@ specified.").ToString(), ReturnNode, ReturnNodeActorComponentPin);
		// we break exec links so this is the only error we get, don't want the SpawnActor node being considered and giving 'unexpected node' type warnings
		ReturnNode->BreakAllNodeLinks();
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// create 'get world subsystem' call node
	UK2Node_CallFunction* CallGetWorldSubsystemNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(ReturnNode, SourceGraph);
	CallGetWorldSubsystemNode->FunctionReference.SetExternalMember(GetWorldSubsystemFuncName, USubsystemBlueprintLibrary::StaticClass());
	CallGetWorldSubsystemNode->AllocateDefaultPins();

	UEdGraphPin* CallGetWorldSubsystemWorldContextPin = CallGetWorldSubsystemNode->FindPinChecked(ContextObjectParamName);
	UEdGraphPin* CallGetWorldSubsystemClassPin = CallGetWorldSubsystemNode->FindPinChecked(SubsystemClassParamName);

	UEdGraphPin* CallGetWorldSubsystemResult = CallGetWorldSubsystemNode->GetReturnValuePin();

	// Copy the world context connection from the spawn node to 'get world subsystem' if necessary
	if (ReturnNodeWorldContextPin)
	{
		CompilerContext.MovePinLinksToIntermediate(*ReturnNodeWorldContextPin, *CallGetWorldSubsystemWorldContextPin);
	}

	CallGetWorldSubsystemClassPin->DefaultObject = UObjectPoolWorldSubsystem::StaticClass();
	CallGetWorldSubsystemResult->PinType.PinSubCategoryObject = UObjectPoolWorldSubsystem::StaticClass();

	//////////////////////////////////////////////////////////////////////////
	// create 'return component to pool' call node
	UK2Node_CallFunction* CallReturnToPoolNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(ReturnNode, SourceGraph);
	CallReturnToPoolNode->FunctionReference.SetExternalMember(ReturnToPoolFuncName, UObjectPoolWorldSubsystem::StaticClass());
	CallReturnToPoolNode->AllocateDefaultPins();

	UEdGraphPin* CallReturnToPoolExec = CallReturnToPoolNode->GetExecPin();
	UEdGraphPin* CallReturnToPoolSelfPin = CallReturnToPoolNode->FindPinChecked(UEdGraphSchema_K2::PN_Self);
	UEdGraphPin* CallReturnToPoolActorComponentPin = CallReturnToPoolNode->FindPinChecked(ActorComponentParamName);
	UEdGraphPin* CallReturnToPoolThen = CallReturnToPoolNode->GetThenPin();

	// Make exec connection between 'then' on 'get world subsystem' and 'begin spawn'
	CompilerContext.MovePinLinksToIntermediate(*ReturnNodeExec, *CallReturnToPoolExec);
	CallGetWorldSubsystemResult->MakeLinkTo(CallReturnToPoolSelfPin);
	CompilerContext.MovePinLinksToIntermediate(*ReturnNodeActorComponentPin, *CallReturnToPoolActorComponentPin);
	CompilerContext.MovePinLinksToIntermediate(*ReturnNodeThen, *CallReturnToPoolThen);

	ReturnNode->BreakAllNodeLinks();
}

UEdGraphPin* UK2Node_ReturnComponentToPool::GetActorComponentPin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_ReturnComponentToPoolHelper::GetActorComponentPinName());
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_ReturnComponentToPool::GetWorldContextPin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_ReturnComponentToPoolHelper::GetWorldContextPinName());
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

#undef LOCTEXT_NAMESPACE