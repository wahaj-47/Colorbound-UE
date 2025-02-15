// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_ReturnComponentToPool.generated.h"

/**
 * 
 */
UCLASS()
class OBJECTPOOL_API UK2Node_ReturnComponentToPool : public UK2Node
{
	GENERATED_BODY()
	/** Returns if the node uses World Object Context input */
	virtual bool UseWorldContext() const;

public:
	bool IsNodeSafeToIgnore() const override { return false; }
	void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	FText GetMenuCategory() const override;
	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	FText GetTooltipText() const override;
	void AllocateDefaultPins() override;
	void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

protected:
	UEdGraphPin* GetActorComponentPin() const;
	UEdGraphPin* GetWorldContextPin() const;
};
