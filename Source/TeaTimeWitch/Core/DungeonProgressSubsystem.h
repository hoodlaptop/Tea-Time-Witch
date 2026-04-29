#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DungeonProgressSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEncounterCleared, FName, EncounterID);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDungeonCleared, FName, DungeonID);

UCLASS()
class TEATIMEWITCH_API UDungeonProgressSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category="Dungeon")
	FName CurrentDungeonID;

	UPROPERTY(BlueprintReadOnly, Category="Dungeon")
	TSet<FName> ClearedEncounters;

	UPROPERTY(BlueprintReadOnly, Category="Dungeon")
	bool bDungeonCleared = false;

	// --- 전투 전환 페이로드(씬 전환 후에도 유지) ---
	UPROPERTY(BlueprintReadOnly, Category="Dungeon|Battle")
	FName PendingBattleEncounterID;

	UPROPERTY(BlueprintReadOnly, Category="Dungeon|Battle")
	TArray<FName> PendingEnemyIDs;

	UPROPERTY(BlueprintReadOnly, Category="Dungeon|Battle")
	FVector PlayerReturnLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category="Dungeon|Battle")
	FName ReturnDungeonLevel;

	UPROPERTY(BlueprintReadOnly, Category="Dungeon|Battle")
	bool bPendingIsFinalEncounter = false;

	// --- DeBug ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Dungeon|Debug")
	bool bShowDungeonDebug = false;

	// --- API ---
	UFUNCTION(BlueprintCallable, Category="Dungeon")
	void EnterDungeon(FName DungeonID);

	UFUNCTION(BlueprintCallable, Category="Dungeon")
	void MarkEncounterCleared(FName EncounterID);

	UFUNCTION(BlueprintPure, Category="Dungeon")
	bool IsEncounterCleared(FName EncounterID) const;

	UFUNCTION(BlueprintCallable, Category="Dungeon")
	void RequestBattleTransition(
		FName EncounterID,
		const TArray<FName>& EnemyIDs,
		FVector ReturnLoc,
		FName ReturnLevel,
		bool bIsFinalEncounter);

	UFUNCTION(BlueprintCallable, Category="Dungeon")
	void OnBattleResult(bool bWon);

	UFUNCTION(BlueprintCallable, Category="Dungeon")
	void ClearDungeon();

	// --- Event --
	UPROPERTY(BlueprintAssignable, Category="Dungeon")
	FOnEncounterCleared OnEncounterCleared;

	UPROPERTY(BlueprintAssignable, Category="Dungeon")
	FOnDungeonCleared OnDungeonCleared;
};
