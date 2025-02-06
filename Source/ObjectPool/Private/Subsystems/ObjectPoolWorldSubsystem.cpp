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

	if (Config == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ObjectPoolConfig not found. Assign the object pool config data asset in project settings"));
		return;
	}
	
	for (FActorPoolInfo ActorPoolInfo : Config->ActorPools)
	{
		if (ActorPoolInfo.ActorClass->ImplementsInterface(UPoolable::StaticClass()))
		{
			FPoolState& ObjectPool = ObjectPools.Add(ActorPoolInfo.ActorClass);
			ObjectPool.MaxPoolSize = ActorPoolInfo.PoolParams.PoolLimit;
			ObjectPool.Lifespan = ActorPoolInfo.PoolParams.Lifespan;
		}
	}

	for (FActorComponentPoolInfo ActorComponentPoolInfo : Config->ActorComponentPools)
	{
		if (ActorComponentPoolInfo.ActorComponentClass->ImplementsInterface(UPoolable::StaticClass()))
		{
			FPoolState& ObjectPool = ObjectPools.Add(ActorComponentPoolInfo.ActorComponentClass);
			ObjectPool.MaxPoolSize = ActorComponentPoolInfo.PoolParams.PoolLimit;
			ObjectPool.Lifespan = ActorComponentPoolInfo.PoolParams.Lifespan;
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
				UE_LOG(LogTemp, Error, TEXT("Pool for %s at max capacity"), *GetNameSafe(ActorClass));
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
		UE_LOG(LogTemp, Error, TEXT("Actor is null"));
		return;
	}

	UClass* ActorClass = Actor->GetClass();

	if (ActorClass->ImplementsInterface(UPoolable::StaticClass()))
	{
		IPoolable::Execute_OnReturnToPool(Actor);

		FPoolState* ObjectPool = ObjectPools.Find(ActorClass);
		
		int32 ActorID = Actor->GetUniqueID();
		FTimerHandle* LifespanTimerHandle = ObjectPool->Timers.Find(ActorID);
		
		if (LifespanTimerHandle != nullptr)
		{
			GetWorld()->GetTimerManager().ClearTimer(*LifespanTimerHandle);
		}

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

UActorComponent* UObjectPoolWorldSubsystem::AddPooledComponentByClass(AActor* Owner, TSubclassOf<UActorComponent> Class, bool bManualAttachment, const FTransform& RelativeTransform, bool bDeferredFinish)
{
	UActorComponent* NewActorComponent = nullptr;
	
	if (Owner == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner is null"));
	}

	if (Class->ImplementsInterface(UPoolable::StaticClass()))
	{
		FPoolState& ObjectPool = ObjectPools.FindOrAdd(Class);

		if (ObjectPool.IsEmpty())
		{
			if (!ObjectPool.IsFull())
			{
				NewActorComponent = Owner->AddComponentByClass(Class, bManualAttachment, RelativeTransform, bDeferredFinish);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Pool for %s at max capacity"), *GetNameSafe(Class));
			}
		}
		else
		{
			NewActorComponent = Cast<UActorComponent>(ObjectPool.Pop());
		}
	}
	else
	{
		NewActorComponent = Owner->AddComponentByClass(Class, bManualAttachment, RelativeTransform, bDeferredFinish);
	}

	return NewActorComponent;
}

void UObjectPoolWorldSubsystem::FinishAddPooledComponent(AActor* Owner, UActorComponent* NewActorComp, bool bManualAttachment, const FTransform& RelativeTransform)
{
	if (Owner == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner is null"));
		return;
	}

	if (NewActorComp)
	{
		if (!NewActorComp->HasBeenCreated())
		{
			Owner->FinishAddComponent(NewActorComp, bManualAttachment, RelativeTransform);
		}

		if (NewActorComp->GetClass()->ImplementsInterface(UPoolable::StaticClass()))
		{
			USceneComponent* NewSceneComp = Cast<USceneComponent>(NewActorComp);
			if (NewSceneComp != nullptr)
			{
				if (!bManualAttachment)
				{
					if (Owner->GetRootComponent() == nullptr)
					{
						Owner->SetRootComponent(NewSceneComp);
					}
					else
					{
						NewSceneComp->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
					}
				}
			}

			NewSceneComp->SetRelativeTransform(RelativeTransform);
			IPoolable::Execute_OnSpawnFromPool(NewActorComp);
		}
	}

	NewActorComp->SetComponentTickEnabled(true);

	USceneComponent* NewSceneComp = Cast<USceneComponent>(NewActorComp);
	if (NewSceneComp)
	{
		NewSceneComp->SetHiddenInGame(false);
	}
}

void UObjectPoolWorldSubsystem::ReturnComponentToPool(UActorComponent* ActorComponent)
{
	if (ActorComponent == NULL)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor is null"));
		return;
	}

	UClass* ComponentClass = ActorComponent->GetClass();

	if (ComponentClass->ImplementsInterface(UPoolable::StaticClass()))
	{
		IPoolable::Execute_OnReturnToPool(ActorComponent);

		FPoolState* ObjectPool = ObjectPools.Find(ComponentClass);

		int32 ComponentID = ActorComponent->GetUniqueID();
		FTimerHandle* LifespanTimerHandle = ObjectPool->Timers.Find(ComponentID);

		if (LifespanTimerHandle != nullptr)
		{
			GetWorld()->GetTimerManager().ClearTimer(*LifespanTimerHandle);
		}

		ActorComponent->SetComponentTickEnabled(false);

		USceneComponent* NewSceneComp = Cast<USceneComponent>(ActorComponent);
		if (NewSceneComp)
		{
			NewSceneComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			NewSceneComp->SetHiddenInGame(true);
		}

		ObjectPool->Add(ActorComponent);
	}
	else
	{
		ActorComponent->DestroyComponent();
	}
}

void UObjectPoolWorldSubsystem::StartLifespanTimer(AActor* Actor)
{
	int32 ActorID = Actor->GetUniqueID();
	UClass* ActorClass = Actor->GetClass();
	FPoolState* ObjectPool = ObjectPools.Find(ActorClass);

	if (ObjectPool->Lifespan == 0.f)
	{
		return;
	}

	FTimerHandle& LifespanTimerHandle = ObjectPool->Timers.FindOrAdd(ActorID);
	FTimerDelegate LifespanTimerDelegate;
	LifespanTimerDelegate.BindUFunction(this, "ReturnToPool", Actor);

	GetWorld()->GetTimerManager().SetTimer(LifespanTimerHandle, LifespanTimerDelegate, ObjectPool->Lifespan, false);
}

void UObjectPoolWorldSubsystem::StartLifespanTimer(UActorComponent* ActorComponent)
{
	int32 ComponentID = ActorComponent->GetUniqueID();
	UClass* ComponentClass = ActorComponent->GetClass();
	FPoolState* ObjectPool = ObjectPools.Find(ComponentClass);

	if (ObjectPool->Lifespan == 0.f)
	{
		return;
	}

	FTimerHandle& LifespanTimerHandle = ObjectPool->Timers.FindOrAdd(ComponentID);
	FTimerDelegate LifespanTimerDelegate;
	LifespanTimerDelegate.BindUFunction(this, "ReturnComponentToPool", ActorComponent);

	GetWorld()->GetTimerManager().SetTimer(LifespanTimerHandle, LifespanTimerDelegate, ObjectPool->Lifespan, false);
}
