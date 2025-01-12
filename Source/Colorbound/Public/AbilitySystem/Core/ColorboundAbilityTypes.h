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

/** Data about animsequence that were played locally (all animsequence in case of server. predictive animsequence in case of client). Never replicated directly. */
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

USTRUCT(BlueprintType)
struct FColorboundGameplayEffectContext : public FGameplayEffectContext
{
    GENERATED_USTRUCT_BODY()

public:
    FColorboundGameplayEffectContext()
        : bBlockedHit(false), bCriticalHit(false)
    {
    }

    FColorboundGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
        : FGameplayEffectContext(InInstigator, InEffectCauser)
        , bBlockedHit(false)
        , bCriticalHit(false)
    {
    }


    bool GetBlockedHit() const { return bBlockedHit; } 
    bool GetCriticalHit() const { return bCriticalHit; }

    void SetBlockedHit(bool bInBlockedHit) { bBlockedHit = bInBlockedHit; }
    void SetCriticalHit(bool bInCriticalHit) { bCriticalHit = bInCriticalHit; }

    virtual UScriptStruct* GetScriptStruct() const override { return StaticStruct(); }

    /** Creates a copy of this context, used to duplicate for later modifications */
    virtual FColorboundGameplayEffectContext* Duplicate() const
    {
        FColorboundGameplayEffectContext* NewContext = new FColorboundGameplayEffectContext();
        *NewContext = *this;
        if (GetHitResult())
        {
            // Does a deep copy of the hit result
            NewContext->AddHitResult(*GetHitResult(), true);
        }
        return NewContext;
    }

    virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

protected:

    UPROPERTY(NotReplicated)
    uint8 bBlockedHit:1;

    UPROPERTY(NotReplicated)
    uint8 bCriticalHit:1;
    
};

template<>
struct TStructOpsTypeTraits< FColorboundGameplayEffectContext > : public TStructOpsTypeTraitsBase2< FColorboundGameplayEffectContext >
{
    enum
    {
        WithNetSerializer = true,
        WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
    };
};