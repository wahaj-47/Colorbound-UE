// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "ViewModel_Character.generated.h"

class UViewModel_Health;

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

public:
	UViewModel_Character();

	UViewModel_Health* GetCurrentHealthViewModel() const;
	void SetCurrentHealthViewModel(UViewModel_Health* NewHealthViewModel);
	
};
