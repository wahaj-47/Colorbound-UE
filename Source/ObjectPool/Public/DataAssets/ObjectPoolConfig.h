// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ObjectPoolConfig.generated.h"

USTRUCT(BlueprintType)
struct FObjectPoolInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 PoolLimit = 50;

	// Actor class must implement 'Poolable' interface.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> ActorClass;

};

/**
 * 
 */
UCLASS()
class OBJECTPOOL_API UObjectPoolConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FObjectPoolInfo> ObjectPools;

};
