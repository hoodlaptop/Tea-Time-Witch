#include "Core/TrenchDungeonGameMode.h"

#include "DungeonProgressSubsystem.h"
#include "Kismet/GameplayStatics.h"

ATrenchDungeonGameMode::ATrenchDungeonGameMode()
{
}

void ATrenchDungeonGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;
	UDungeonProgressSubsystem* DP = GI->GetSubsystem<UDungeonProgressSubsystem>();
	if (!DP) return;
	
	DP->EnterDungeon(DungeonID);
	
	DP->OnEncounterCleared.AddDynamic(this, &ATrenchDungeonGameMode::HandleEncounterCleared);
	DP->OnDungeonCleared.AddDynamic(this, &ATrenchDungeonGameMode::HandleDungeonCleared);
}

void ATrenchDungeonGameMode::HandleEncounterCleared(FName ClearedEncounterID)
{
	UE_LOG(LogTemp, Log, TEXT("[Trench] Encounter cleared : %s"), *ClearedEncounterID.ToString());
}

void ATrenchDungeonGameMode::HandleDungeonCleared(FName ClearedDungeonID)
{
	UE_LOG(LogTemp, Log, TEXT("[Trench] Dungeon cleared : %s - return to TeaShop pending"), 
		*ClearedDungeonID.ToString());
	
	// UGameplayStatics::OpenLevel(this, TEXT("TeaShop_Whitebox"));
}
