// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintGraph/K2Node_ReturnToPool.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "KismetCompiler.h"
#include "K2Node_CallFunction.h"
#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Subsystems/ObjectPoolWorldSubsystem.h"

struct FK2Node_ReturnToPoolHelper
{
	static FName GetWorldContextPinName()
	{
		return TEXT("WorldContextObject");
	}

	static FName GetActorPinName()
	{
		return TEXT("Pooled Actor");
	}

};


#define LOCTEXT_NAMESPACE "K2Node_ReturnToPool"

bool UK2Node_ReturnToPool::UseWorldContext() const
{
	UBlueprint* BP = GetBlueprint();
	const UClass* ParentClass = BP ? BP->ParentClass : nullptr;
	return ParentClass ? ParentClass->HasMetaDataHierarchical(FBlueprintMetadata::MD_ShowWorldContextPin) != nullptr : false;
}

void UK2Node_ReturnToPool::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(NodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

FText UK2Node_ReturnToPool::GetMenuCategory() const
{
	return FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Gameplay);
}

FText UK2Node_ReturnToPool::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return NSLOCTEXT("K2Node", "ReturnToPool", "Return To Pool");
}

FText UK2Node_ReturnToPool::GetTooltipText() const
{
	return NSLOCTEXT("K2Node", "ReturnToPool_Tooltip", "Returns the Actor To Pool if Poolable otherwise destroys it");
}

void UK2Node_ReturnToPool::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	// Add execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// If required add the world context pin
	if (UseWorldContext())
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), FK2Node_ReturnToPoolHelper::GetWorldContextPinName());
	}

	UEdGraphPin* ActorPin = CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, AActor::StaticClass(), FK2Node_ReturnToPoolHelper::GetActorPinName());

}

void UK2Node_ReturnToPool::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	static const FName GetWorldSubsystemFuncName = GET_FUNCTION_NAME_CHECKED(USubsystemBlueprintLibrary, GetWorldSubsystem);
	static const FName ContextObjectParamName(TEXT("ContextObject"));
	static const FName SubsystemClassParamName(TEXT("Class"));

	static const FName ReturnToPoolFuncName = GET_FUNCTION_NAME_CHECKED(UObjectPoolWorldSubsystem, ReturnToPool);
	static const FName ActorParamName(TEXT("Actor"));

	UK2Node_ReturnToPool* ReturnNode = this;
	UEdGraphPin* ReturnNodeExec = ReturnNode->GetExecPin();
	UEdGraphPin* ReturnNodeActorPin = ReturnNode->GetActorPin();
	UEdGraphPin* ReturnNodeWorldContextPin = ReturnNode->GetWorldContextPin();
	UEdGraphPin* ReturnNodeThen = ReturnNode->GetThenPin();

	if (!ReturnNodeActorPin || ((0 == ReturnNodeActorPin->LinkedTo.Num())))
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("ReturnToPoolNodeMissingActor_Error", "Return to Pool node @@ must have a @@ specified.").ToString(), ReturnNode, ReturnNodeActorPin);
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
	// create 'return to pool' call node
	UK2Node_CallFunction* CallReturnToPoolNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(ReturnNode, SourceGraph);
	CallReturnToPoolNode->FunctionReference.SetExternalMember(ReturnToPoolFuncName, UObjectPoolWorldSubsystem::StaticClass());
	CallReturnToPoolNode->AllocateDefaultPins();

	UEdGraphPin* CallReturnToPoolExec = CallReturnToPoolNode->GetExecPin();
	UEdGraphPin* CallReturnToPoolSelfPin = CallReturnToPoolNode->FindPinChecked(UEdGraphSchema_K2::PN_Self);
	UEdGraphPin* CallReturnToPoolActorPin = CallReturnToPoolNode->FindPinChecked(ActorParamName);
	UEdGraphPin* CallReturnToPoolThen = CallReturnToPoolNode->GetThenPin();

	// Make exec connection between 'then' on 'get world subsystem' and 'begin spawn'
	CompilerContext.MovePinLinksToIntermediate(*ReturnNodeExec, *CallReturnToPoolExec);
	CallGetWorldSubsystemResult->MakeLinkTo(CallReturnToPoolSelfPin);
	CompilerContext.MovePinLinksToIntermediate(*ReturnNodeActorPin, *CallReturnToPoolActorPin);
	CompilerContext.MovePinLinksToIntermediate(*ReturnNodeThen, *CallReturnToPoolThen);

	ReturnNode->BreakAllNodeLinks();
}

bool UK2Node_ReturnToPool::IsCompatibleWithGraph(UEdGraph const* Graph) const
{
	UBlueprint* Blueprint = FBlueprintEditorUtils::FindBlueprintForGraph(Graph);
	return (Blueprint != nullptr) && FBlueprintEditorUtils::IsActorBased(Blueprint) && Super::IsCompatibleWithGraph(Graph);
}

UEdGraphPin* UK2Node_ReturnToPool::GetActorPin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_ReturnToPoolHelper::GetActorPinName());
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UK2Node_ReturnToPool::GetWorldContextPin() const
{
	UEdGraphPin* Pin = FindPin(FK2Node_ReturnToPoolHelper::GetWorldContextPinName());
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

#undef LOCTEXT_NAMESPACE

