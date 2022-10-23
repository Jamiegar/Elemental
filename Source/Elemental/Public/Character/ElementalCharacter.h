// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ElementalCharacter.generated.h"


UCLASS()
class ELEMENTAL_API AElementalCharacter : public ACharacter
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
		float _totalKnockBackPercentage = 0.0f;
	UPROPERTY(EditAnywhere, Category="Dash")
		float _dashForceMultiplier = 10;
	UPROPERTY(EditAnywhere, Category="Dash")
		int _numOfDashes = 1;
	UPROPERTY(EditAnywhere, Category="Dash")
		float _dashCoolDown = 1.5;

private:
	int CurrentNumDashes = 0;
	FTimerHandle CoolDownTimerHandle;
	bool CanDash = true;
	
public:
	// Sets default values for this character's properties
	explicit AElementalCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Health")
		float CalcTotalKnockback(const float Damage);
	UFUNCTION(BlueprintCallable, Category="Health")
		void ApplyKnockback(const float Damage ,AActor* Inst);

	void MoveForward(const float Axis);
	void MoveRight(const float Axis);
	void CharJump();
	
	
};
