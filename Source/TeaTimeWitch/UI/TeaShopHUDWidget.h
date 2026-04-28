#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "TeaShopHUDWidget.generated.h"

class UOverlay;
class UDialogueWidget;
class UTeaCraftingWidget;
class UTextBlock;

UCLASS(Abstract, BlueprintType)
class TEATIMEWITCH_API UTeaShopHUDWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetDialogueWidget(UDialogueWidget* Widget);
	void ClearDialogueWidget();

	void SetTeaCraftWidget(UTeaCraftingWidget* Widget);
	void ClearTeaCraftWidget();

	void SetDebugText(const FString& Text);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> DialogueSlot;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> CraftingSlot;
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> DebugInfoText;
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> ControlGuideText;
};
