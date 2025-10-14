// VyniosNikolaos, Thesis, University of Patras, 2025

#include "Player/GASTPSPlayerCharacter.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "AbilitySystemComponent.h"
#include "GAS/AbilitySystemStatics.h"
#include "Widgets/CrosshairWidget.h"


AGASTPSPlayerCharacter::AGASTPSPlayerCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true;

	FollowCam = CreateDefaultSubobject<UCameraComponent>(TEXT("View Cam"));
	FollowCam->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);

	// Store default movement values
	DefaultMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	DefaultJumpZVelocity = GetCharacterMovement()->JumpZVelocity;
	DefaultFieldOfView = 90.0f;
	DefaultSocketOffset = FVector::ZeroVector;

	PrimaryActorTick.bCanEverTick = true;
}

void AGASTPSPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	APlayerController* OwningPlayerController = GetController<APlayerController>();
	if (OwningPlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
			OwningPlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (InputSubsystem)
		{
			InputSubsystem->RemoveMappingContext(GameplayInputMappingContext);
			InputSubsystem->AddMappingContext(GameplayInputMappingContext, 0);
		}

		// Store default FOV, socket offset, and create crosshair
		if (FollowCam)
		{
			DefaultFieldOfView = FollowCam->FieldOfView;
		}
		if (CameraBoom)
		{
			DefaultSocketOffset = CameraBoom->SocketOffset;
		}
		ShowCrosshair();
	}
}

void AGASTPSPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Triggered, this, &AGASTPSPlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &AGASTPSPlayerCharacter::HandleLookInput);
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &AGASTPSPlayerCharacter::HandleMoveInput);
		
		EnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Started, this, &AGASTPSPlayerCharacter::HandleSprintStarted);
		EnhancedInputComponent->BindAction(SprintInputAction, ETriggerEvent::Completed, this, &AGASTPSPlayerCharacter::HandleSprintCompleted);

		EnhancedInputComponent->BindAction(AimInputAction, ETriggerEvent::Started, this, &AGASTPSPlayerCharacter::HandleAimStarted);
		EnhancedInputComponent->BindAction(AimInputAction, ETriggerEvent::Completed, this, &AGASTPSPlayerCharacter::HandleAimCompleted);

		for (const TPair<EAbilityInputID, UInputAction*>& InputActionPair : GameplayAbilityInputActions)
		{
			EnhancedInputComponent->BindAction(InputActionPair.Value, ETriggerEvent::Triggered, this, &AGASTPSPlayerCharacter::HandleAbilityInput, InputActionPair.Key);
		}

	}
}

void AGASTPSPlayerCharacter::HandleLookInput(const FInputActionValue& InputActionValue)
{
	FVector2D InputVal = InputActionValue.Get<FVector2D>();

	AddControllerPitchInput(-InputVal.Y);
	AddControllerYawInput(InputVal.X);
}

void AGASTPSPlayerCharacter::HandleMoveInput(const FInputActionValue& InputActionValue)
{
	FVector2D InputVal = InputActionValue.Get<FVector2D>();
	InputVal.Normalize();

	AddMovementInput(GetMoveFwdDir()*InputVal.Y + GetLookRightDir()*InputVal.X, true);
}

void AGASTPSPlayerCharacter::HandleAbilityInput(const FInputActionValue& InputActionValue, EAbilityInputID InputID)
{
	bool bPressed = InputActionValue.Get<bool>();
	if (bPressed)
	{
		GetAbilitySystemComponent()->AbilityLocalInputPressed(int32(InputID));
	}
	else
	{
		GetAbilitySystemComponent()->AbilityLocalInputReleased(int32(InputID));
	}

	if (InputID == EAbilityInputID::BasicAttack)
	{
		FGameplayTag BasicAttackTag = bPressed ? UAbilitySystemStatics::GetBasicAttackInputPressedTag() : UAbilitySystemStatics::GetBasicAttackInputReleasedTag();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this,BasicAttackTag, FGameplayEventData());
		//Server_SendGameplayEventToSelf(BasicAttackTag, FGameplayEventData());
	}
}

void AGASTPSPlayerCharacter::OnDead()
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		DisableInput(PlayerController);
	}
}

void AGASTPSPlayerCharacter::OnRespawn()
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		EnableInput(PlayerController);
	}
}

FVector AGASTPSPlayerCharacter::GetLookRightDir()
{
	return FollowCam->GetRightVector();
}

FVector AGASTPSPlayerCharacter::GetLookFwdDir()
{
	return FollowCam->GetForwardVector();
}

