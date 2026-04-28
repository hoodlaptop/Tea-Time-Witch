#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "../Dialogue/TTWDialogueTypes.h"
#include "DialogueWidget.generated.h"

class UDialogueSystem;
class UTextBlock;
class UButton;
class ANPCBase;

UCLASS(Abstract, BlueprintType)
class TEATIMEWITCH_API UDialogueWidget : public UCommonUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> SpeakerNameText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> DialogueText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ContinueButton;

	UPROPERTY()
	TObjectPtr<UDialogueSystem> DialogueSubsystem;

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION()
	void HandleLineChanged(const FDialogueLine& Line);

	UFUNCTION()
	void HandleStarted(ANPCBase* Speaker);

	UFUNCTION()
	void HandleEnded();

	UFUNCTION()
	void OnContinueClicked();
};
