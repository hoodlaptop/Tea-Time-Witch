#include "BattleManager.h"

#include "BattleCharacter.h"
#include "TurnManager.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"

ABattleManager::ABattleManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABattleManager::BeginPlay()
{
	Super::BeginPlay();

	if (!AllyClass || !EnemyClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] AllyClass/EnemyClass not set"));
		return;
	}
	if (!StatDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] StatDataTable not set"));
		return;
	}

	SpawnCharacters();

	if (Allies.Num() == 0 || Enemies.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleManager] Spawn failed: A=%d, E=%d"),
		       Allies.Num(), Enemies.Num());
		return;
	}

	const TSubclassOf<ATurnManager> TMClass =
		TurnManagerClass ? TurnManagerClass : TSubclassOf<ATurnManager>(ATurnManager::StaticClass());
	TurnManager = GetWorld()->SpawnActor<ATurnManager>(TMClass);
	if (TurnManager)
	{
		TurnManager->Initialize(this);
		TurnManager->StartBattle();
	}
}

void ABattleManager::SpawnCharacters()
{
	UWorld* World = GetWorld();
	if (!World) { return; }

	auto SpawnGroup = [&](
		const TArray<FName>& IDs,
		const TArray<TObjectPtr<AActor>>& SpawnPoints,
		EBattleSide SideIn,
		TSubclassOf<ABattleCharacter> ClassIn,
		TArray<TObjectPtr<ABattleCharacter>>& Out)
	{
		for (int32 i = 0; i < IDs.Num(); ++i)
		{
			FTransform Xform = FTransform::Identity;
			if (SpawnPoints.IsValidIndex(i) && SpawnPoints[i])
			{
				Xform = SpawnPoints[i]->GetActorTransform();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[BattleManager] Missing spawn point for index %d (using origin)"), i);
			}

			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			ABattleCharacter* C = World->SpawnActor<ABattleCharacter>(ClassIn, Xform, Params);
			if (C)
			{
				C->InitializeBattle(IDs[i], SideIn, StatDataTable, SkillDataTable);
				Out.Add(C);
			}
		}
	};

	SpawnGroup(AllyIDs, AllySpawnPoints, EBattleSide::Ally, AllyClass, Allies);
	SpawnGroup(EnemyIDs, EnemySpawnPoints, EBattleSide::Enemy, EnemyClass, Enemies);

	UE_LOG(LogTemp, Verbose, TEXT("[BattleManager] Spawned %d allies, %d enemies"),
	       Allies.Num(), Enemies.Num());
}
