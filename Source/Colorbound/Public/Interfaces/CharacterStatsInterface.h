// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CharacterStatsInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCharacterStatsInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COLORBOUND_API ICharacterStatsInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void LevelUp();

	UFUNCTION(BlueprintNativeEvent)
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintNativeEvent)
	void SetCharacterLevel(int32 InLevel);

	UFUNCTION(BlueprintNativeEvent)
	void AddToCharacterLevel(int32 InLevel);

	UFUNCTION(BlueprintNativeEvent)
	int32 GetCharacterXP() const;

	UFUNCTION(BlueprintNativeEvent)
	void SetCharacterXP(int32 InXP);

	UFUNCTION(BlueprintNativeEvent)
	void AddToCharacterXP(int32 InXP);

	UFUNCTION(BlueprintNativeEvent)
	int32 FindLevelForXP(int32 XP) const;
};
