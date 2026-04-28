#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../Battle/BattleTypes.h"
#include "TeaCraftTypes.generated.h"

UENUM(BlueprintType)
enum class EIngredientType : uint8
{
	Base,
	Herb,
	Spice
};

USTRUCT(BlueprintType)
struct FIngredientData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName IngredientID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIngredientType Type = EIngredientType::Herb;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEmotionType AromaType = EEmotionType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> Icon;
};

USTRUCT(BlueprintType)
struct FRecipeData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RecipeID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText TeaName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> RequiredIngredients;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ResultTeaID;
};

USTRUCT(BlueprintType)
struct FBrewResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bSuccess = false;
	UPROPERTY(BlueprintReadOnly)
	FName MatchedRecipeID;
	UPROPERTY(BlueprintReadOnly)
	FText TeaName;
	UPROPERTY(BlueprintReadOnly)
	FText ResultMessage;
};
