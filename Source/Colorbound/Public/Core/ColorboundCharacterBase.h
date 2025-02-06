// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "AbilitySystemInterface.h"
#include "Interfaces/CharacterStatsInterface.h"
#include "GameplayTagContainer.h"
#include "ColorboundCharacterBase.generated.h"

class UMaterialInterface;
class UMaterialInstanceDynamic;
class UGameplayAbility;
class UColorboundAbilitySystemComponent;
class UColorboundAttributeSet;
class UGameplayEffect;
class UColorboundAbilitySet;
class UPaperZDAnimSequence;
class UViewModel_Character;

USTRUCT(BlueprintType)
struct FColorboundAbilityAnimation
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colorbound|Abilities|Animation")
	TSoftObjectPtr<UPaperZDAnimSequence> Animation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Colorbound|Abilities|Animation")
	FGameplayTagContainer RequiredTags;
};

/**
 * 
 */
UCLASS()
class COLORBOUND_API AColorboundCharacterBase : public APaperZDCharacter, public IAbilitySystemInterface, public ICharacterStatsInterface
{
	GENERATED_BODY()

private:

	float BaseWalkSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Colorbound|Sprite", meta = (AllowPrivateAccess="true"))
	TObjectPtr<UMaterialInstanceDynamic> SpriteMaterialInstance;

public:
	AColorboundCharacterBase();
	virtual void BeginPlay() override;

	// Implement IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UColorboundAttributeSet* GetAttributeSet() const;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Abilities|Cosmetic")
	UPaperZDAnimSequence* GetAnimationSequence(const FGameplayTagContainer& Rules) const;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Character")
	void Die();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Die();

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Abilities")
	TArray<FColorboundAbilityAnimation> AnimationRules;

	UPROPERTY()
	TObjectPtr<UColorboundAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UColorboundAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputSystem|Abilities")
	TObjectPtr<UColorboundAbilitySet> AbilitySet;

	UPROPERTY(EditDefaultsOnly, Category = "Colorbound|Sprite")
	TObjectPtr<UMaterialInterface> SpriteMaterial;

	UPROPERTY(BlueprintReadOnly, Category = "Colorbound|Character|State")
	bool bHitReacting = false;

	UPROPERTY(BlueprintReadOnly, Category = "Colorbound|Character|UI")
	TObjectPtr<UViewModel_Character> CharacterViewModel;

	// Initialize the Character's attributes. Must run on Server but we run it on Client too
	// so that we don't have to wait. The Server's replication to the Client won't matter since
	// the values should be the same.
	virtual void InitializeAbilitySet();

	UFUNCTION(BlueprintImplementableEvent, Category = "Colorbound|Character")
	void StartDeathTimeline();

	virtual void HitTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

};
