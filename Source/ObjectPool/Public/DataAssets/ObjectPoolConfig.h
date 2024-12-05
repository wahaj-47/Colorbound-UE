// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ObjectPoolConfig.generated.h"

USTRUCT(BlueprintType)
struct FObjectPoolParams
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 PoolLimit = 50;

	// How long this object lives before returning to pool, 0 = forever
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Lifespan = 5.0f;
};

USTRUCT(BlueprintType)
struct FActorPoolInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FObjectPoolParams PoolParams;

	// Actor class must implement 'Poolable' interface.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> ActorClass;

};

USTRUCT(BlueprintType)
struct FActorComponentPoolInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FObjectPoolParams PoolParams;

	// Actor class must implement 'Poolable' interface.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UActorComponent> ActorComponentClass;

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
	TArray<FActorPoolInfo> ActorPools;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FActorComponentPoolInfo> ActorComponentPools;

};
