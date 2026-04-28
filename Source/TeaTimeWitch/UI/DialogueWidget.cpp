#include "DialogueWidget.h"

#include "DialogueSystem.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetIsFocusable(true);

	if (UWorld* World = GetWorld())
	{
		DialogueSubsystem = World->GetSubsystem<UDialogueSystem>();
	}

	if (DialogueSubsystem)
	{
		DialogueSubsystem->OnLineChanged.AddDynamic(this, &UDialogueWidget::HandleLineChanged);
		DialogueSubsystem->OnStarted.AddDynamic(this, &UDialogueWidget::HandleStarted);
		DialogueSubsystem->OnEnded.AddDynamic(this, &UDialogueWidget::HandleEnded);

		if (DialogueSubsystem->bIsActive)
		{
			HandleLineChanged(DialogueSubsystem->CurrentLine);
		}
	}

	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddDynamic(this, &UDialogueWidget::OnContinueClicked);
	}

	SetKeyboardFocus();
}

void UDialogueWidget::NativeDestruct()
{
	if (DialogueSubsystem)
	{
		DialogueSubsystem->OnLineChanged.RemoveDynamic(this, &UDialogueWidget::HandleLineChanged);
		DialogueSubsystem->OnStarted.RemoveDynamic(this, &UDialogueWidget::HandleStarted);
		DialogueSubsystem->OnEnded.RemoveDynamic(this, &UDialogueWidget::HandleEnded);
	}
	
	if (ContinueButton)
	{
		ContinueButton->OnClicked.RemoveDynamic(this, &UDialogueWidget::OnContinueClicked);
	}
	
	Super::NativeDestruct();
}

FReply UDialogueWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (DialogueSubsystem && DialogueSubsystem->bIsActive)
	{
		const FKey Key = InKeyEvent.GetKey();
		if (Key == EKeys::SpaceBar || Key == EKeys::Enter || Key == EKeys::E)
		{
			DialogueSubsystem->AdvanceLine();
			return FReply::Handled(); 
		}
	}
	
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UDialogueWidget::HandleLineChanged(const FDialogueLine& Line)
{
	if (SpeakerNameText)
	{
		SpeakerNameText->SetText(Line.SpeakerName);
	}
	if (DialogueText)
	{
		DialogueText->SetText(Line.Text);
	}
	
	if (ContinueButton)
	{
		ContinueButton->SetIsEnabled(true);
	}
}

void UDialogueWidget::HandleStarted(ANPCBase* /*Speaker*/)
{
	// 필요한가?
}

void UDialogueWidget::HandleEnded()
{
	RemoveFromParent();
}

void UDialogueWidget::OnContinueClicked()
{
	if (DialogueSubsystem)
	{
		DialogueSubsystem->AdvanceLine();
	}
}
