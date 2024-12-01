// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameplayPrediction.h"
#include "GameplayAbilityRepAnimSequence.generated.h"

class UPaperZDAnimSequence;

/**
 * 
 */
USTRUCT()
struct FGameplayAbilityRepAnimSequence
{
	GENERATED_USTRUCT_BODY()

	/** Animation ref. */
	UPROPERTY()
	TObjectPtr<UPaperZDAnimSequence> Animation;

	/** Slot name used by sequences. */
	UPROPERTY()
	FName SlotName;

	/** Play Rate */
	UPROPERTY()
	float PlayRate;

	/** Server time this sequence was queued */
	UPROPERTY()
	float TriggeredTime;

	/** ID incremented every time a seqeunce is played, used to trigger replication when the same montage is played multiple times. This ID wraps around when it reaches its max value. */
	UPROPERTY()
	uint8 PlayInstanceId;

	/** Bit set when sequence has been stopped. */
	UPROPERTY()
	uint8 IsStopped : 1;

	/** Stops PlayRate from replicating to save bandwidth. PlayRate will be assumed to be 1.f. */
	UPROPERTY()
	uint8 bSkipPlayRate : 1;

	UPROPERTY()
	FPredictionKey PredictionKey;

	FGameplayAbilityRepAnimSequence()
		: Animation(nullptr),
		SlotName(NAME_None),
		PlayRate(0.f),
		TriggeredTime(0.f),
		PlayInstanceId(0),
		IsStopped(true),
		bSkipPlayRate(false)
	{
	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

template<>
struct TStructOpsTypeTraits<FGameplayAbilityRepAnimSequence> : public TStructOpsTypeTraitsBase2<FGameplayAbilityRepAnimSequence>
{
	enum
	{
		WithNetSerializer = true,
	};
};
