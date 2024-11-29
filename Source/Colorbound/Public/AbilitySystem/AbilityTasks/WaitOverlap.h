// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "WaitOverlap.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOverlapEvent, AActor*, OverlappedActor);

/**
 * 
 */
UCLASS()
class COLORBOUND_API UWaitOverlap : public UAbilityTask
{
	GENERATED_BODY()

public:
    // Blueprint callable function to start the task
    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
    static UWaitOverlap* WaitOverlap(UGameplayAbility* OwningAbility, UPrimitiveComponent* CollisionComponent);

    // Delegates for overlap events
    UPROPERTY(BlueprintAssignable)
    FOnOverlapEvent OnBeginOverlap;

    UPROPERTY(BlueprintAssignable)
    FOnOverlapEvent OnEndOverlap;

protected:
    // Begin UGameplayTask interface
    virtual void Activate() override;
    virtual void OnDestroy(bool bInOwnerFinished) override;
    // End UGameplayTask interface

private:
    UPROPERTY()
    UPrimitiveComponent* TargetComponent;

    UFUNCTION()
    void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	
};
