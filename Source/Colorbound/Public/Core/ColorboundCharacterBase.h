// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"
#include "AbilitySystemInterface.h"
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
class COLORBOUND_API AColorboundCharacterBase : public APaperZDCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

private:

	UPROPERTY(BlueprintReadOnly, Category = "Colorbound|Sprite", meta = (AllowPrivateAccess="true"))
	TObjectPtr<UMaterialInstanceDynamic> SpriteMaterialInstance;

public:
	virtual void BeginPlay() override;
	// Implement IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UColorboundAttributeSet* GetAttributeSet() const;
	/**
	* Getters for attributes from GDAttributeSetBase
	**/
	UFUNCTION(BlueprintCallable, Category = "Colorbound|Attributes")
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Axis")
	FVector GetForwardVector() const;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Axis")
	FVector GetRightVector() const;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Axis")
	void SetDirectionality(FVector2D Direction);

	UFUNCTION(Server, Reliable, Category = "Colorbound|Axis")
	void Server_SetDirectionality(FVector2D Direction);

	UFUNCTION(BlueprintCallable, Category = "Colorbound|Abilities|Cosmetic")
	UPaperZDAnimSequence* GetAnimationSequence(const FGameplayTagContainer& Rules) const;

	UFUNCTION(NetMulticast, Unreliable)
	void HitReact();
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Colorbound|Abilities")
	TArray<FColorboundAbilityAnimation> AnimationRules;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Colorbound|Axis")
	FVector ForwardVector = FVector(1,0,0);

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Colorbound|Axis")
	FVector RightVector = FVector(0,-1,0);

	UPROPERTY()
	TObjectPtr<UColorboundAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UColorboundAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InputSystem|Abilities")
	TObjectPtr<UColorboundAbilitySet> AbilitySet;

	UPROPERTY(EditDefaultsOnly, Category = "Colorbound|Sprite")
	TObjectPtr<UMaterialInterface> SpriteMaterial;

	// Initialize the Character's attributes. Must run on Server but we run it on Client too
	// so that we don't have to wait. The Server's replication to the Client won't matter since
	// the values should be the same.
	virtual void InitializeAbilitySet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Colorbound|Abilities|Cosmetic", DisplayName = "OnCharacterHit")
	void K2_OnCharacterHit();

	UFUNCTION()
	void UpdateDirectionality(FVector2D Direction);
};
