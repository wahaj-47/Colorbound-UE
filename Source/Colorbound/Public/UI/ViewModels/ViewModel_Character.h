// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "ViewModel_Character.generated.h"

class UViewModel_Health;
class UViewModel_Stats;

/**
 * 
 */
UCLASS()
class COLORBOUND_API UViewModel_Character : public UMVVMViewModelBase
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess))
	TObjectPtr<UViewModel_Health> CurrentHealthViewModel;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess))
	TObjectPtr<UViewModel_Stats> CurrentStatsViewModel;

public:
	UViewModel_Character();

	UViewModel_Health* GetCurrentHealthViewModel() const;
	void SetCurrentHealthViewModel(UViewModel_Health* NewHealthViewModel);

	UViewModel_Stats* GetCurrentStatsViewModel() const;
	void SetCurrentStatsViewModel(UViewModel_Stats* NewStatsViewModel);
	
};
