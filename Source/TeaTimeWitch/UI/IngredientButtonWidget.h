#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "IngredientButtonWidget.generated.h"

class UButton;
class UTextBlock;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIngredientButtonClicked, FName, IngredientID);

UCLASS(Abstract)
class TEATIMEWITCH_API UIngredientButtonWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetData(FName InID, FText InDisplayName, int32 InCount, UTexture2D* InIcon);

	UPROPERTY(BlueprintAssignable)
	FOnIngredientButtonClicked OnIngredientClicked;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CountText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> IconImage;

	UPROPERTY()
	FName IngredientID;

	UFUNCTION()
	void HandleClicked();
};
