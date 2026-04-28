#include "BattleGameMode.h"

#include "../Core/HD2DCameraPawn.h"
#include "GameFramework/PlayerController.h"

ABattleGameMode::ABattleGameMode()
{
	// 전투 중엔 입력 없는 카메라만 — B2의 HD2DCameraPawn 재활용
	DefaultPawnClass = AHD2DCameraPawn::StaticClass();
	PlayerControllerClass = APlayerController::StaticClass();
}