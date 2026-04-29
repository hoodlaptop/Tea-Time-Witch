#pragma once

#include "CoreMinimal.h"
#include "TTWGameModeBase.h"
#include "GameFramework/GameModeBase.h"
#include "TeaShopGameMode.generated.h"

class UTeaShopHUDWidget;
class UDialogueWidget;
class UTeaCraftingWidget;
class UDialogueSystem;
class AEliaCharacter;
class ANPCBase;

UCLASS()
class TEATIMEWITCH_API ATeaShopGameMode : public ATTWGameModeBase
{
	GENERATED_BODY()

public:
	ATeaShopGameMode();

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UDialogueWidget> DialogueWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UTeaCraftingWidget> TeaCraftingWidgetClass;

	UFUNCTION(BlueprintPure)
	bool IsTeaCraftOpen() const { return ActiveCraftingWidget != nullptr; }

	UFUNCTION(BlueprintCallable)
	void CloseTeaCraft();
	UFUNCTION(BlueprintCallable)
	void OpenTeaCraftDirect(AEliaCharacter* Elia);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleDialogueStarted(ANPCBase* Speaker);
	UFUNCTION()
	void HandleDialogueEnded();
	UFUNCTION()
	void HandleDialogueAction(EDialogueAction Action, FName Param);
	UFUNCTION()
	void HandleBrewComplete(const FBrewResult& Result);

private:
	UPROPERTY()
	TObjectPtr<UTeaShopHUDWidget> ShopHUD;
	UPROPERTY()
	TObjectPtr<UDialogueWidget> ActiveDialogueWidget;
	UPROPERTY()
	TObjectPtr<UTeaCraftingWidget> ActiveCraftingWidget;

	FName ExpectedRecipeID;
	bool bBrewBound = false;

	AEliaCharacter* GetElia() const;
};
