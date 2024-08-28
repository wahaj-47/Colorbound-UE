// Fill out your copyright notice in the Description page of Project Settings.


#include "ColorboundGameplayAbility.h"
#include "ColorboundCharacterBase.h"

AColorboundCharacterBase* UColorboundGameplayAbility::GetColorboundCharacterFromActorInfo() const
{
    return (CurrentActorInfo ? Cast<AColorboundCharacterBase>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}
