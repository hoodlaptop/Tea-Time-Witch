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
	if (EncounterID.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Dungeon] RequestBattleTransition failed: EncounterID is None"));
		return;
	}

	if (EnemyIDs.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Dungeon] RequestBattleTransition failed: EnemyIDs is empty"));
		return;
	}

	if (ReturnLevel.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Dungeon] RequestBattleTransition failed: ReturnLevel is None"));
		return;
	}
	
	PendingBattleEncounterID = EncounterID;
	PendingEnemyIDs = EnemyIDs;
	PlayerReturnLocation = ReturnLoc;
	ReturnDungeonLevel = ReturnLevel;
	bPendingIsFinalEncounter = bIsFinalEncounter;
	
	UE_LOG(LogTemp, Log, TEXT("[Dungeon] BattleTransition - Encounter=%s, EnemyCount=%d, ReturnLevel=%s, Final=%d"),
		*EncounterID.ToString(), EnemyIDs.Num(), *ReturnLevel.ToString(), bIsFinalEncounter ? 1 : 0);
	
	bIsReturningFromBattle = false;
	UGameplayStatics::OpenLevel(this, TEXT("BattleLevel"));
}

void UDungeonProgressSubsystem::OnBattleResult(bool bWon)
{
	if (PendingBattleEncounterID.IsNone()) return;

	bool bWasFinal = false;
	if (bWon)
	{
		bWasFinal = bPendingIsFinalEncounter;
		MarkEncounterCleared(PendingBattleEncounterID);
	}

	if (bWasFinal)
	{
		PendingBattleEncounterID = NAME_None;
		PendingEnemyIDs.Reset();
		PlayerReturnLocation = FVector::ZeroVector;
		ReturnDungeonLevel = NAME_None;
		bPendingIsFinalEncounter = false;
		bIsReturningFromBattle = false;

		ClearDungeon();
		return;
	}

	if (ReturnDungeonLevel.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Dungeon] OnBattleResult: ReturnDungeonLevel is None - cannot return"));
		return;
	}
	
	const FName LevelToReturn = ReturnDungeonLevel;
	
	PendingBattleEncounterID = NAME_None;
	PendingEnemyIDs.Reset();
	ReturnDungeonLevel = NAME_None;
	bPendingIsFinalEncounter = false;

	bIsReturningFromBattle = true;
	UGameplayStatics::OpenLevel(this, LevelToReturn);
}

void UDungeonProgressSubsystem::ClearDungeon()
{
	if (bDungeonCleared) return;

	bDungeonCleared = true;
	LastClearedDungeonID = CurrentDungeonID;
	OnDungeonCleared.Broadcast(CurrentDungeonID);

	if (UWorld* World = GetWorld())
	{
		FTimerHandle TempHandle;
		World->GetTimerManager().SetTimer(
			TempHandle,
			FTimerDelegate::CreateUObject(this, &UDungeonProgressSubsystem::ExitDungeon,
			                              FName(TEXT("TeaShop_Whitebox"))),
			2.0f,
			false);
	}
}

void UDungeonProgressSubsystem::ExitDungeon(FName ReturnLevel)
{
	bIsReturningFromDungeon = true;
	CurrentDungeonID = NAME_None;
	ClearedEncounters.Reset();
	bDungeonCleared = false;

	PendingBattleEncounterID = NAME_None;
	PendingEnemyIDs.Reset();
	PlayerReturnLocation = FVector::ZeroVector;
	ReturnDungeonLevel = NAME_None;
	bPendingIsFinalEncounter = false;
	bIsReturningFromBattle = false;

	if (ReturnLevel.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Dungeon] ExitDungeon: ReturnLevel is None"));
		return;
	}

	UGameplayStatics::OpenLevel(this, ReturnLevel);
}
