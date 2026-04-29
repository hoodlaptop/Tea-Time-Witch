#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "BattleCharacterWidget.generated.h"

class UTextBlock;
class UProgressBar;
class UImage;
class UBorder;
class UButton;
class ABattleCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBattleCharacterClicked, ABattleCharacter*, Character);

UCLASS(Abstract, BlueprintType)
class TEATIMEWITCH_API UBattleCharacterWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void BindCharacter(ABattleCharacter* InCharacter);
	void Refresh();
	void SetSelectable(bool bInSelectable);
	void SetHighlight(bool bShow);

	UFUNCTION(BlueprintPure)
	ABattleCharacter* GetTargetCharacter() const { return TargetCharacter; }

	UPROPERTY(BlueprintAssignable)
	FOnBattleCharacterClicked OnCharacterClicked;

protected:
	virtual void NativeDestruct() override;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> NameText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> HPBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> HPText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UProgressBar> MPBar;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> MPText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> PortraitImage;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBorder> HighlightBorder;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> SelectButton;

	UPROPERTY()
	TObjectPtr<ABattleCharacter> TargetCharacter;

	bool bSelectable = false;

	UFUNCTION()
	void OnSelectButtonClicked();

	UFUNCTION()
	void HandleHPChanged(int32 NewHP, int32 MaxHP);

	UFUNCTION()
	void HandleDied();
};
