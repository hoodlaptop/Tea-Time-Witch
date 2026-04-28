#include "TeaTimeWitchGameMode.h"

#include "TeaTimeWitchPlayerController.h"
#include "../Characters/EliaCharacter.h"

ATeaTimeWitchGameMode::ATeaTimeWitchGameMode()
{
	DefaultPawnClass = AEliaCharacter::StaticClass();
	PlayerControllerClass = ATeaTimeWitchPlayerController::StaticClass();
}
