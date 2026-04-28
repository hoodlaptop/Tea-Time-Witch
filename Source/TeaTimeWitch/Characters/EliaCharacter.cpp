#include "EliaCharacter.h"

#include "DialogueSystem.h"
#include "../UI/DialogueWidget.h"
#include "../Dialogue/TTWDialogueTypes.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "NPCBase.h"
#include "PaperFlipbook.h"
#include "PaperFlipbookComponent.h"
#include "TeaCraftingComponent.h"
#include "TeaCraftingWidget.h"
#include "TeaShopGameMode.h"
#include "TeaTimeWitchPlayerController.h"
#include "Blueprint/UserWidget.h"

AEliaCharacter::AEliaCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(34.f, 80.f);

	Flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
	Flipbook->SetupAttachment(RootComponent);
	Flipbook->SetRelativeLocation(FVector(0.f, 0.f, -80.f));
	Flipbook->SetCastShadow(true);
	Flipbook->bCastDynamicShadow = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 1000.f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;
	SpringArm->SetRelativeRotation(FRotator(-40.f, 45.f, 0.f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;
	Camera->SetFieldOfView(35.f);

	FacingArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("FacingArrow"));
	FacingArrow->SetupAttachment(RootComponent);
	FacingArrow->ArrowSize = 1.5f;
	FacingArrow->bIsScreenSizeScaled = true;
	FacingArrow->SetHiddenInGame(false);

	UCharacterMovementComponent* Move = GetCharacterMovement();
	Move->bOrientRotationToMovement = false; // 우리가 직접 처리
	Move->MaxWalkSpeed = 400.f;
	Move->BrakingDecelerationWalking = 2000.f;
	Move->GroundFriction = 8.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	Idle_Flipbooks.SetNum(5);
	Walk_Flipbooks.SetNum(5);

	CraftingComp = CreateDefaultSubobject<UTeaCraftingComponent>(TEXT("CraftingComp"));
}

void AEliaCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
}

void AEliaCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateState();
	UpdateFacingDirection();
	UpdateBillboard();
}

void AEliaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AEliaCharacter::OnMove);
		}

		if (OpenTeaCraftAction)
		{
			EIC->BindAction(OpenTeaCraftAction, ETriggerEvent::Started,
			                this, &AEliaCharacter::OnOpenTeaCraft);
		}

		if (InteractAction)
		{
			EIC->BindAction(InteractAction, ETriggerEvent::Started,
			                this, &AEliaCharacter::OnInteractPressed);
		}
	}
}

void AEliaCharacter::OnMove(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (Axis.IsNearlyZero() || !Camera) return;

	FVector CamFwd = Camera->GetForwardVector();
	CamFwd.Z = 0.0f;
	CamFwd.Normalize();

	FVector CamRight = FVector::CrossProduct(FVector::UpVector, CamFwd);
	CamRight.Normalize();

	const FVector MoveDir = (CamFwd * Axis.Y) + (CamRight * Axis.X);
	AddMovementInput(MoveDir, 1.f);
}

void AEliaCharacter::UpdateState()
{
	const float SpeedSq = GetVelocity().SizeSquared2D();
	const ECharacterState NewState = (SpeedSq > WalkingSpeedThresholdSq)
		                                 ? ECharacterState::Walking
		                                 : ECharacterState::Idle;

	if (NewState != CurrentState)
	{
		CurrentState = NewState;
		UpdateFlipbook();
	}
}

