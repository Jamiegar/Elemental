// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ElementalPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AElementalPlayerCharacter::AElementalPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	:AElementalCharacter(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true; //Actor can tick
	_springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm")); //Create a spring arm component
	_thirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera")); //Create a camera component 

	_springArmComp->SetupAttachment(GetRootComponent()); //Attach the spring arm to the root component
	_thirdPersonCamera->SetupAttachment(_springArmComp); //Attach the camera to the spring arm
	
	_springArmComp->bEnableCameraLag = true; //Enables camera lag on the spring arm so the camera smoothly moves 
	_springArmComp->bUsePawnControlRotation = true; //Set the pawn control rotation to true so the mouse can rotate the character and spring arm
	bUseControllerRotationYaw = false; 
}

void AElementalPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//Movement action maps binding, this binds all of the input to functions that can handle that input
	PlayerInputComponent->BindAxis(TEXT("MoveForwardBack"), this, &AElementalPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRightLeft"), this, &AElementalPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AElementalPlayerCharacter::TurnCamera);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AElementalPlayerCharacter::CameraUp);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AElementalPlayerCharacter::CharJump);
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

