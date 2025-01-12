// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/ColorboundCharacterBase.h"
#include "ColorboundEnemyCharacter.generated.h"

class AColorboundAIController;
class UBehaviorTree;

/**
 * 
 */
UCLASS()
class COLORBOUND_API AColorboundEnemyCharacter : public AColorboundCharacterBase
{
	GENERATED_BODY()

public:
	AColorboundEnemyCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	UPROPERTY(EditAnywhere, Category = "Colorbound|AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AColorboundAIController> ColorboundAIController;

};
