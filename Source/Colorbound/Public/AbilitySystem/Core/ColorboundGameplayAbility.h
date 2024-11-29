// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "ColorboundGameplayAbility.generated.h"

class AColorboundCharacterBase;
class AColorboundAbilitySystemComponent;
class UPaperZDAnimInstance;
class UPaperZDAnimSequence;
struct FColorboundGameplayAbilityActorInfo;

/**
 * Defines how ability is meant to activate
 */
UENUM(BlueprintType)
enum class EColorboundAbilityActivationPolicy : uint8
{
	OnInputTriggered,
	WhileInputActive,
	OnGranted
};

/**
 * 
 */
UCLASS()
class COLORBOUND_API UColorboundGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	EColorboundAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	void TryActivateAbilityOnGranted(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Ability")
	AColorboundCharacterBase* GetColorboundCharacterFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Ability")
	UColorboundAbilitySystemComponent* GetColorboundAbilitySystemComponentFromActorInfo() const;

	const FColorboundGameplayAbilityActorInfo* GetColorboundActorInfo(const FGameplayAbilityActorInfo* ActorInfo);

	// --------------------------------------
	//	Animation
	// --------------------------------------

	/** Returns the currently playing montage for this ability, if any */
	UFUNCTION(BlueprintCallable, Category = Animation)
	UPaperZDAnimSequence* GetCurrentSequence() const;

	/** Call to set/get the current montage from a montage task. Set to allow hooking up montage events to ability events */
	virtual void SetCurrentSequence(UPaperZDAnimSequence* InCurrentSeqeunce);

protected:

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	/** Called when this ability is granted to the ability system component. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Colorbound|Ability", DisplayName = "OnAbilityAdded")
	void K2_OnAbilityAdded();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Ability Activation")
	EColorboundAbilityActivationPolicy ActivationPolicy;

	/** Active montage being played by this ability */
	UPROPERTY()
	TObjectPtr<UPaperZDAnimSequence> CurrentSequence;


	
};
