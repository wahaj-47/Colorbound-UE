// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Core/ColorboundAbilityTypes.h"
#include "AbilitySystem/Core/ColorboundAbilitySystemComponent.h"
#include "Core/ColorboundPlayerController.h"
#include "Core/ColorboundPlayerState.h"
#include "PaperZDAnimationComponent.h"

void FColorboundGameplayAbilityActorInfo::InitFromActor(AActor* InOwnerActor, AActor* InAvatarActor, UAbilitySystemComponent* InAbilitySystemComponent)
{
	Super::InitFromActor(InOwnerActor, InAvatarActor, InAbilitySystemComponent);

	ColorboundAbilitySystemComponent = Cast<UColorboundAbilitySystemComponent>(InAbilitySystemComponent);
	
	if (AActor* const AvatarActorPtr = AvatarActor.Get())
	{
		// Grab Components that we care about
		ColorboundAnimationComponent = AvatarActorPtr->FindComponentByClass<UPaperZDAnimationComponent>();
	}

	if (PlayerController.IsValid())
	{
		ColorboundPlayerController = Cast<AColorboundPlayerController>(PlayerController.Get());
		ColorboundPlayerState = PlayerController->GetPlayerState<AColorboundPlayerState>();
	}
}

void FColorboundGameplayAbilityActorInfo::SetAvatarActor(AActor* InAvatarActor)
{
	Super::SetAvatarActor(InAvatarActor);
}

void FColorboundGameplayAbilityActorInfo::ClearActorInfo()
{
	Super::ClearActorInfo();

	ColorboundAbilitySystemComponent = nullptr;
	ColorboundPlayerController = nullptr;
	ColorboundPlayerState = nullptr;
}

AColorboundPlayerController* FColorboundGameplayAbilityActorInfo::GetColorboundPlayerController() const
{
	return ColorboundPlayerController.Get();
}

AColorboundPlayerState* FColorboundGameplayAbilityActorInfo::GetColorboundPlayerState() const
{
	return ColorboundPlayerState.Get();
}

UPaperZDAnimInstance* FColorboundGameplayAbilityActorInfo::GetAnimInstance() const
{
	UPaperZDAnimationComponent* AnimationComponent = ColorboundAnimationComponent.Get();
	if (AnimationComponent)
	{
		return AnimationComponent->GetAnimInstance();
	}

	return nullptr;
}

bool FColorboundGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	uint16 RepBits = 0;
	if (Ar.IsSaving())
	{
		if (bReplicateInstigator && Instigator.IsValid())
		{
			RepBits |= 1 << 0;
		}
		if (bReplicateEffectCauser && EffectCauser.IsValid())
		{
			RepBits |= 1 << 1;
		}
		if (AbilityCDO.IsValid())
		{
			RepBits |= 1 << 2;
		}
		if (bReplicateSourceObject && SourceObject.IsValid())
		{
			RepBits |= 1 << 3;
		}
		if (Actors.Num() > 0)
		{
			RepBits |= 1 << 4;
		}
		if (HitResult.IsValid())
		{
			RepBits |= 1 << 5;
		}
		if (bHasWorldOrigin)
		{
			RepBits |= 1 << 6;
		}
		if (bBlockedHit)
		{
			RepBits |= 1 << 7;
		}
		if (bCriticalHit)
		{
			RepBits |= 1 << 8;
		}
	}

	Ar.SerializeBits(&RepBits, 9);

	if (RepBits & (1 << 0))
	{
		Ar << Instigator;
	}
	if (RepBits & (1 << 1))
	{
		Ar << EffectCauser;
	}
	if (RepBits & (1 << 2))
	{
		Ar << AbilityCDO;
	}
	if (RepBits & (1 << 3))
	{
		Ar << SourceObject;
	}
	if (RepBits & (1 << 4))
	{
		SafeNetSerializeTArray_Default<31>(Ar, Actors);
	}
	if (RepBits & (1 << 5))
	{
		if (Ar.IsLoading())
		{
			if (!HitResult.IsValid())
			{
				HitResult = TSharedPtr<FHitResult>(new FHitResult());
			}
		}
		HitResult->NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 6))
	{
		Ar << WorldOrigin;
		bHasWorldOrigin = true;
	}
	else
	{
		bHasWorldOrigin = false;
	}
	if (RepBits & (1 << 7))
	{
		bBlockedHit = true;
	}
	if (RepBits & (1 << 8))
	{
		bCriticalHit = true;
	}

	if (Ar.IsLoading())
	{
		AddInstigator(Instigator.Get(), EffectCauser.Get()); // Just to initialize InstigatorAbilitySystemComponent
	}

	bOutSuccess = true;
	return true;
}
