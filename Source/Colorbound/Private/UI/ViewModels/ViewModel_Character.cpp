// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ViewModels/ViewModel_Character.h"
#include "UI/ViewModels/ViewModel_Health.h"

UViewModel_Character::UViewModel_Character()
{
	CurrentHealthViewModel = CreateDefaultSubobject<UViewModel_Health>("HealthViewModel");
}

UViewModel_Health* UViewModel_Character::GetCurrentHealthViewModel() const
{
	return CurrentHealthViewModel;
}

void UViewModel_Character::SetCurrentHealthViewModel(UViewModel_Health* NewHealthViewModel)
{
	UE_MVVM_SET_PROPERTY_VALUE(CurrentHealthViewModel, NewHealthViewModel);
}
