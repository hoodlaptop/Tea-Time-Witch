#pragma once

#include "CoreMinimal.h"
#include "Core/TTWGameModeBase.h"
#include "TrenchDungeonGameMode.generated.h"

UCLASS()
class TEATIMEWITCH_API ATrenchDungeonGameMode : public ATTWGameModeBase
{
	GENERATED_BODY()
public:
	ATrenchDungeonGameMode();
	
	UPROPERTY(EditDefaultsOnly, Category="Dungeon")
	FName DungeonID = TEXT("Trench");
	
	UPROPERTY(EditDefaultsOnly, Category="Dungeon|Debug")
	bool bShowDungeonDebug = true;
	
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	
	virtual void PostLogin(class APlayerController* NewPlayer) override;
	
	UFUNCTION()
	void HandleEncounterCleared(FName ClearedEncounterID);
	
	UFUNCTION()
	void HandleDungeonCleared(FName ClearedDungeonID);
};
