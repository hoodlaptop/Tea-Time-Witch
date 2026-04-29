#include "Core/DungeonProgressSubsystem.h"

#include "Kismet/GameplayStatics.h"

void UDungeonProgressSubsystem::EnterDungeon(FName DungeonID)
{
	if (CurrentDungeonID == DungeonID) return;

	CurrentDungeonID = DungeonID;
	ClearedEncounters.Reset();
	bDungeonCleared = false;
}

void UDungeonProgressSubsystem::MarkEncounterCleared(FName EncounterID)
{
	if (EncounterID.IsNone()) return;
	if (ClearedEncounters.Contains(EncounterID)) return;

	ClearedEncounters.Add(EncounterID);
	OnEncounterCleared.Broadcast(EncounterID);
}

bool UDungeonProgressSubsystem::IsEncounterCleared(FName EncounterID) const
{
	return ClearedEncounters.Contains(EncounterID);
}

void UDungeonProgressSubsystem::RequestBattleTransition(FName EncounterID, const TArray<FName>& EnemyIDs,
	FVector ReturnLoc, FName ReturnLevel, bool bIsFinalEncounter)
{
	PendingBattleEncounterID = EncounterID;
	PendingEnemyIDs = EnemyIDs;
	PlayerReturnLocation = ReturnLoc;
	ReturnDungeonLevel = ReturnLevel;
	bPendingIsFinalEncounter = bIsFinalEncounter;
	
	UE_LOG(LogTemp, Log, 
		TEXT("[Dungeon] BattleTransition stub - Encounter = %s, EnemyCounter = %d, ReturnLevel = %s, Final = %d"),
		*EncounterID.ToString(), EnemyIDs.Num(), *ReturnLevel.ToString(), bIsFinalEncounter ? 1 : 0);
	
	// OpenLevel
	// UGameplayStatics::OpenLevel(this, TEXT("BattleLevel"));
}

void UDungeonProgressSubsystem::OnBattleResult(bool bWon)
{
	if (PendingBattleEncounterID.IsNone()) return;
	
	if (bWon)
	{
		const bool bWasFinal = bPendingIsFinalEncounter;
		MarkEncounterCleared(PendingBattleEncounterID);
		if (bWasFinal) { ClearDungeon();}
	}
	
	/*if (!ReturnDungeonLevel.IsNone())
	{
		UGameplayStatics::OpenLevel(this, ReturnDungeonLevel);
	}*/
	
	// 페이지로드 정리
	PendingBattleEncounterID = NAME_None;
	PendingEnemyIDs.Reset();
	bPendingIsFinalEncounter = false;
}

void UDungeonProgressSubsystem::ClearDungeon()
{
	if (bDungeonCleared) return;
	
	bDungeonCleared = true;
	OnDungeonCleared.Broadcast(CurrentDungeonID);
}
