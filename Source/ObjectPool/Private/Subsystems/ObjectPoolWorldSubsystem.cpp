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
	for (FActorPoolInfo ActorPoolInfo : Config->ActorPools)
	{
		if (ActorPoolInfo.ActorClass->ImplementsInterface(UPoolable::StaticClass()))
		{
			FPoolState& ObjectPool = ObjectPools.Add(ActorPoolInfo.ActorClass);
			ObjectPool.MaxPoolSize = ActorPoolInfo.PoolParams.PoolLimit;
			ObjectPool.Lifespan = ActorPoolInfo.PoolParams.Lifespan;
		}
	}
}

AActor* UObjectPoolWorldSubsystem::BeginDeferredPooledActorSpawnFromClass(const UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, const FTransform& SpawnTransform, ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, AActor* Owner, ESpawnActorScaleMethod TransformScaleMethod)
{
	AActor* SpawnedActor = nullptr;

	if (ActorClass.Get()->ImplementsInterface(UPoolable::StaticClass()))
	{
		FPoolState& ObjectPool = ObjectPools.FindOrAdd(ActorClass);

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
			SpawnedActor = Cast<AActor>(ObjectPool.Pop());
			SpawnedActor->SetOwner(Owner);
			SpawnedActor->SetActorTransform(SpawnTransform);
			SpawnedActor->SpawnCollisionHandlingMethod = CollisionHandlingOverride;
		}
	}
	else
	{
		SpawnedActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(WorldContextObject, ActorClass, SpawnTransform, CollisionHandlingOverride, Owner, TransformScaleMethod);
	}

	return SpawnedActor;
}

AActor* UObjectPoolWorldSubsystem::FinishSpawningPooledActor(AActor* Actor, const FTransform& SpawnTransform, ESpawnActorScaleMethod TransformScaleMethod)
{
	if (Actor)
	{
		if (!Actor->IsActorInitialized())
		{
			Actor->FinishSpawning(SpawnTransform, false, nullptr, TransformScaleMethod);
		}

		if (Actor->GetClass()->ImplementsInterface(UPoolable::StaticClass()))
		{
			IPoolable::Execute_OnSpawnFromPool(Actor);
			StartLifespanTimer(Actor);
		}
	}

	Actor->SetActorTickEnabled(true);
	Actor->SetActorHiddenInGame(false);
	Actor->SetActorEnableCollision(true);

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
		
		int32 ActorID = Actor->GetUniqueID();
		FTimerHandle* LifespanTimerHandle = ObjectPool->Timers.Find(ActorID);

		GetWorld()->GetTimerManager().ClearTimer(*LifespanTimerHandle);

		Actor->SetActorTickEnabled(false);
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorEnableCollision(false);
		
		ObjectPool->Add(Actor);
	}
	else
	{
		Actor->Destroy();
	}
}

void UObjectPoolWorldSubsystem::StartLifespanTimer(AActor* Actor)
{
	int32 ActorID = Actor->GetUniqueID();
	UClass* ActorClass = Actor->GetClass();
	FPoolState* ObjectPool = ObjectPools.Find(ActorClass);

	FTimerHandle& LifespanTimerHandle = ObjectPool->Timers.FindOrAdd(ActorID);
	FTimerDelegate LifespanTimerDelegate;
	LifespanTimerDelegate.BindUFunction(this, "ReturnToPool", Actor);

	GetWorld()->GetTimerManager().SetTimer(LifespanTimerHandle, LifespanTimerDelegate, ObjectPool->Lifespan, false);
}
