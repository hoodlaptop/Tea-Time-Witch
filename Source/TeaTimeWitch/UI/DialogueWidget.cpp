#include "DialogueWidget.h"

#include "DialogueSystem.h"
#include "Components/Button.h"
#include "Components/Image.h"
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
	// 이름과 대사 업데이트
	if (SpeakerNameText)
	{
		SpeakerNameText->SetText(Line.SpeakerName);
	}
	if (DialogueText)
	{
		DialogueText->SetText(Line.Text);
	}

	// 초상화 업데이트
	if (SpeakerPortrait)
	{
		// 데이터 테이블(구조체)에 할당된 텍스처가 있는지 확인
		if (Line.PortraitTexture) 
		{
			SpeakerPortrait->SetBrushFromTexture(Line.PortraitTexture);
			SpeakerPortrait->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			// 할당된 텍스처가 없다면 초상화 숨김 (내레이션 처리)
			SpeakerPortrait->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (ContinueButton)
	{
		ContinueButton->SetIsEnabled(true);
	}
}

void UDialogueWidget::HandleStarted(ANPCBase* /*Speaker*/)
{
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
