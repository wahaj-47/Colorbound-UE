// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GameplayAbilityRepAnimSequence.h"

bool FGameplayAbilityRepAnimSequence::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Ar << Position;

	uint8 bIsStopped = IsStopped;
	Ar.SerializeBits(&bIsStopped, 1);
	IsStopped = bIsStopped & 1;

	if (Ar.IsLoading())
	{
		PlayInstanceId = 0;
	}

	uint8 SkipPlayRate = bSkipPlayRate;
	Ar.SerializeBits(&SkipPlayRate, 1);
	bSkipPlayRate = SkipPlayRate & 1;

	Ar << Animation;
	Ar << PlayRate;
	Ar << PlayInstanceId;

	PredictionKey.NetSerialize(Ar, Map, bOutSuccess);

	Ar << SlotName;

	bOutSuccess = true;
	return true;
}
