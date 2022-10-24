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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta=(DisplayName="Can Move"))
		bool _canMove = true;
private:
	int CurrentNumDashes = 0;
	float _movementXAxis;
	float _movementYAxis;
	
public:
	// Sets default values for this character's properties
	explicit AElementalCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Health")
		float CalcTotalKnockback(const float Damage);
	UFUNCTION(BlueprintCallable, Category="Health")
		void ApplyKnockback(const float Damage ,AActor* Inst);
	UFUNCTION(BlueprintCallable, BlueprintPure, Category= "Axis")
		inline float MovementXAxis() { return _movementXAxis; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category= "Axis")
		inline float MovementYAxis() { return _movementYAxis; }

	void MoveForward(const float Axis);
	void MoveRight(const float Axis);
	void CharJump();
	
	
};
