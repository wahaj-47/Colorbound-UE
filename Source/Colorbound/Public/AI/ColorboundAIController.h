// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ColorboundAIController.generated.h"

class UBehaviorTreeComponent;

/**
 * 
 */
UCLASS()
class COLORBOUND_API AColorboundAIController : public AAIController
{
	GENERATED_BODY()

public:
	AColorboundAIController();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBehaviorTreeComponent> BehaviourTreeComponent;
	
};
