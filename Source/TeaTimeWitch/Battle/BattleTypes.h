#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BattleTypes.generated.h"

UENUM(BlueprintType)
enum class EBattleSide : uint8
{
	Ally,
	Enemy
};

UENUM(BlueprintType)
enum class EBattleState : uint8
{
	None,
	BattleStart,
	TurnStart,
	PlayerTurn,
	EnemyTurn,
	ActionExecuting,
	TurnEnd,
	BattleEnd_Win,
	BattleEnd_Lose
};

UENUM(BlueprintType)
enum class EEmotionType : uint8
{
	None,
	Anger,
	Sadness,
	Fear,
	Emptiness,
	Obsession,
	Regret
};

USTRUCT(BlueprintType)
struct FCharacterStatData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CharacterID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHP = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxMP = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Attack = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Defense = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Speed = 50;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEmotionType EmotionType = EEmotionType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEmotionType WeaknessAroma = EEmotionType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName DefaultSkillID; // 기본 스킬 1개만
};

USTRUCT(BlueprintType)
struct FSkillData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SkillID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BasePower = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MPCost = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEmotionType AromaType = EEmotionType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bTargetEnemy = true;
};
