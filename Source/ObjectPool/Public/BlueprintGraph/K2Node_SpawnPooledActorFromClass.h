// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node_ConstructObjectFromClass.h"
#include "K2Node_SpawnPooledActorFromClass.generated.h"

/**
 * 
 */
UCLASS()
class OBJECTPOOL_API UK2Node_SpawnPooledActorFromClass : public UK2Node_ConstructObjectFromClass
{
	GENERATED_BODY()

private:
	void AllocateDefaultPins() override;
	bool IsSpawnVarPin(UEdGraphPin* Pin) const override;
	void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

	void FixupScaleMethodPin();

	UEdGraphPin* GetSpawnTransformPin() const;
	UEdGraphPin* GetCollisionHandlingOverridePin() const;
	UEdGraphPin* GetScaleMethodPin() const;
	UEdGraphPin* TryGetScaleMethodPin() const;
	UEdGraphPin* GetOwnerPin() const;

protected:
	FText GetBaseNodeTitle() const override;
	FText GetDefaultNodeTitle() const override;
	FText GetNodeTitleFormat() const override;
	FText GetTooltipText() const override;
	UClass* GetClassPinBaseClass() const override;
};
