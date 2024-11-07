// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ObjectPoolWorldSubsystem.h"
#include "Interfaces/Poolable.h"
#include "Kismet/GameplayStatics.h"
#include "DataAssets/ObjectPoolConfig.h"

UObjectPoolWorldSubsystem::UObjectPoolWorldSubsystem()
{
	FString ConfigPath;
	if (GConfig->GetString(TEXT("/Script/ObjectPool.ObjectPoolDeveloperSettings"), TEXT("ObjectPoolConfig"), ConfigPath, GGameIni))
	{
		ObjectPoolConfig = TSoftObjectPtr<UObjectPoolConfig>(FSoftObjectPath(ConfigPath));
	}
}

void UObjectPoolWorldSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	UObjectPoolConfig* Config = ObjectPoolConfig.LoadSynchronous();
	for (FObjectPoolInfo ObjectPoolInfo : Config->ObjectPools)
	{
		if (ObjectPoolInfo.ActorClass->ImplementsInterface(UPoolable::StaticClass()))
		{
			FPoolState& ObjectPool = ObjectPools.Add(ObjectPoolInfo.ActorClass);
			ObjectPool.MaxPoolSize = ObjectPoolInfo.PoolLimit;
		}
	}
}

AActor* UObjectPoolWorldSubsystem::BeginDeferredPooledActorSpawnFromClass(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, AActor* Owner, ESpawnActorScaleMethod TransformScaleMethod)
{
	AActor* SpawnedActor = nullptr;

	if (ActorClass.Get()->ImplementsInterface(UPoolable::StaticClass()))
	{
		FPoolState& ObjectPool = ObjectPools.FindOrAdd(ActorClass);

		UE_LOG(LogTemp, Warning, TEXT("Current Pool Size: %d"), ObjectPool.PoolSize);
		UE_LOG(LogTemp, Warning, TEXT("Max Pool Size: %d"), ObjectPool.MaxPoolSize);
		
		if (ObjectPool.IsEmpty())
		{
			if (!ObjectPool.IsFull())
			{
				SpawnedActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(WorldContextObject, ActorClass, SpawnTransform, CollisionHandlingOverride, Owner, TransformScaleMethod);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Pool at max capacity"));
			}
		}
		else
		{
			SpawnedActor = ObjectPool.Pop();
			SpawnedActor->SetOwner(Owner);
			SpawnedActor->SetActorTransform(SpawnTransform);
			SpawnedActor->SpawnCollisionHandlingMethod = CollisionHandlingOverride;
		}
	}

	return SpawnedActor;
}

AActor* UObjectPoolWorldSubsystem::FinishSpawningPooledActor(AActor* Actor, const FTransform& SpawnTransform, ESpawnActorScaleMethod TransformScaleMethod)
{
	if (Actor )
	{
		if (!Actor->IsActorInitialized())
		{
			Actor->FinishSpawning(SpawnTransform, false, nullptr, TransformScaleMethod);
		}
		IPoolable::Execute_OnSpawnFromPool(Actor);
	}

	return Actor;
}

void UObjectPoolWorldSubsystem::ReturnToPool(AActor* Actor)
{
	if (Actor == NULL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor was null"));
		return;
	}

	UClass* ActorClass = Actor->GetClass();

	if (ActorClass->ImplementsInterface(UPoolable::StaticClass()))
	{
		IPoolable::Execute_OnReturnToPool(Actor);
		FPoolState* ObjectPool = ObjectPools.Find(ActorClass);
		ObjectPool->Add(Actor);
	}
	else
	{
		Actor->Destroy();
	}
}
