// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ElementalCharacter.h"
#include "ElementalPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ELEMENTAL_API AElementalPlayerCharacter : public AElementalCharacter
{
	GENERATED_BODY()
protected:
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category ="Camera")
    	class USpringArmComponent* _springArmComp = nullptr;
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Camera")
    	class UCameraComponent* _thirdPersonCamera = nullptr;
    UPROPERTY(EditAnywhere, Category="Camera")
    	float _cameraRotationSpeed = 0.5;
    UPROPERTY(EditAnywhere, Category="Camera")
    	FVector2D _cameraPitchClamp = FVector2D(-137, 0);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera")
		bool _cameraCanTurn = true;
    
public:
    // Sets default values for this character's properties
	explicit AElementalPlayerCharacter(const FObjectInitializer& ObjectInitializer);

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    void TurnCamera(const float Axis);
    void CameraUp(const float Axis);
	
};
