// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/ElementalCharacter.h"


// Sets default values
AElementalCharacter::AElementalCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ThirdPersonCamera"));

	SpringArmComp->SetupAttachment(GetRootComponent());
	ThirdPersonCamera->AttachToComponent(SpringArmComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	SpringArmComp->bEnableCameraLag = true;
	CurrentNumDashes = NumOfDashes;
	
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
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AElementalCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AElementalCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Dodge"), IE_Pressed, this, &AElementalCharacter:: Dash);
	
}

void AElementalCharacter::MoveForward(const float axis)
{
	AddMovementInput(GetActorForwardVector() * axis);
}

void AElementalCharacter::MoveRight(const float axis)
{
	AddMovementInput(GetActorRightVector() * axis);
}

void AElementalCharacter::TurnCamera(const float Axis)
{
	AddControllerYawInput(Axis * CameraRotationSpeed);
	AController* ActorController = GetController();
	
	const FRotator ControlRotation = ActorController->GetControlRotation();

	const float ClampPitch = FMath::ClampAngle(ControlRotation.Pitch, CameraPitchClamp.X, CameraPitchClamp.Y);
	ActorController->SetControlRotation(FRotator(ClampPitch, ControlRotation.Yaw, 0));
}

void AElementalCharacter::Dash()
{
	if(CurrentNumDashes <= 0 || CanDash == false)
		return;

	const FVector Vel = GetVelocity();
	const FVector DashVel = FVector(Vel.X, Vel.Y, 0) * DashForceMultiplier;
	LaunchCharacter(DashVel, false, false);
	CurrentNumDashes--;

	if(CurrentNumDashes <= 0)
		CanDash = false;
	
	GetWorldTimerManager().SetTimer(CoolDownTimerHandle, this, &AElementalCharacter::ResetDashes, DashCoolDown, false);
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

	CurrentNumDashes = NumOfDashes;
	CanDash = true;
}

void AElementalCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if(CanDash == false && !CoolDownTimerHandle.IsValid())
		ResetDashes();
}