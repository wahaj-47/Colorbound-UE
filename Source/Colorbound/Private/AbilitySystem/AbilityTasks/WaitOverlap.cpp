// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/WaitOverlap.h"
#include "Components/PrimitiveComponent.h"

UWaitOverlap* UWaitOverlap::WaitOverlap(UGameplayAbility* OwningAbility, UPrimitiveComponent* CollisionComponent)
{
    UWaitOverlap* Task = NewAbilityTask<UWaitOverlap>(OwningAbility);
    Task->TargetComponent = CollisionComponent;
    return Task;
}

void UWaitOverlap::Activate()
{
    if (TargetComponent)
    {
        TargetComponent->OnComponentBeginOverlap.AddDynamic(this, &UWaitOverlap::HandleBeginOverlap);
        TargetComponent->OnComponentEndOverlap.AddDynamic(this, &UWaitOverlap::HandleEndOverlap);
    }
}

void UWaitOverlap::OnDestroy(bool bInOwnerFinished)
{
    if (TargetComponent)
    {
        TargetComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UWaitOverlap::HandleBeginOverlap);
        TargetComponent->OnComponentEndOverlap.RemoveDynamic(this, &UWaitOverlap::HandleEndOverlap);
    }
    Super::OnDestroy(bInOwnerFinished);
}

void UWaitOverlap::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor)
    {
        OnBeginOverlap.Broadcast(OtherActor);
    }
}

void UWaitOverlap::HandleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor)
    {
        OnEndOverlap.Broadcast(OtherActor);
    }
}
