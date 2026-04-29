#include "UI/SkillButtonWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void USkillButtonWidget::SetSkillData(FName InSkillID, const FText& DisplayName, int32 MPCost, bool bAffordable)
{
	SkillID = InSkillID;
	if (SkillNameText) { SkillNameText->SetText(DisplayName); }
	if (MPCostText)
	{
		MPCostText->SetText(FText::FromString(FString::Printf(TEXT("MP %d"), MPCost)));
	}
	if (SelectButton)
	{
		SelectButton->SetIsEnabled(bAffordable);
		if (!SelectButton->OnClicked.IsAlreadyBound(this, &USkillButtonWidget::OnButtonClicked))
		{
			SelectButton->OnClicked.AddDynamic(this, &USkillButtonWidget::OnButtonClicked);
		}
	}
}

void USkillButtonWidget::NativeDestruct()
{
	if (SelectButton)
	{
		SelectButton->OnClicked.RemoveDynamic(this, &USkillButtonWidget::OnButtonClicked);
	}
	Super::NativeDestruct();
}

void USkillButtonWidget::OnButtonClicked()
{
	OnClicked.Broadcast(SkillID);
}
