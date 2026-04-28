#include "HD2DCameraPawn.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

AHD2DCameraPawn::AHD2DCameraPawn()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(SceneRoot);
	SpringArm->bDoCollisionTest = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bEnableCameraLag = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	ApplyCameraSettings();
}

void AHD2DCameraPawn::ApplyCameraSettings()
{
	if (!SpringArm || !Camera) { return; }

	SpringArm->TargetArmLength = ArmLength;
	SpringArm->SetRelativeRotation(FRotator(CameraPitch, CameraYaw, 0.f));

	if (bUseOrthographic)
	{
		Camera->SetProjectionMode(ECameraProjectionMode::Orthographic);
		Camera->SetOrthoWidth(OrthoWidth);
	}
	else
	{
		Camera->SetProjectionMode(ECameraProjectionMode::Perspective);
		Camera->SetFieldOfView(FieldOfView);
	}
}

void AHD2DCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	ApplyCameraSettings();
}

#if WITH_EDITOR
void AHD2DCameraPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ApplyCameraSettings();
}

void AHD2DCameraPawn::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	ApplyCameraSettings();
}
#endif
