#pragma once

#include "../Dialogue/TTWDialogueTypes.h"
#include "../TeaCraft/TeaCraftTypes.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EliaCharacter.generated.h"

class UTeaCraftingComponent;
class UTeaCraftingWidget;
class UPaperFlipbook;
class UPaperFlipbookComponent;
class USpringArmComponent;
class UCameraComponent;
class UArrowComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Idle,
	Walking,
};

UENUM()
enum class EFacingDirection : uint8
{
	North UMETA(DisplayName = "North"),
	NorthEast UMETA(DisplayName = "NorthEast"),
	East UMETA(DisplayName = "East"),
	SouthEast UMETA(DisplayName = "SouthEast"),
	South UMETA(DisplayName = "South"),
	SouthWest UMETA(DisplayName = "SouthWest"),
	West UMETA(DisplayName = "West"),
	NorthWest UMETA(DisplayName = "NorthWest"),
};

struct FResolvedSprite
{
	int32 SlotIndex;
	bool bMirrorX;
};

static FResolvedSprite ResolveSprite(EFacingDirection Dir)
{
	switch (Dir)
	{
	case EFacingDirection::North: return {0, false};
	case EFacingDirection::NorthEast: return {1, false};
	case EFacingDirection::East: return {2, false};
	case EFacingDirection::SouthEast: return {3, false};
	case EFacingDirection::South: return {4, false};
	case EFacingDirection::SouthWest: return {3, true};
	case EFacingDirection::West: return {2, true};
	case EFacingDirection::NorthWest: return {1, true};
	default: return {4, false};
	}
}

UCLASS()
class TEATIMEWITCH_API AEliaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEliaCharacter();

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintPure)
	class UTeaCraftingComponent* GetCraftingComp() const { return CraftingComp; }

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// === Components ===
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HD2D|Components")
	TObjectPtr<UPaperFlipbookComponent> Flipbook;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HD2D|Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HD2D|Components")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HD2D|Components")
	TObjectPtr<UArrowComponent> FacingArrow;

	// === Input ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HD2D|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HD2D|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HD2D|Input")
	TObjectPtr<UInputAction> InteractAction;

	// === Sprites (8 directions) ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HD2D|Sprites")
	TArray<TObjectPtr<UPaperFlipbook>> Idle_Flipbooks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HD2D|Sprites")
	TArray<TObjectPtr<UPaperFlipbook>> Walk_Flipbooks;

	// === Tuning ===
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HD2D|Tuning")
	float WalkingSpeedThresholdSq = 100.f; // |v|^2 임계값 (10 cm/s 정도)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HD2D|Tuning")
	bool bShowDebugMessages = false;

	// === State (read-only at runtime) ===
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HD2D|State")
	ECharacterState CurrentState = ECharacterState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HD2D|State")
	EFacingDirection CurrentDirection = EFacingDirection::South;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TeaCraft")
	TObjectPtr<UTeaCraftingComponent> CraftingComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HD2D|Input")
	TObjectPtr<UInputAction> OpenTeaCraftAction;

private:
	void OnMove(const FInputActionValue& Value);

	void UpdateState();
	void UpdateFacingDirection();
	void UpdateFlipbook();
	void UpdateBillboard();


	void OnOpenTeaCraft(const FInputActionValue& Value);

	void OnInteractPressed(const FInputActionValue& Value);
};
