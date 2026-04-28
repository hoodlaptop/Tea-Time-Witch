#include "Core/TeaTimeWitchPlayerController.h"
#include "Blueprint/UserWidget.h"

ATeaTimeWitchPlayerController::ATeaTimeWitchPlayerController()
{
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
}

void ATeaTimeWitchPlayerController::SetTeaShopInputState(ETeaShopInputState NewState, UUserWidget* FocusWidget)
{
	CurrentState = NewState;

	switch (NewState)
	{
	case ETeaShopInputState::Game:
		{
			FInputModeGameOnly Mode;
			SetInputMode(Mode);
			SetShowMouseCursor(false);
			if (APawn* P = GetPawn()) { EnableInput(this); }
			break;
		}
	case ETeaShopInputState::Dialogue:
		{
			FInputModeGameAndUI Mode;
			if (FocusWidget) { Mode.SetWidgetToFocus(FocusWidget->TakeWidget()); }
			Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(Mode);
			SetShowMouseCursor(true);
			if (APawn* P = GetPawn()) { DisableInput(this); } // 캐릭터 이동 차단
			break;
		}
	case ETeaShopInputState::Crafting:
		{
			FInputModeUIOnly Mode;
			if (FocusWidget) { Mode.SetWidgetToFocus(FocusWidget->TakeWidget()); }
			SetInputMode(Mode);
			SetShowMouseCursor(true);
			if (APawn* P = GetPawn()) { DisableInput(this); }
			break;
		}
	}
}
