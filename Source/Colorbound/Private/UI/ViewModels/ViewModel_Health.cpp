// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ViewModels/ViewModel_Health.h"

int32 UViewModel_Health::GetCurrentHealth() const
{
	return CurrentHealth;
}

void UViewModel_Health::SetCurrentHealth(int32 NewCurrentHealth)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(CurrentHealth, NewCurrentHealth))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHealthPercent);
	}
}

int32 UViewModel_Health::GetMaxHealth() const
{
	return MaxHealth;
}

void UViewModel_Health::SetMaxHealth(int32 NewMaxHealth)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(MaxHealth, NewMaxHealth))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHealthPercent);
	}
}

float UViewModel_Health::GetHealthPercent() const
{
	if (MaxHealth != 0)
	{
		return (float)CurrentHealth / (float)MaxHealth;
	}
	else
	{
		return 0;
	}
}
