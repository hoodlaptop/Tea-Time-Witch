#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TeaTimeWitchPlayerController.generated.h"

UENUM(BlueprintType)
enum class ETeaShopInputState : uint8
{
	Game,
	Dialogue,
	Crafting,
	Battle
};

UCLASS()
class TEATIMEWITCH_API ATeaTimeWitchPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATeaTimeWitchPlayerController();

	UFUNCTION(BlueprintCallable)
	void SetTeaShopInputState(ETeaShopInputState NewState, class UUserWidget* FocusWidget);

	UFUNCTION(BlueprintPure)
	ETeaShopInputState GetTeaShopInputState() const { return CurrentState; }

private:
	ETeaShopInputState CurrentState = ETeaShopInputState::Game;
};
