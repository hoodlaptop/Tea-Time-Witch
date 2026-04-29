#pragma once

#include "CoreMinimal.h"
#include "../Core/TTWGameModeBase.h"
#include "BattleGameMode.generated.h"

class UBattleHUDWidget;
class ABattleManager;

UCLASS()
class TEATIMEWITCH_API ABattleGameMode : public ATTWGameModeBase
{
	GENERATED_BODY()

public:
	ABattleGameMode();
	
protected:
	virtual void BeginPlay() override;
	
	void SetupBattleHUD();
	
	UFUNCTION()
	void HandleBattleResultConfirmed(bool bWon);
	
private:
	UPROPERTY()
	TObjectPtr<UBattleHUDWidget> BattleHUD;

	UPROPERTY()
	TObjectPtr<ABattleManager> BattleManagerRef;
};