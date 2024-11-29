// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"

#include "ColorboundAbilityTypes.generated.h"

class UColorboundAbilitySystemComponent;
class AColorboundPlayerController;
class AColorboundPlayerState;
class UPaperZDAnimationComponent;
class UPaperZDAnimInstance;
class UPaperZDAnimSequence;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FColorboundGameplayAbilityActorInfo : public FGameplayAbilityActorInfo
{
    GENERATED_USTRUCT_BODY()

    typedef Super FGameplayAbilityActorInfo;

    virtual ~FColorboundGameplayAbilityActorInfo()
    {
    }

    //Our ColorboundAbilitySystemComponent. Should NEVER be null.
    UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
    TWeakObjectPtr<UColorboundAbilitySystemComponent> ColorboundAbilitySystemComponent;

    //Our Colorbound Player Controller. Often nullptr.
    UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
    TWeakObjectPtr<AColorboundPlayerController> ColorboundPlayerController;

    //Our Colorbound Player State. Often nullptr.
    UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
    TWeakObjectPtr<AColorboundPlayerState> ColorboundPlayerState;

    //Our PaperZD Anim Instance. Often nullptr.
    UPROPERTY(BlueprintReadOnly, Category = "ActorInfo")
    TWeakObjectPtr<UPaperZDAnimationComponent> ColorboundAnimationComponent;

    virtual void InitFromActor(AActor* OwnerActor, AActor* AvatarActor, UAbilitySystemComponent* InAbilitySystemComponent) override;

    virtual void SetAvatarActor(AActor* AvatarActor) override;

    virtual void ClearActorInfo() override;

    //Gets the Kaos Player Controller. This is often nullptr.
    AColorboundPlayerController* GetColorboundPlayerController() const;

    //Gets the Kaos Player State. This is often nullptr.
    AColorboundPlayerState* GetColorboundPlayerState() const;

    UPaperZDAnimInstance* GetAnimInstance() const;
};

/** Data about montages that were played locally (all montages in case of server. predictive montages in case of client). Never replicated directly. */
USTRUCT()
struct FGameplayAbilityLocalAnimSequence
{
    GENERATED_USTRUCT_BODY()

    FGameplayAbilityLocalAnimSequence()
        : AnimSequence(nullptr), PlayInstanceId(0), AnimatingAbility(nullptr)
    {
    }

    /** What sequence is being played */
    UPROPERTY()
    TObjectPtr<UPaperZDAnimSequence> AnimSequence;

    /** ID tied to a particular play of a sequence, used to trigger replication when the same sequence is played multiple times. This ID wraps around when it reaches its max value.  */
    UPROPERTY()
    uint8 PlayInstanceId;

    /** Prediction key that started the sequence play */
    UPROPERTY()
    FPredictionKey PredictionKey;

    /** The ability, if any, that instigated this seqeunce */
    UPROPERTY()
    TWeakObjectPtr<UGameplayAbility> AnimatingAbility;
};