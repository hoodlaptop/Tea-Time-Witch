#include "DialogueSystem.h"

bool UDialogueSystem::StartConversation(FName StartLineID, ANPCBase* Speaker, UDataTable* Table)
{
	if (bIsActive || !Table) return false;
	CurrentTable = Table;
	CurrentSpeaker = Speaker;

	FDialogueLine Line;
	if (!LoadLine(StartLineID, Line))
	{
		CurrentTable = nullptr;
		CurrentSpeaker = nullptr;
		return false;
	}

	bIsActive = true;
	CurrentLineID = StartLineID;
	CurrentLine = Line;

	OnStarted.Broadcast(Speaker);
	OnLineChanged.Broadcast(CurrentLine);
	return true;
}

void UDialogueSystem::AdvanceLine()
{
	if (!bIsActive || bWaitingForActionResult) return;
	ProcessCurrentLineAction();
}

void UDialogueSystem::JumpToLine(FName LineID)
{
	if (LineID.IsNone())
	{
		EndConversation();
		return;
	}

	FDialogueLine Line;
	if (!LoadLine(LineID, Line))
	{
		EndConversation();
		return;
	}

	CurrentLineID = LineID;
	CurrentLine = Line;
	OnLineChanged.Broadcast(CurrentLine);
}

void UDialogueSystem::EndConversation()
{
	if (!bIsActive) return;
	bIsActive = false;
	bWaitingForActionResult = false;
	CurrentLineID = NAME_None;
	CurrentLine = FDialogueLine();
	CurrentSpeaker = nullptr;
	CurrentTable = nullptr;
	OnEnded.Broadcast();
}

void UDialogueSystem::NotifyActionResult(bool bSuccess)
{
	if (!bIsActive || !bWaitingForActionResult) return;
	bWaitingForActionResult = false;

	const FName Target = bSuccess ? CurrentLine.OnSuccessJumpTo : CurrentLine.OnFailJumpTo;
	if (!Target.IsNone())
	{
		JumpToLine(Target);
	}
	else if (!CurrentLine.NextLineID.IsNone())
	{
		JumpToLine(CurrentLine.NextLineID);
	}
	else
	{
		EndConversation();
	}
}

void UDialogueSystem::ProcessCurrentLineAction()
{
	switch (CurrentLine.Action)
	{
	case EDialogueAction::None:
		if (CurrentLine.NextLineID.IsNone()) { EndConversation(); return; }
		JumpToLine(CurrentLine.NextLineID);
		return;

	case EDialogueAction::JumpTo:
		JumpToLine(CurrentLine.ActionParam);
		return;

	case EDialogueAction::OpenTeaCraft:
	case EDialogueAction::StartBattle:
		bWaitingForActionResult = true;
		OnAction.Broadcast(CurrentLine.Action, CurrentLine.ActionParam);
		return;

	case EDialogueAction::EndDialogue:
		EndConversation();
		return;
	}
}

bool UDialogueSystem::LoadLine(FName LineID, FDialogueLine& OutLine) const
{
	if (!CurrentTable) return false;
	
	const FDialogueLine* Row = CurrentTable->FindRow<FDialogueLine>(LineID, TEXT("DialogueSystem"));
	if (!Row)
	{
		UE_LOG(LogTemp, Error, TEXT("DialogueSystem: missing line %s"), *LineID.ToString());
		return false;
	}
	OutLine = *Row;
	return true;
}
