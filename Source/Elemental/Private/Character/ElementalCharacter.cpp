// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ElementalCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Character/CharacterCurveMovementComponent.h"


// Sets default values
AElementalCharacter::AElementalCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterCurveMovementComponent>(CharacterMovementComponentName))
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	_springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	_thirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));

	_springArmComp->SetupAttachment(GetRootComponent());
	_thirdPersonCamera->SetupAttachment(_springArmComp);
	
	_springArmComp->bEnableCameraLag = true;
	_springArmComp->bUsePawnControlRotation = true;
	CurrentNumDashes = _numOfDashes;
	bUseControllerRotationYaw = false;
}

// Called when the game starts or when spawned
void AElementalCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called to bind functionality to input
void AElementalCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//Movement action maps binding 
	PlayerInputComponent->BindAxis(TEXT("MoveForwardBack"), this, &AElementalCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRightLeft"), this, &AElementalCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AElementalCharacter::TurnCamera);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AElementalCharacter::CameraUp);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AElementalCharacter::CharJump);
	PlayerInputComponent->BindAction(TEXT("Dodge"), IE_Pressed, this, &AElementalCharacter:: Dash);
}

void AElementalCharacter::MoveForward(const float Axis)
{
	static_cast<UCharacterCurveMovementComponent*>(GetCharacterMovement())->AddCurveForwardBackMovement(GetActorForwardVector(), Axis);
	
}

void AElementalCharacter::MoveRight(const float Axis)
{
	static_cast<UCharacterCurveMovementComponent*>(GetCharacterMovement())->AddCurveRightLeftMovement(GetActorRightVector(), Axis);
}

void AElementalCharacter::CharJump()
{
	Jump();
}

void AElementalCharacter::TurnCamera(const float Axis)
{
	AddControllerYawInput(Axis * _cameraRotationSpeed);

	AController* ActorController = GetController();
	const FRotator ControlRotation = ActorController->GetControlRotation();

	const float ClampPitch = FMath::ClampAngle(ControlRotation.Pitch, _cameraPitchClamp.X, _cameraPitchClamp.Y);
	ActorController->SetControlRotation(FRotator(ClampPitch, ControlRotation.Yaw, 0));
}

void AElementalCharacter::CameraUp(const float Axis)
{
	AddControllerPitchInput(Axis * _cameraRotationSpeed);
}

void AElementalCharacter::Dash()
{
	if(CurrentNumDashes <= 0 || CanDash == false)
		return;

	const FVector Vel = GetVelocity();
	const FVector DashVel = FVector(Vel.X, Vel.Y, 0) * _dashForceMultiplier;
	LaunchCharacter(DashVel, false, false);
	CurrentNumDashes--;

	if(CurrentNumDashes <= 0)
		CanDash = false;
	
	GetWorldTimerManager().SetTimer(CoolDownTimerHandle, this, &AElementalCharacter::ResetDashes, _dashCoolDown, false);
}

void AElementalCharacter::ResetDashes()
{
	UE_LOG(LogTemp, Warning, TEXT("IsOnGround: %s"), GetMovementComponent()->IsFalling()? TEXT("false") : TEXT("true"));
	
    if(CoolDownTimerHandle.IsValid())
        CoolDownTimerHandle.Invalidate();
	
	if(GetMovementComponent()->IsFalling())
	{
		return;
	}

	CurrentNumDashes = _numOfDashes;
	CanDash = true;
}

void AElementalCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if(CanDash == false && !CoolDownTimerHandle.IsValid())
		ResetDashes();
}