#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TTWDialogueTypes.generated.h"

class UTexture2D;

UENUM(BlueprintType)
enum class EDialogueAction : uint8
{
	None,
	JumpTo,
	OpenTeaCraft,
	StartBattle,
	EndDialogue,
	EnterDungeon
};

USTRUCT(BlueprintType)
struct FDialogueLine : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName LineID;

	UPROPERTY(EditAnywhere)
	FText SpeakerName;

	UPROPERTY(EditAnywhere, meta=(MultiLine=true))
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> PortraitTexture;

	UPROPERTY(EditAnywhere)
	FName NextLineID;

	UPROPERTY(EditAnywhere)
	EDialogueAction Action = EDialogueAction::None;

	UPROPERTY(EditAnywhere)
	FName ActionParam;

	UPROPERTY(EditAnywhere)
	FName OnSuccessJumpTo;

	UPROPERTY(EditAnywhere)
	FName OnFailJumpTo;
};
