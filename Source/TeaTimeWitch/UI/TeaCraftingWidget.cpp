#include "TeaCraftingWidget.h"
#include "IngredientButtonWidget.h"
#include "../TeaCraft/TeaCraftingComponent.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void UTeaCraftingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BrewButton) { BrewButton->OnClicked.AddDynamic(this, &UTeaCraftingWidget::OnBrewClicked); }
	if (ClearButton) { ClearButton->OnClicked.AddDynamic(this, &UTeaCraftingWidget::OnClearClicked); }
	if (Slot0Button) { Slot0Button->OnClicked.AddDynamic(this, &UTeaCraftingWidget::OnSlot0Clicked); }
	if (Slot1Button) { Slot1Button->OnClicked.AddDynamic(this, &UTeaCraftingWidget::OnSlot1Clicked); }
	if (Slot2Button) { Slot2Button->OnClicked.AddDynamic(this, &UTeaCraftingWidget::OnSlot2Clicked); }
}

void UTeaCraftingWidget::NativeDestruct()
{
	if (TeaCraftComp)
	{
		TeaCraftComp->OnInventoryChanged.RemoveAll(this);
		TeaCraftComp->OnSelectionChanged.RemoveAll(this);
		TeaCraftComp->OnBrewComplete.RemoveAll(this);
	}
	Super::NativeDestruct();
}

void UTeaCraftingWidget::InitWithComponent(UTeaCraftingComponent* InComp)
{
	if (!InComp) return;

	TeaCraftComp = InComp;
	TeaCraftComp->OnInventoryChanged.AddDynamic(this, &UTeaCraftingWidget::HandleInventoryChanged);
	TeaCraftComp->OnSelectionChanged.AddDynamic(this, &UTeaCraftingWidget::HandleSelectionChanged);
	TeaCraftComp->OnBrewComplete.AddDynamic(this, &UTeaCraftingWidget::HandleBrewComplete);
	RefreshIngredientGrid();
	RefreshSelectionSlots();
}

void UTeaCraftingWidget::OnIngredientClicked(FName IngredientID)
{
	if (TeaCraftComp)
	{
		TeaCraftComp->AddToSelection(IngredientID);
	}
}

void UTeaCraftingWidget::OnBrewClicked()
{
	if (TeaCraftComp) { TeaCraftComp->TryBrew(); }
}

void UTeaCraftingWidget::OnClearClicked()
{
	if (TeaCraftComp) { TeaCraftComp->ClearSelection(); }
}

void UTeaCraftingWidget::OnSlot0Clicked()
{
	if (TeaCraftComp) { TeaCraftComp->RemoveFromSelection(0); }
}

void UTeaCraftingWidget::OnSlot1Clicked()
{
	if (TeaCraftComp) { TeaCraftComp->RemoveFromSelection(1); }
}

void UTeaCraftingWidget::OnSlot2Clicked()
{
	if (TeaCraftComp) { TeaCraftComp->RemoveFromSelection(2); }
}

void UTeaCraftingWidget::HandleInventoryChanged()
{
	RefreshIngredientGrid();
}

void UTeaCraftingWidget::HandleSelectionChanged()
{
	RefreshSelectionSlots();
}

void UTeaCraftingWidget::HandleBrewComplete(const FBrewResult& Result)
{
	if (ResultText) { ResultText->SetText(Result.ResultMessage); }
	RefreshIngredientGrid();
	RefreshSelectionSlots();
}

void UTeaCraftingWidget::RefreshIngredientGrid()
{
	if (!IngredientGrid || !IngredientButtonClass || !TeaCraftComp) return;
	IngredientGrid->ClearChildren();

	const TArray<FName> IDs = TeaCraftComp->GetOwnedIngredientIDs();
	int32 Index = 0;
	for (const FName& ID : IDs)
	{
		FIngredientData Data;
		if (!TeaCraftComp->GetIngredientData(ID, Data)) { continue; }

		UIngredientButtonWidget* Btn = CreateWidget<UIngredientButtonWidget>(this, IngredientButtonClass);
		if (!Btn) { continue; }
		Btn->SetData(ID, Data.DisplayName, TeaCraftComp->GetIngredientCount(ID), Data.Icon);
		Btn->OnIngredientClicked.AddDynamic(this, &UTeaCraftingWidget::OnIngredientClicked);

		UUniformGridSlot* GridSlot = IngredientGrid->AddChildToUniformGrid(
			Btn, Index / GridColumnCount, Index % GridColumnCount);
		++Index;
	}
}

void UTeaCraftingWidget::RefreshSelectionSlots()
{
	if (!TeaCraftComp) return;
	const TArray<FName>& Sel = TeaCraftComp->GetCurrentSelection();

	auto SetSlot = [&](UTextBlock* T, int32 Idx)
	{
		if (!T) { return; }
		if (Sel.IsValidIndex(Idx))
		{
			FIngredientData Data;
			T->SetText(TeaCraftComp->GetIngredientData(Sel[Idx], Data) ? Data.DisplayName : FText::FromName(Sel[Idx]));
		}
		else
		{
			T->SetText(NSLOCTEXT("TeaCraft", "EmptySlot", "[ 빈 ]"));
		}
	};
	SetSlot(Slot0Text, 0);
	SetSlot(Slot1Text, 1);
	SetSlot(Slot2Text, 2);

	if (CurrentSelectionText)
	{
		FString Joined;
		for (int32 i = 0; i < Sel.Num(); ++i)
		{
			if (i > 0) { Joined += TEXT(", "); }
			Joined += Sel[i].ToString();
		}
		CurrentSelectionText->SetText(FText::FromString(FString::Printf(TEXT("선택: %s"), *Joined)));
	}
}
