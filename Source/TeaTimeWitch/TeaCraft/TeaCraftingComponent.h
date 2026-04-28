#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TeaCraftTypes.h"
#include "TeaCraftingComponent.generated.h"

class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectionChanged);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBrewComplete, const FBrewResult&, Result);

UCLASS(ClassGroup=(TeaCraft), meta=(BlueprintSpawnableComponent))
class TEATIMEWITCH_API UTeaCraftingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTeaCraftingComponent();

	static const int32 MaxSelectionSlots = 3;

	UPROPERTY(EditAnywhere, Category = "TeaCraft|Data")
	TObjectPtr<UDataTable> IngredientTable;
	UPROPERTY(EditAnywhere, Category = "TeaCraft|Data")
	TObjectPtr<UDataTable> RecipeTable;

	UPROPERTY(EditAnywhere, Category = "TeaCraft|Debug")
	TArray<FName> StartingIngredients;

	UFUNCTION(BlueprintCallable)
	void AddIngredient(FName ID, int32 Count = 1);
	UFUNCTION(BlueprintCallable)
	bool RemoveIngredient(FName ID, int32 Count = 1);

	UFUNCTION(BlueprintCallable)
	bool AddToSelection(FName ID);
	UFUNCTION(BlueprintCallable)
	void RemoveFromSelection(int32 SlotIndex);
	UFUNCTION(BlueprintCallable)
	void ClearSelection();

	UFUNCTION(BlueprintCallable)
	FBrewResult TryBrew();

	UFUNCTION(BlueprintPure)
	bool GetIngredientData(FName ID, FIngredientData& OutData) const;
	UFUNCTION(BlueprintPure)
	TArray<FName> GetOwnedIngredientIDs() const;
	UFUNCTION(BlueprintPure)
	const TArray<FName>& GetCurrentSelection() const { return CurrentSelection; }

	UFUNCTION(BlueprintPure)
	int32 GetIngredientCount(FName ID) const;

	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnInventoryChanged;
	UPROPERTY(BlueprintAssignable)
	FOnSelectionChanged OnSelectionChanged;
	UPROPERTY(BlueprintAssignable)
	FOnBrewComplete OnBrewComplete;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TMap<FName, int32> Inventory;
	UPROPERTY(VisibleAnywhere)
	TArray<FName> CurrentSelection;

private:
	bool MatchRecipe(const TArray<FName>& Selection, FName& OutRecipeID) const;
	static bool AreIngredientListsEqual(const TArray<FName>& A, const TArray<FName>& B);
};
