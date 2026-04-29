#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleManager.generated.h"

class ABattleCharacter;
class ATurnManager;
class UDataTable;

UCLASS()
class TEATIMEWITCH_API ABattleManager : public AActor
{
	GENERATED_BODY()

public:
	ABattleManager();

	virtual void BeginPlay() override;

	const TArray<TObjectPtr<ABattleCharacter>>& GetAllies() const { return Allies; }
	const TArray<TObjectPtr<ABattleCharacter>>& GetEnemies() const { return Enemies; }
	ATurnManager* GetTurnManager() const { return TurnManager; }

protected:
	void SpawnCharacters();

	UPROPERTY(EditAnywhere, Category = "Battle|Setup")
	TSubclassOf<ABattleCharacter> AllyClass;

	UPROPERTY(EditAnywhere, Category = "Battle|Setup")
	TSubclassOf<ABattleCharacter> EnemyClass;

	UPROPERTY(EditAnywhere, Category = "Battle|Setup")
	TArray<FName> AllyIDs;

	UPROPERTY(EditAnywhere, Category = "Battle|Setup")
	TArray<FName> EnemyIDs;

	UPROPERTY(EditAnywhere, Category = "Battle|Data")
	TObjectPtr<UDataTable> StatDataTable;

	UPROPERTY(EditAnywhere, Category = "Battle|Data")
	TObjectPtr<UDataTable> SkillDataTable;

	UPROPERTY(EditAnywhere, Category = "Battle|Spawn")
	TArray<TObjectPtr<AActor>> AllySpawnPoints;

	UPROPERTY(EditAnywhere, Category = "Battle|Spawn")
	TArray<TObjectPtr<AActor>> EnemySpawnPoints;

	UPROPERTY(EditAnywhere, Category = "Battle|TurnManager")
	TSubclassOf<ATurnManager> TurnManagerClass;

	UPROPERTY()
	TArray<TObjectPtr<ABattleCharacter>> Allies;
	UPROPERTY()
	TArray<TObjectPtr<ABattleCharacter>> Enemies;
	UPROPERTY()
	TObjectPtr<ATurnManager> TurnManager;
};