FVector AGASTPSPlayerCharacter::GetMoveFwdDir()
{
	return FVector::CrossProduct(GetLookRightDir(), FVector::UpVector);
}

void AGASTPSPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateCameraFOV(DeltaTime);
	UpdateCameraOffset(DeltaTime);
}

void AGASTPSPlayerCharacter::HandleSprintStarted(const FInputActionValue& InputActionValue)
{
	// Cancel aim if trying to sprint
	if (bIsAiming)
	{
		HandleAimCompleted(InputActionValue);
	}

	if (!bIsSprinting)
	{
		bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed * SprintSpeedMultiplier;
		GetCharacterMovement()->JumpZVelocity = DefaultJumpZVelocity * SprintJumpVelocityMultiplier;
	}
}

void AGASTPSPlayerCharacter::HandleAimStarted(const FInputActionValue& InputActionValue)
{
	// Cancel sprint if trying to aim
	if (bIsSprinting)
	{
		HandleSprintCompleted(InputActionValue);
	}

	if (!bIsAiming)
	{
		bIsAiming = true;
		SetCharacterOrientationMode(false); // Orient to camera when aiming
		UpdateCrosshairState();
		GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed/2 ;
		
		// Add aim tag for animation
		UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
		if (ASC)
		{
			ASC->AddLooseGameplayTag(UAbilitySystemStatics::GetAimStatTag());
		}
	}
}

void AGASTPSPlayerCharacter::HandleAimCompleted(const FInputActionValue& InputActionValue)
{
	if (bIsAiming)
	{
		bIsAiming = false;
		SetCharacterOrientationMode(true); // Orient to movement when not aiming
		UpdateCrosshairState();
		GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;

		// Remove aim tag for animation
		UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
		if (ASC)
		{
			ASC->RemoveLooseGameplayTag(UAbilitySystemStatics::GetAimStatTag());
		}
	}
}

void AGASTPSPlayerCharacter::UpdateCameraFOV(float DeltaTime)
{
	if (!FollowCam) return;

	float TargetFOV = bIsAiming ? AimFieldOfView : DefaultFieldOfView;
	float CurrentFOV = FollowCam->FieldOfView;

	if (FMath::Abs(CurrentFOV - TargetFOV) > 0.1f)
	{
		float NewFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, AimCameraZoomSpeed);
		FollowCam->SetFieldOfView(NewFOV);
	}
}

void AGASTPSPlayerCharacter::UpdateCameraOffset(float DeltaTime)
{
	if (!CameraBoom) return;

	FVector TargetOffset = bIsAiming ? AimSocketOffset : DefaultSocketOffset;
	FVector CurrentOffset = CameraBoom->SocketOffset;

	if (!CurrentOffset.Equals(TargetOffset, 0.1f))
	{
		FVector NewOffset = FMath::VInterpTo(CurrentOffset, TargetOffset, DeltaTime, AimCameraZoomSpeed);
		CameraBoom->SocketOffset = NewOffset;
	}
}

void AGASTPSPlayerCharacter::SetCharacterOrientationMode(bool bOrientToMovement)
{
	GetCharacterMovement()->bOrientRotationToMovement = bOrientToMovement;
	bUseControllerRotationYaw = !bOrientToMovement;
}

void AGASTPSPlayerCharacter::ShowCrosshair()
{
	if (!CrosshairWidgetClass) return;

	APlayerController* PC = GetController<APlayerController>();
	if (PC && PC->IsLocalController())
	{
		if (!CrosshairWidget)
		{
			CrosshairWidget = CreateWidget<UCrosshairWidget>(PC, CrosshairWidgetClass);
			if (CrosshairWidget)
			{
				CrosshairWidget->AddToViewport();
				CrosshairWidget->ShowCrosshair();
			}
		}
	}
}

void AGASTPSPlayerCharacter::HideCrosshair()
{
	if (CrosshairWidget)
	{
		CrosshairWidget->RemoveFromParent();
		CrosshairWidget = nullptr;
	}
}

void AGASTPSPlayerCharacter::UpdateCrosshairState()
{
	if (CrosshairWidget)
	{
		CrosshairWidget->SetAimingState(bIsAiming);
	}
}

void AGASTPSPlayerCharacter::HandleSprintCompleted(const FInputActionValue& InputActionValue)
{
	if (bIsSprinting)
	{
		bIsSprinting = false;
		GetCharacterMovement()->MaxWalkSpeed = DefaultMaxWalkSpeed;
		GetCharacterMovement()->JumpZVelocity = DefaultJumpZVelocity;
	}
}

