// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ElementalPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AElementalPlayerCharacter::AElementalPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	:AElementalCharacter(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	_springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	_thirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));

	_springArmComp->SetupAttachment(GetRootComponent());
	_thirdPersonCamera->SetupAttachment(_springArmComp);
	
	_springArmComp->bEnableCameraLag = true;
	_springArmComp->bUsePawnControlRotation = false;
	bUseControllerRotationYaw = false;
}

void AElementalPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//Movement action maps binding 
	PlayerInputComponent->BindAxis(TEXT("MoveForwardBack"), this, &AElementalPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRightLeft"), this, &AElementalPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AElementalPlayerCharacter::TurnCamera);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AElementalPlayerCharacter::CameraUp);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AElementalPlayerCharacter::CharJump);
	PlayerInputComponent->BindAction(TEXT("Dodge"), IE_Pressed, this, &AElementalPlayerCharacter:: Dash);
}

void AElementalPlayerCharacter::TurnCamera(const float Axis)
{
	if(_cameraCanTurn == false)
		return;
	
	AddControllerYawInput(Axis * _cameraRotationSpeed);

	AController* ActorController = GetController();
	const FRotator ControlRotation = ActorController->GetControlRotation();

	const float ClampPitch = FMath::ClampAngle(ControlRotation.Pitch, _cameraPitchClamp.X, _cameraPitchClamp.Y);
	ActorController->SetControlRotation(FRotator(ClampPitch, ControlRotation.Yaw, 0));
}

void AElementalPlayerCharacter::CameraUp(const float Axis)
{
	if(_cameraCanTurn == false)
		return;
	
	AddControllerPitchInput(Axis * _cameraRotationSpeed);
}

