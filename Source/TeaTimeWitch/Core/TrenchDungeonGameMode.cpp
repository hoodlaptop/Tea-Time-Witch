#include "Core/TrenchDungeonGameMode.h"

#include "DungeonProgressSubsystem.h"
#include "TeaTimeWitchPlayerController.h"
#include "Kismet/GameplayStatics.h"

ATrenchDungeonGameMode::ATrenchDungeonGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATrenchDungeonGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!GEngine) { return; }

	UGameInstance* GI = GetGameInstance();
	if (!GI) { return; }
	UDungeonProgressSubsystem* DP = GI->GetSubsystem<UDungeonProgressSubsystem>();
	if (!DP || !DP->bShowDungeonDebug) { return; }

	const FString Msg = FString::Printf(
		TEXT("Dungeon: %s | Cleared: %d | DungeonCleared: %s | Pending: %s"),
		*DP->CurrentDungeonID.ToString(),
		DP->ClearedEncounters.Num(),
		DP->bDungeonCleared ? TEXT("YES") : TEXT("no"),
		DP->PendingBattleEncounterID.IsNone()
			? TEXT("(none)")
			: *DP->PendingBattleEncounterID.ToString());

	GEngine->AddOnScreenDebugMessage(9001, 0.5f, FColor::Cyan, Msg);
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

	DP->bShowDungeonDebug = bShowDungeonDebug;
}

void ATrenchDungeonGameMode::PostLogin(class APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (auto* TPC = Cast<ATeaTimeWitchPlayerController>(NewPlayer))
	{
		TPC->SetTeaShopInputState(ETeaShopInputState::Game, nullptr);
	}
}

void ATrenchDungeonGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
	
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;
	UDungeonProgressSubsystem* DP = GI->GetSubsystem<UDungeonProgressSubsystem>();
	if (!DP) return;
	
	// 전투에서 복귀 중이고, 유효한 복귀 위치가 있는 경우에만 텔레포트
	if (DP->bIsReturningFromBattle && !DP->PlayerReturnLocation.IsNearlyZero())
	{
		if (APawn* Pawn = NewPlayer->GetPawn())
		{
			// TeleportPhysics를 사용하여 물리 엔진의 간섭 없이 정확한 위치로 이동
			Pawn->SetActorLocation(
				DP->PlayerReturnLocation,
				false,
				nullptr,
				ETeleportType::TeleportPhysics);
			UE_LOG(LogTemp, Log, TEXT("[Trench] Returned from battle, teleported to %s"),
				   *DP->PlayerReturnLocation.ToString());
		}
		// 처리가 완료되었으므로 플래그 초기화
		DP->bIsReturningFromBattle = false;
	}
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
