#include "TTWGameModeBase.h"

#include "CommonUserWidget.h"
#include "TeaTimeWitchPlayerController.h"
#include "Characters/EliaCharacter.h"
#include "Kismet/GameplayStatics.h"

ATTWGameModeBase::ATTWGameModeBase()
{
	DefaultPawnClass = AEliaCharacter::StaticClass();
	PlayerControllerClass = ATeaTimeWitchPlayerController::StaticClass();
}

void ATTWGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC || !HUDWidgetClass) return;
	
	HUDWidget = CreateWidget<UCommonUserWidget>(PC, HUDWidgetClass);
	if (HUDWidget)
	{
		HUDWidget->AddToViewport(0);
	}
}
