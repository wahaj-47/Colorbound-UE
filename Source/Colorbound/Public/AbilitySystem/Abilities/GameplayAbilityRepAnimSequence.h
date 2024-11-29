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

	/** Animation ref. When playing a dynamic montage this points to the AnimSequence the montage was created from */
	UPROPERTY()
	TObjectPtr<UPaperZDAnimSequence> Animation;

	/** Optional slot name used by dynamic montages. */
	UPROPERTY()
	FName SlotName;

	/** Play Rate */
	UPROPERTY()
	float PlayRate;

	/** Montage position */
	UPROPERTY(NotReplicated)
	float Position;

	/** ID incremented every time a montage is played, used to trigger replication when the same montage is played multiple times. This ID wraps around when it reaches its max value. */
	UPROPERTY()
	uint8 PlayInstanceId;

	/** Bit set when montage has been stopped. */
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
		Position(0.f),
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
