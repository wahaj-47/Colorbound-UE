// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ViewModels/ViewModel_Character.h"
#include "UI/ViewModels/ViewModel_Health.h"
#include "UI/ViewModels/ViewModel_Stats.h"

UViewModel_Character::UViewModel_Character()
{
	CurrentHealthViewModel = CreateDefaultSubobject<UViewModel_Health>("HealthViewModel");
	CurrentStatsViewModel = CreateDefaultSubobject<UViewModel_Stats>("StatsViewModel");
}

UViewModel_Health* UViewModel_Character::GetCurrentHealthViewModel() const
{
	return CurrentHealthViewModel;
}

void UViewModel_Character::SetCurrentHealthViewModel(UViewModel_Health* NewHealthViewModel)
{
	UE_MVVM_SET_PROPERTY_VALUE(CurrentHealthViewModel, NewHealthViewModel);
}

UViewModel_Stats* UViewModel_Character::GetCurrentStatsViewModel() const
{
	return CurrentStatsViewModel;
}

void UViewModel_Character::SetCurrentStatsViewModel(UViewModel_Stats* NewStatsViewModel)
{
	UE_MVVM_SET_PROPERTY_VALUE(CurrentStatsViewModel, NewStatsViewModel);
}
