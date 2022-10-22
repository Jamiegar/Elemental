// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ElementalPower.generated.h"

/**
 * 
 */
UCLASS()

class ELEMENTAL_API UElementalPower : public UDataAsset
{
class UNiagaraSystem;
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Default")
		FString ElementName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Primary")
		UNiagaraSystem* PrimaryProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Curve")
		UNiagaraSystem* CurveProjectile;
	
};
