#include "IngredientButtonWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UIngredientButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Btn)
	{
		Btn->OnClicked.AddDynamic(this, &UIngredientButtonWidget::HandleClicked);
	}
}

void UIngredientButtonWidget::SetData(FName InID, FText InDisplayName, int32 InCount, UTexture2D* InIcon)
{
	IngredientID = InID;
	
	if (NameText)
	{
		NameText->SetText(InDisplayName);
	}
	
	if (CountText)
	{
		CountText->SetText(FText::AsNumber(InCount));
	}
	
	if (IconImage && InIcon)
	{
		IconImage->SetBrushFromTexture(InIcon);
	}
}

void UIngredientButtonWidget::HandleClicked()
{
	OnIngredientClicked.Broadcast(IngredientID);
}
