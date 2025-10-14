// VyniosNikolaos, Thesis, University of Patras, 2025
#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "InputActionValue.h"
#include "GAS/GASTPSGameplayAbilityTypes.h"
#include "GASTPSPlayerCharacter.generated.h"

class UInputAction;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class AGASTPSPlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()
	
public:
	AGASTPSPlayerCharacter();
	virtual void PawnClientRestart() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	class UCameraComponent* FollowCam;

	FVector GetLookRightDir();
	FVector GetLookFwdDir();
	FVector GetMoveFwdDir();
	
	
	/******************/
	/*      Input     */
	/******************/	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* GameplayInputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* JumpInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* LookInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* MoveInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* SprintInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* AimInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TMap<EAbilityInputID, UInputAction*> GameplayAbilityInputActions;

	void HandleLookInput(const FInputActionValue& InputActionValue);
	void HandleMoveInput(const FInputActionValue& InputActionValue);
	void HandleAbilityInput(const FInputActionValue& InputActionValue, EAbilityInputID InputID);
	void HandleSprintStarted(const FInputActionValue& InputActionValue);
	void HandleSprintCompleted(const FInputActionValue& InputActionValue);
	void HandleAimStarted(const FInputActionValue& InputActionValue);
	void HandleAimCompleted(const FInputActionValue& InputActionValue);

	/******************/
	/*     Sprint     */
	/******************/
	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	float SprintSpeedMultiplier = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Sprint")
	float SprintJumpVelocityMultiplier = 1.2f;

	bool bIsSprinting = false;
	float DefaultMaxWalkSpeed;
	float DefaultJumpZVelocity;

	/******************/
	/*      Aim       */
	/******************/
	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AimFieldOfView = 70.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	float AimCameraZoomSpeed = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Aim")
	FVector AimSocketOffset = FVector(0.0f, 50.0f, 20.0f);

	bool bIsAiming = false;
	float DefaultFieldOfView;
	FVector DefaultSocketOffset;

	void UpdateCameraFOV(float DeltaTime);
	void UpdateCameraOffset(float DeltaTime);
	void SetCharacterOrientationMode(bool bOrientToMovement);

	/*******************/
	/*   Crosshair     */
	/*******************/
	UPROPERTY(EditDefaultsOnly, Category = "Crosshair")
	TSubclassOf<class UCrosshairWidget> CrosshairWidgetClass;

	UPROPERTY()
	class UCrosshairWidget* CrosshairWidget;

	void ShowCrosshair();
	void HideCrosshair();
	void UpdateCrosshairState();

public:
	virtual void Tick(float DeltaTime) override;

private:
	/*******************/
	/* Death & Respawn */
	/*******************/
	virtual void OnDead() override;
	virtual void OnRespawn() override;


};
