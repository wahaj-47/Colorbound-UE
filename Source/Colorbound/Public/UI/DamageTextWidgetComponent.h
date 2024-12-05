// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Interfaces/Poolable.h"
#include "DamageTextWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class COLORBOUND_API UDamageTextWidgetComponent : public UWidgetComponent, public IPoolable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageText(float Damage);
	
};
