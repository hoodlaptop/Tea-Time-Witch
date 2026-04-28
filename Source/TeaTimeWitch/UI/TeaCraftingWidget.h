#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "../TeaCraft/TeaCraftTypes.h"
#include "TeaCraftingWidget.generated.h"

class UTeaCraftingComponent;
class UUniformGridPanel;
class UButton;
class UTextBlock;
class UIngredientButtonWidget;

UCLASS(Abstract, BlueprintType)
class TEATIMEWITCH_API UTeaCraftingWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitWithComponent(UTeaCraftingComponent* InComp);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> IngredientGrid;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BrewButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ClearButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ResultText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Slot0Button;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Slot1Button;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Slot2Button;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Slot0Text;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Slot1Text;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Slot2Text;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CurrentSelectionText;

	UPROPERTY(EditDefaultsOnly, Category = "TeaCraft|Widgets")
	TSubclassOf<UIngredientButtonWidget> IngredientButtonClass;

	UPROPERTY(EditDefaultsOnly, Category = "TeaCraft|UI")
	int32 GridColumnCount = 4;

	UPROPERTY()
	TObjectPtr<UTeaCraftingComponent> TeaCraftComp;

	UFUNCTION()
	void OnIngredientClicked(FName IngredientID);
	UFUNCTION()
	void OnBrewClicked();
	UFUNCTION()
	void OnClearClicked();

	UFUNCTION()
	void OnSlot0Clicked();
	UFUNCTION()
	void OnSlot1Clicked();
	UFUNCTION()
	void OnSlot2Clicked();

	UFUNCTION()
	void HandleInventoryChanged();
	UFUNCTION()
	void HandleSelectionChanged();
	UFUNCTION()
	void HandleBrewComplete(const FBrewResult& Result);

	void RefreshIngredientGrid();
	void RefreshSelectionSlots();
};
