#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "SkillButtonWidget.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillButtonClicked, FName, SkillID);

UCLASS(Abstract, BlueprintType)
class TEATIMEWITCH_API USkillButtonWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	void SetSkillData(FName InSkillID, const FText& DisplayName, int32 MPCost, bool bAffordable);

	UPROPERTY(BlueprintAssignable)
	FOnSkillButtonClicked OnClicked;

protected:
	virtual void NativeDestruct() override;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> SelectButton;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> SkillNameText;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> MPCostText;

	FName SkillID;

	UFUNCTION()
	void OnButtonClicked();
};
