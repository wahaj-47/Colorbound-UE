// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_AddComponentByClass.h"
#include "K2Node_AddPooledComponentByClass.generated.h"

/**
 * 
 */
UCLASS()
class OBJECTPOOL_API UK2Node_AddPooledComponentByClass : public UK2Node_AddComponentByClass
{
	GENERATED_BODY()

public:
	UK2Node_AddPooledComponentByClass(const FObjectInitializer& ObjectInitializer);

	//~ Begin UEdGraphNode Interface.
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	//~ End UEdGraphNode Interface.

protected:
	//~ Begin UK2Node_ConstructObjectFromClass Interface
	virtual FText GetBaseNodeTitle() const override;
	virtual FText GetDefaultNodeTitle() const override;
	virtual FText GetNodeTitleFormat() const override;
	//~ End UK2Node_ConstructObjectFromClass Interface
	
};
