// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "ScalableFloat.h"
#include "ColorboundPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 /*StatValue*/)

class UColorboundAbilitySystemComponent;
class UColorboundAttributeSet;

USTRUCT(BlueprintType)
struct FColorboundLevelUpInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FScalableFloat LevelUpRequirement = FScalableFloat();

};

/**
 * 
 */
UCLASS()
class COLORBOUND_API AColorboundPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_PlayerLevel)
	int32 PlayerLevel = 1;

	UFUNCTION()
	void OnRep_PlayerLevel(int32 OldLevel);

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_PlayerXP)
	int32 PlayerXP = 1;

	UFUNCTION()
	void OnRep_PlayerXP(int32 OldXP);

protected:

	UPROPERTY()
	TObjectPtr<UColorboundAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UColorboundAttributeSet> AttributeSet;

public:
	UPROPERTY(EditDefaultsOnly)
	FColorboundLevelUpInfo LevelUpInfo;

	FOnPlayerStatChanged OnPlayerLevelChangedDelegate;
	FOnPlayerStatChanged OnPlayerXPChangedDelegate;

	AColorboundPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Implement IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UColorboundAttributeSet* GetAttributeSet() const;

	UFUNCTION(BlueprintCallable, Category = "Colorbound|PlayerState")
	UColorboundAbilitySystemComponent* GetColorboundAbilitySystemComponent() const { return AbilitySystemComponent; }

	FORCEINLINE int32 GetPlayerLevel() const { return PlayerLevel; }
	FORCEINLINE int32 GetPlayerXP() const { return PlayerXP; }

	void SetPlayerLevel(int32 InPlayerLevel);
	void SetPlayerXP(int32 InPlayerXP);

};
