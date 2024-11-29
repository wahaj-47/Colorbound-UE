// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "Interfaces/Poolable.h"
#include "ColorboundAreaOfEffectActor.generated.h"

class USphereComponent;
class AColorboundEnemyCharacter;

UCLASS()
class COLORBOUND_API AColorboundAreaOfEffectActor : public AActor, public IPoolable
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<AColorboundEnemyCharacter*, FActiveGameplayEffectHandle> ActiveGameplayEffectHandles;
	
public:	
	// Sets default values for this actor's properties
	AColorboundAreaOfEffectActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USphereComponent> CollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colorbound|Ability", meta = (ExposeOnSpawn = "true"))
	FGameplayEffectSpecHandle DamageSpecHandle;

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	void OnSpawnFromPool_Implementation() override;
	void OnReturnToPool_Implementation() override;

};
