// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "ViewModel_Stats.generated.h"

/**
 * 
 */
UCLASS()
class COLORBOUND_API UViewModel_Stats : public UMVVMViewModelBase
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess))
	int32 CurrentLevel;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess))
	int32 CurrentXP;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess))
	int32 CurrentLevelUpRequirement;

	UPROPERTY()
	int32 PreviousLevelUpRequirement;

public:
	int32 GetCurrentLevel() const;
	void SetCurrentLevel(int32 NewLevel);

	int32 GetCurrentXP() const;
	void SetCurrentXP(int32 NewXP);

	int32 GetCurrentLevelUpRequirement() const;
	void SetCurrentLevelUpRequirement(int32 NewLevelUpRequirement);

	UFUNCTION(BlueprintPure, FieldNotify)
	float GetXPPercent() const;
	
};
