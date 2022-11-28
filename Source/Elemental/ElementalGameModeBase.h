// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IDetailTreeNode.h"
#include "Manager.h"
#include "GameFramework/GameModeBase.h"
#include "ElementalGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ELEMENTAL_API AElementalGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Managers", meta = (DeterminesOutputType = "ManagerClassType"))
	AManager* GetManagerByClass(TSubclassOf<AManager> ManagerClassType); 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Managers")
	TArray<AManager*> Managers;
};
