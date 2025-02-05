// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "ViewModel_Health.generated.h"

/**
 * 
 */
UCLASS()
class COLORBOUND_API UViewModel_Health : public UMVVMViewModelBase
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess))
	int32 CurrentHealth;

	UPROPERTY(BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess))
	int32 MaxHealth;

public:
	int32 GetCurrentHealth() const;
	void SetCurrentHealth(int32 NewCurrentHealth);

	void SetMaxHealth(int32 NewMaxHealth);
	int32 GetMaxHealth() const;
	
	UFUNCTION(BlueprintPure, FieldNotify)
	float GetHealthPercent() const;
};
