// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/ColorboundCharacterBase.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "ScalableFloat.h"
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

	// Character Stats Interface
	virtual int32 GetCharacterLevel_Implementation() const override;
	virtual int32 GetCharacterXP_Implementation() const override;
	// end Character Stats Interface

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Character")
	int32 CharacterLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Character")
	FScalableFloat CharacterXP = FScalableFloat();

	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void HitTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;

	UPROPERTY(EditAnywhere, Category = "Colorbound|AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AColorboundAIController> ColorboundAIController;

};