void AEliaCharacter::UpdateFacingDirection()
{
	const FVector Velocity = GetVelocity();

	if (Velocity.SizeSquared2D() < 1.0f) return;
	if (!Camera) return;

	FVector CamFwd = Camera->GetForwardVector();
	CamFwd.Z = 0.0f;
	CamFwd.Normalize();

	FVector CamRight = FVector::CrossProduct(FVector::UpVector, CamFwd);
	CamRight.Normalize();

	const FVector VelDir = Velocity.GetSafeNormal2D();
	const float ForwardDot = FVector::DotProduct(VelDir, CamFwd);
	const float RightDot = FVector::DotProduct(VelDir, CamRight);

	float AngleDeg = FMath::RadiansToDegrees(FMath::Atan2(RightDot, ForwardDot));
	if (AngleDeg < 0.0f)
	{
		AngleDeg += 360.f;
	}

	const int32 DirIndex = FMath::RoundToInt(AngleDeg / 45.0f) % 8;
	const EFacingDirection NewDir = static_cast<EFacingDirection>(DirIndex);

	if (NewDir != CurrentDirection)
	{
		CurrentDirection = NewDir;
		UpdateFlipbook();

		if (bShowDebugMessages && GEngine)
		{
			const UEnum* EnumPtr = StaticEnum<EFacingDirection>();
			const FString Name = EnumPtr
				                     ? EnumPtr->GetNameStringByValue(static_cast<int64>(NewDir))
				                     : TEXT("?");
			GEngine->AddOnScreenDebugMessage(
				-1, 15.f, FColor::Yellow,
				FString::Printf(TEXT("Facing : %s (Angle %.1f)"), *Name, AngleDeg));
		}

		if (FacingArrow)
		{
			const float WorldYaw = FMath::RadiansToDegrees(FMath::Atan2(VelDir.Y, VelDir.X));
			FacingArrow->SetWorldRotation(FRotator(0.0f, WorldYaw, 0.0f));
		}
	}
}

void AEliaCharacter::UpdateFlipbook()
{
	if (!Flipbook) { return; }

	const FResolvedSprite Resolved = ResolveSprite(CurrentDirection);
	const TArray<TObjectPtr<UPaperFlipbook>>& Source =
		(CurrentState == ECharacterState::Walking) ? Walk_Flipbooks : Idle_Flipbooks;

	if (!Source.IsValidIndex(Resolved.SlotIndex)) { return; }

	UPaperFlipbook* Target = Source[Resolved.SlotIndex];
	if (Target && Flipbook->GetFlipbook() != Target)
	{
		Flipbook->SetFlipbook(Target);
	}

	FVector Scale = Flipbook->GetRelativeScale3D();
	const float AbsX = FMath::Abs(Scale.X);
	Scale.X = Resolved.bMirrorX ? -AbsX : AbsX;
	Flipbook->SetRelativeScale3D(Scale);
}

void AEliaCharacter::UpdateBillboard()
{
	if (!Flipbook || !Camera) { return; }

	const float CamYaw = Camera->GetComponentRotation().Yaw;

	Flipbook->SetWorldRotation(FRotator(0.f, CamYaw + 90.f, 0.f));
}

void AEliaCharacter::OnOpenTeaCraft(const FInputActionValue&)
{
	UDialogueSystem* DS = GetWorld()->GetSubsystem<UDialogueSystem>();
	if (DS && DS->bIsActive) { return; }   // 대화 중 가드

	auto* TPC = Cast<ATeaTimeWitchPlayerController>(GetController());
	auto* GM = GetWorld()->GetAuthGameMode<ATeaShopGameMode>();
	if (!TPC || !GM) { return; }

	// 토글: GameMode가 만든 HUD에 위젯이 있으면 닫고, 없으면 열기
	if (GM->IsTeaCraftOpen())
	{
		GM->CloseTeaCraft();
		TPC->SetTeaShopInputState(ETeaShopInputState::Game, nullptr);
	}
	else
	{
		GM->OpenTeaCraftDirect(this);  // 대화 컨텍스트 없이 단순 열기
	}
}

void AEliaCharacter::OnInteractPressed(const FInputActionValue& /*Value*/)
{
	UDialogueSystem* DS = GetWorld()->GetSubsystem<UDialogueSystem>();
	if (DS && DS->bIsActive) { return; }

	TArray<FOverlapResult> Hits;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(NPCFind), false, this);
	GetWorld()->OverlapMultiByObjectType(
		Hits,
		GetActorLocation(),
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(180.f),
		Params);

	ANPCBase* Best = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();
	for (const FOverlapResult& H : Hits)
	{
		ANPCBase* NPC = Cast<ANPCBase>(H.GetActor());
		if (!NPC) { continue; }
		const float D = FVector::DistSquared(GetActorLocation(), NPC->GetActorLocation());
		if (D < BestDistSq)
		{
			BestDistSq = D;
			Best = NPC;
		}
	}

	if (Best) { Best->TryStartDialogue(this); }
}
