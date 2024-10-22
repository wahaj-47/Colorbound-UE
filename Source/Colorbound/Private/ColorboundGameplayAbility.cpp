// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorboundGameplayAbility.h"
#include "ColorboundCharacterBase.h"
#include "PaperZDAnimationComponent.h"

AColorboundCharacterBase* UColorboundGameplayAbility::GetColorboundCharacterFromActorInfo() const
{
    return (CurrentActorInfo ? Cast<AColorboundCharacterBase>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

UPaperZDAnimInstance* UColorboundGameplayAbility::GetAnimInstanceFromActorInfo() const
{
    return (CurrentActorInfo ? Cast<AColorboundCharacterBase>(CurrentActorInfo->AvatarActor.Get())->GetAnimationComponent()->GetAnimInstance() : nullptr);;
}
