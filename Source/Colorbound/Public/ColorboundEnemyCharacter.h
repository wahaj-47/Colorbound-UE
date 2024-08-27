// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ColorboundCharacterBase.h"
#include "ColorboundEnemyCharacter.generated.h"

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
	
};
