// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ObjectPoolWorldSubsystem.generated.h"

class UObjectPoolConfig;

USTRUCT()
struct FPoolState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 PoolSize = 0;

	UPROPERTY()
	int32 MaxPoolSize = 50;

	UPROPERTY()
	TArray<AActor*> Array;

	bool IsEmpty() const
	{
		return Array.IsEmpty();
	}

	bool IsFull() const
	{
		return PoolSize == MaxPoolSize;
	}

	void Add(AActor* Actor)
	{
		Array.Add(Actor);
		PoolSize++;
	}

	AActor* Pop()
	{
		return Array.Pop();
	}
};

/**
 *
 */
UCLASS(config = Game, defaultconfig)
class OBJECTPOOL_API UObjectPoolWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UObjectPoolWorldSubsystem();

	void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Pooled Spawning", meta = (WorldContext = "WorldContextObject", UnsafeDuringActorConstruction = "true", BlueprintInternalUseOnly = "true"))
	AActor* BeginDeferredPooledActorSpawnFromClass(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined, AActor* Owner = nullptr, ESpawnActorScaleMethod TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot);

	UFUNCTION(BlueprintCallable, Category = "Pooled Spawning", meta = (UnsafeDuringActorConstruction = "true", BlueprintInternalUseOnly = "true"))
	AActor* FinishSpawningPooledActor(class AActor* Actor, const FTransform& SpawnTransform, ESpawnActorScaleMethod TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot);

	UFUNCTION(BlueprintCallable, Category = "Pooled Spawning", meta = (BlueprintInternalUseOnly = "true"))
	void ReturnToPool(AActor* Actor);

private:
	UPROPERTY(config, VisibleAnywhere, Category = "Object Pool")
	TSoftObjectPtr<UObjectPoolConfig> ObjectPoolConfig;

	UPROPERTY()
	TMap<UClass*, FPoolState> ObjectPools;

};
