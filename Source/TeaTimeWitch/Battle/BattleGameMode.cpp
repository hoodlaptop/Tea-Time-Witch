#include "BattleGameMode.h"

#include "BattleManager.h"
#include "TurnManager.h"
#include "../Core/DungeonProgressSubsystem.h"
#include "../Core/HD2DCameraPawn.h"
#include "../Core/TeaTimeWitchPlayerController.h"
#include "../UI/BattleHUDWidget.h"
#include "Engine/GameInstance.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ABattleGameMode::ABattleGameMode()
{
	DefaultPawnClass = AHD2DCameraPawn::StaticClass();
}

void ABattleGameMode::BeginPlay()
{
	Super::BeginPlay(); // base가 HUDWidget 생성

	// BattleManager의 BeginPlay → TurnManager Spawn 후에 접근하기 위해 다음 Tick 지연
	GetWorldTimerManager().SetTimerForNextTick(this, &ABattleGameMode::SetupBattleHUD);
}

void ABattleGameMode::SetupBattleHUD()
{
	BattleHUD = Cast<UBattleHUDWidget>(HUDWidget);
	if (!BattleHUD)
	{
		UE_LOG(LogTemp, Error,
		       TEXT("[BattleGameMode] HUDWidgetClass가 UBattleHUDWidget(WBP_BattleHUD)으로 설정되지 않음"));
		return;
	}

	for (TActorIterator<ABattleManager> It(GetWorld()); It; ++It)
	{
		BattleManagerRef = *It;
		break;
	}
	if (!BattleManagerRef)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleGameMode] BattleManager가 레벨에 없음"));
		return;
	}

	ATurnManager* TurnMgr = BattleManagerRef->GetTurnManager();
	if (!TurnMgr)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleGameMode] TurnManager가 아직 생성되지 않음"));
		return;
	}

	BattleHUD->InitWithBattle(BattleManagerRef, TurnMgr);
	BattleHUD->OnBattleResultConfirmed.AddDynamic(this, &ABattleGameMode::HandleBattleResultConfirmed);

	if (ATeaTimeWitchPlayerController* PC = Cast<ATeaTimeWitchPlayerController>(
		UGameplayStatics::GetPlayerController(this, 0)))
	{
		PC->SetTeaShopInputState(ETeaShopInputState::Battle, BattleHUD);
	}
}

void ABattleGameMode::HandleBattleResultConfirmed(bool bWon)
{
	UE_LOG(LogTemp, Log, TEXT("[BattleGameMode] Battle ended, won=%d. Calling DP->OnBattleResult."),
	       bWon ? 1 : 0);

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UDungeonProgressSubsystem* DP = GI->GetSubsystem<UDungeonProgressSubsystem>())
		{
			DP->OnBattleResult(bWon);
			// DP가 OpenLevel(ReturnDungeonLevel)로 던전 복귀
		}
	}
}
