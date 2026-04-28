#include "TeaCraftingComponent.h"
#include "Engine/DataTable.h"

UTeaCraftingComponent::UTeaCraftingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTeaCraftingComponent::BeginPlay()
{
	Super::BeginPlay();
	for (const FName& ID : StartingIngredients) { AddIngredient(ID, 1); }
}

void UTeaCraftingComponent::AddIngredient(FName ID, int32 Count)
{
	if (ID == NAME_None || Count <= 0) { return; }
	Inventory.FindOrAdd(ID) += Count;
	OnInventoryChanged.Broadcast();
}

bool UTeaCraftingComponent::RemoveIngredient(FName ID, int32 Count)
{
	if (ID == NAME_None || Count <= 0) { return false; }
	int32* Cur = Inventory.Find(ID);
	if (!Cur || *Cur < Count) { return false; }
	*Cur -= Count;
	if (*Cur <= 0) { Inventory.Remove(ID); }
	OnInventoryChanged.Broadcast();
	return true;
}

bool UTeaCraftingComponent::AddToSelection(FName ID)
{
	if (CurrentSelection.Num() >= MaxSelectionSlots) { return false; }
	if (GetIngredientCount(ID) <= 0) { return false; }

	int32 AlreadySelected = 0;
	for (const FName& Sel : CurrentSelection) { if (Sel == ID) { ++AlreadySelected; } }
	if (AlreadySelected >= GetIngredientCount(ID)) { return false; } // 보유 수량 초과 방지

	CurrentSelection.Add(ID);
	OnSelectionChanged.Broadcast();
	return true;
}

void UTeaCraftingComponent::RemoveFromSelection(int32 SlotIndex)
{
	if (!CurrentSelection.IsValidIndex(SlotIndex)) { return; }
	CurrentSelection.RemoveAt(SlotIndex);
	OnSelectionChanged.Broadcast();
}

void UTeaCraftingComponent::ClearSelection()
{
	if (CurrentSelection.Num() == 0) { return; }
	CurrentSelection.Reset();
	OnSelectionChanged.Broadcast();
}

FBrewResult UTeaCraftingComponent::TryBrew()
{
	FBrewResult Result;

	if (CurrentSelection.Num() == 0)
	{
		Result.ResultMessage = NSLOCTEXT("TeaCraft", "Empty", "재료를 선택하세요.");
		OnBrewComplete.Broadcast(Result);
		return Result;
	}

	FName MatchedID = NAME_None;
	const bool bMatched = MatchRecipe(CurrentSelection, MatchedID);

	// 재료 차감 (성공/실패 무관)
	const TArray<FName> Used = CurrentSelection;
	for (const FName& ID : Used) { RemoveIngredient(ID, 1); }

	if (bMatched && RecipeTable)
	{
		const FRecipeData* Row = RecipeTable->FindRow<FRecipeData>(MatchedID, TEXT("TryBrew"));
		if (Row)
		{
			Result.bSuccess = true;
			Result.MatchedRecipeID = MatchedID;
			Result.TeaName = Row->TeaName;
			Result.ResultMessage = FText::Format(
				NSLOCTEXT("TeaCraft", "BrewSuccess", "{0}가 완성되었습니다."), Row->TeaName);
		}
	}
	else
	{
		Result.ResultMessage = NSLOCTEXT("TeaCraft", "BrewFail", "이상한 차가 만들어졌습니다.");
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 4.f,
		                                 Result.bSuccess ? FColor::Green : FColor::Yellow,
		                                 Result.ResultMessage.ToString());
	}

	ClearSelection();
	OnBrewComplete.Broadcast(Result);
	return Result;
}

bool UTeaCraftingComponent::GetIngredientData(FName ID, FIngredientData& OutData) const
{
	if (!IngredientTable) { return false; }
	const FIngredientData* Row = IngredientTable->FindRow<FIngredientData>(ID, TEXT("GetIngredientData"));
	if (!Row) { return false; }
	OutData = *Row;
	return true;
}

TArray<FName> UTeaCraftingComponent::GetOwnedIngredientIDs() const
{
	TArray<FName> Out;
	for (const auto& Pair : Inventory) { if (Pair.Value > 0) { Out.Add(Pair.Key); } }
	Out.Sort([](FName A, FName B) { return A.LexicalLess(B); });
	return Out;
}

int32 UTeaCraftingComponent::GetIngredientCount(FName ID) const
{
	const int32* Cur = Inventory.Find(ID);
	return Cur ? *Cur : 0;
}

bool UTeaCraftingComponent::MatchRecipe(const TArray<FName>& Selection, FName& OutRecipeID) const
{
	if (!RecipeTable) { return false; }
	for (const FName& RowName : RecipeTable->GetRowNames())
	{
		const FRecipeData* Row = RecipeTable->FindRow<FRecipeData>(RowName, TEXT("MatchRecipe"));
		if (!Row) { continue; }
		if (AreIngredientListsEqual(Selection, Row->RequiredIngredients))
		{
			OutRecipeID = RowName;
			return true;
		}
	}
	return false;
}

bool UTeaCraftingComponent::AreIngredientListsEqual(const TArray<FName>& A, const TArray<FName>& B)
{
	if (A.Num() != B.Num()) { return false; }
	// 멀티셋 비교 (순서 무관, 중복 허용)
	TMap<FName, int32> CA, CB;
	for (const FName& N : A) { CA.FindOrAdd(N)++; }
	for (const FName& N : B) { CB.FindOrAdd(N)++; }
	if (CA.Num() != CB.Num()) { return false; }
	for (const auto& Pair : CA)
	{
		const int32* InB = CB.Find(Pair.Key);
		if (!InB || *InB != Pair.Value) { return false; }
	}
	return true;
}
