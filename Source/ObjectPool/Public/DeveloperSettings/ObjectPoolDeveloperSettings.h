// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ObjectPoolDeveloperSettings.generated.h"

class UObjectPoolConfig;

/**
 * 
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Object Pool Settings"))
class OBJECTPOOL_API UObjectPoolDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(config, EditAnywhere, Category = "Object Pool")
	TSoftObjectPtr<UObjectPoolConfig> ObjectPoolConfig;

};
