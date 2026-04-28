#include "TeaShopHUDWidget.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "DialogueWidget.h"
#include "TeaCraftingWidget.h"

void UTeaShopHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ControlGuideText)
	{
		ControlGuideText->SetText(FText::FromString(TEXT("[WASD] 이동   [E] 상호작용   [T] 차 조합")));
	}
}

void UTeaShopHUDWidget::SetDialogueWidget(UDialogueWidget* Widget)
{
	if (!DialogueSlot || !Widget) { return; }
	DialogueSlot->ClearChildren();
    
	if (UOverlaySlot* OVSlot = Cast<UOverlaySlot>(DialogueSlot->AddChild(Widget)))
	{
		OVSlot->SetHorizontalAlignment(HAlign_Fill);
		OVSlot->SetVerticalAlignment(VAlign_Fill);
	}
}

void UTeaShopHUDWidget::ClearDialogueWidget()
{
	if (DialogueSlot) { DialogueSlot->ClearChildren(); }
}

void UTeaShopHUDWidget::SetTeaCraftWidget(UTeaCraftingWidget* Widget)
{
	if (!CraftingSlot || !Widget) { return; }
	CraftingSlot->ClearChildren();
    
	if (UOverlaySlot* OVSlot = Cast<UOverlaySlot>(CraftingSlot->AddChild(Widget)))
	{
		OVSlot->SetHorizontalAlignment(HAlign_Fill);
		OVSlot->SetVerticalAlignment(VAlign_Fill);
	}
}

void UTeaShopHUDWidget::ClearTeaCraftWidget()
{
	if (CraftingSlot) { CraftingSlot->ClearChildren(); }
}

void UTeaShopHUDWidget::SetDebugText(const FString& Text)
{
	if (DebugInfoText) { DebugInfoText->SetText(FText::FromString(Text)); }
}