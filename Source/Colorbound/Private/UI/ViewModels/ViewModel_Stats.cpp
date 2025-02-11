// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ViewModels/ViewModel_Stats.h"

int32 UViewModel_Stats::GetCurrentLevel() const
{
	return CurrentLevel;
}

void UViewModel_Stats::SetCurrentLevel(int32 NewLevel)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(CurrentLevel, NewLevel))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetXPPercent);
	}
}

int32 UViewModel_Stats::GetCurrentXP() const
{
	return CurrentXP;
}

void UViewModel_Stats::SetCurrentXP(int32 NewXP)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(CurrentXP, NewXP))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetXPPercent);
	}
}

int32 UViewModel_Stats::GetCurrentLevelUpRequirement() const
{
	return CurrentLevelUpRequirement;
}

void UViewModel_Stats::SetCurrentLevelUpRequirement(int32 NewLevelUpRequirement)
{
	int32 OldLevelUpRequirement = CurrentLevelUpRequirement;
	if (UE_MVVM_SET_PROPERTY_VALUE(CurrentLevelUpRequirement, NewLevelUpRequirement))
	{
		PreviousLevelUpRequirement = OldLevelUpRequirement;
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetXPPercent);
	}
}

float UViewModel_Stats::GetXPPercent() const
{
	UE_LOG(LogTemp, Warning, TEXT("Getting XP Percent"));

	if (CurrentLevel > 0)
	{
		int32 DeltaLevelUpRequirement = CurrentLevelUpRequirement - PreviousLevelUpRequirement;
		int32 XPForThisLevel = CurrentXP - PreviousLevelUpRequirement;

		return (float)XPForThisLevel / (float)DeltaLevelUpRequirement;
	}
	else
	{
		return 0;
	}
}



