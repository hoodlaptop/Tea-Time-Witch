#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HD2DCameraPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class TEATIMEWITCH_API AHD2DCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	AHD2DCameraPawn();

	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HD2D")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HD2D")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HD2D")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HD2D|Camera",
		meta = (ClampMin = "-89.0", ClampMax = "0.0"))
	float CameraPitch = -40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HD2D|Camera")
	float CameraYaw = -45.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HD2D|Camera",
		meta = (ClampMin = "200.0", ClampMax = "3000.0"))
	float ArmLength = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HD2D|Camera",
		meta = (ClampMin = "10.0", ClampMax = "90.0"))
	float FieldOfView = 35.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HD2D|Camera")
	bool bUseOrthographic = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HD2D|Camera",
		meta = (EditCondition = "bUseOrthographic"))
	float OrthoWidth = 2048.f;

	void ApplyCameraSettings();
};
