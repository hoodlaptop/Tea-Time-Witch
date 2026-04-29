#include "UI/BattleHUDWidget.h"
#include "../Battle/BattleCharacter.h"
#include "../Battle/BattleManager.h"
#include "../Battle/SkillComponent.h"
#include "../Battle/StatComponent.h"
#include "../Battle/TurnManager.h"
#include "BattleCharacterWidget.h"
#include "SkillButtonWidget.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"

void UBattleHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(true);
	
	if (AttackButton) { AttackButton->OnClicked.AddDynamic(this, &UBattleHUDWidget::OnAttackClicked); }
	if (SkillButton) { SkillButton->OnClicked.AddDynamic(this, &UBattleHUDWidget::OnSkillClicked); }
	if (TargetCancelButton)
	{
		TargetCancelButton->OnClicked.AddDynamic(this, &UBattleHUDWidget::OnTargetCancelClicked);
	}
	if (ContinueButton) { ContinueButton->OnClicked.AddDynamic(this, &UBattleHUDWidget::OnContinueClicked); }

	HideAllPanels();
	if (ResultPanel) { ResultPanel->SetVisibility(ESlateVisibility::Collapsed); }
}

void UBattleHUDWidget::NativeDestruct()
{
	if (TurnManager)
	{
		TurnManager->OnPlayerActionRequested.RemoveDynamic(this, &UBattleHUDWidget::HandlePlayerActionRequested);
		TurnManager->OnActionExecuted.RemoveDynamic(this, &UBattleHUDWidget::HandleActionExecuted);
		TurnManager->OnBattleEnded.RemoveDynamic(this, &UBattleHUDWidget::HandleBattleEnded);
	}
	Super::NativeDestruct();
}

void UBattleHUDWidget::InitWithBattle(ABattleManager* InBattle, ATurnManager* InTurn)
{
	BattleManager = InBattle;
	TurnManager = InTurn;
	if (!BattleManager || !TurnManager || !CharacterWidgetClass) { return; }

	TurnManager->OnPlayerActionRequested.AddDynamic(this, &UBattleHUDWidget::HandlePlayerActionRequested);
	TurnManager->OnActionExecuted.AddDynamic(this, &UBattleHUDWidget::HandleActionExecuted);
	TurnManager->OnBattleEnded.AddDynamic(this, &UBattleHUDWidget::HandleBattleEnded);

	RebuildCharacterPanels();
	HideAllPanels();
}

void UBattleHUDWidget::RebuildCharacterPanels()
{
	if (!BattleManager || !CharacterWidgetClass) { return; }

	if (AllyContainer) { AllyContainer->ClearChildren(); }
	if (EnemyContainer) { EnemyContainer->ClearChildren(); }
	AllyWidgets.Reset();
	EnemyWidgets.Reset();

	for (const TObjectPtr<ABattleCharacter>& Ally : BattleManager->GetAllies())
	{
		if (!Ally) { continue; }
		UBattleCharacterWidget* W = CreateWidget<UBattleCharacterWidget>(this, CharacterWidgetClass);
		if (!W) { continue; }
		W->BindCharacter(Ally);
		if (AllyContainer) { AllyContainer->AddChild(W); }
		AllyWidgets.Add(W);
	}

	for (const TObjectPtr<ABattleCharacter>& Enemy : BattleManager->GetEnemies())
	{
		if (!Enemy) { continue; }
		UBattleCharacterWidget* W = CreateWidget<UBattleCharacterWidget>(this, CharacterWidgetClass);
		if (!W) { continue; }
		W->BindCharacter(Enemy);
		W->OnCharacterClicked.AddDynamic(this, &UBattleHUDWidget::OnEnemyClicked);
		if (EnemyContainer) { EnemyContainer->AddChild(W); }
		EnemyWidgets.Add(W);
	}
}

void UBattleHUDWidget::HandlePlayerActionRequested(ABattleCharacter* Actor)
{
	CurrentActor = Actor;
	SelectedSkillID = NAME_None;

	// 현재 액터 하이라이트
	for (UBattleCharacterWidget* W : AllyWidgets)
	{
		if (W) { W->SetHighlight(W->GetTargetCharacter() == Actor); }
	}

	ShowPanel(EBattlePanel::Command);
}

void UBattleHUDWidget::HandleActionExecuted(const FBattleActionResult& Result)
{
	HideAllPanels();
	ClearActorHighlights();

	if (Result.bMissed)
	{
		AppendBattleLog(TEXT("행동 실패..."), FLinearColor(0.7f, 0.7f, 0.7f));
		return;
	}

	const FString SourceName = Result.Source && Result.Source->GetStatComponent()
		                           ? Result.Source->GetStatComponent()->GetDisplayName().ToString()
		                           : TEXT("?");
	const FString TargetName = Result.Target && Result.Target->GetStatComponent()
		                           ? Result.Target->GetStatComponent()->GetDisplayName().ToString()
		                           : TEXT("?");

	FString Msg;
	FLinearColor Color = FLinearColor::White;
	if (Result.bWeakness)
	{
		Msg = FString::Printf(TEXT("%s의 공격! [WEAKNESS!] %s에게 %d 데미지!"),
		                      *SourceName, *TargetName, Result.Damage);
		Color = FLinearColor(1.f, 0.7f, 0.1f);
	}
	else
	{
		Msg = FString::Printf(TEXT("%s의 공격! %s에게 %d 데미지."),
		                      *SourceName, *TargetName, Result.Damage);
	}
	AppendBattleLog(Msg, Color);
}

void UBattleHUDWidget::HandleBattleEnded(bool bWon)
{
	bWonCached = bWon;
	HideAllPanels();
	ClearActorHighlights();
	ShowResultPanel(bWon);
}

void UBattleHUDWidget::ShowPanel(EBattlePanel Panel)
{
	if (PanelSwitcher)
	{
		PanelSwitcher->SetActiveWidgetIndex((int32)Panel);
		PanelSwitcher->SetVisibility(ESlateVisibility::Visible);
	}
	EnableEnemyTargets(Panel == EBattlePanel::Target);
}

void UBattleHUDWidget::HideAllPanels()
{
	if (PanelSwitcher) { PanelSwitcher->SetVisibility(ESlateVisibility::Collapsed); }
	EnableEnemyTargets(false);
}

void UBattleHUDWidget::ShowSkillPanel(ABattleCharacter* User)
{
	if (!SkillContainer || !User || !SkillButtonClass) { return; }

	SkillContainer->ClearChildren();

	USkillComponent* SC = User->GetSkillComponent();
	UStatComponent* Stats = User->GetStatComponent();
	if (!SC || !Stats) { return; }

	const TArray<FName>& IDs = SC->GetSkillIDs();
	const int32 CurMP = Stats->GetCurrentMP();

	for (const FName& ID : IDs)
	{
		const FSkillData* Data = SC->GetSkillData(ID);
		USkillButtonWidget* Btn = CreateWidget<USkillButtonWidget>(this, SkillButtonClass);
		if (!Btn) { continue; }

		const FText SkillName = Data ? Data->DisplayName : FText::FromName(ID);
		const int32 MP = Data ? Data->MPCost : 0;
		Btn->SetSkillData(ID, SkillName, MP, MP <= CurMP);
		Btn->OnClicked.AddDynamic(this, &UBattleHUDWidget::OnSkillSelected);
		SkillContainer->AddChild(Btn);
	}

	ShowPanel(EBattlePanel::Skill);
}

void UBattleHUDWidget::ShowTargetPanel(FName SkillID)
{
	SelectedSkillID = SkillID;
	ShowPanel(EBattlePanel::Target);
}

void UBattleHUDWidget::EnableEnemyTargets(bool bEnable)
{
	for (UBattleCharacterWidget* W : EnemyWidgets)
	{
		if (!W) { continue; }
		ABattleCharacter* C = W->GetTargetCharacter();
		const bool bAlive = C && C->CanAct();
		W->SetSelectable(bEnable && bAlive);
	}
}

void UBattleHUDWidget::ClearActorHighlights()
{
	for (UBattleCharacterWidget* W : AllyWidgets) { if (W) { W->SetHighlight(false); } }
	for (UBattleCharacterWidget* W : EnemyWidgets) { if (W) { W->SetHighlight(false); } }
}

void UBattleHUDWidget::ShowResultPanel(bool bWon)
{
	if (!ResultPanel) { return; }
	ResultPanel->SetVisibility(ESlateVisibility::Visible);
	if (ResultText)
	{
		ResultText->SetText(FText::FromString(bWon ? TEXT("VICTORY!") : TEXT("DEFEAT...")));
		ResultText->SetColorAndOpacity(bWon
			                               ? FSlateColor(FLinearColor(1.f, 0.85f, 0.2f))
			                               : FSlateColor(FLinearColor(0.7f, 0.2f, 0.2f)));
	}
}

void UBattleHUDWidget::AppendBattleLog(const FString& Message, FLinearColor Color)
{
	if (!BattleLogText) { return; }
	BattleLogText->SetText(FText::FromString(Message));
	BattleLogText->SetColorAndOpacity(FSlateColor(Color));
}

void UBattleHUDWidget::OnAttackClicked()
{
	if (!CurrentActor) { return; }
	UStatComponent* Stats = CurrentActor->GetStatComponent();
	if (!Stats) { return; }
	const FName Default = Stats->GetStatData().DefaultSkillID;
	if (Default.IsNone()) { return; }
	ShowTargetPanel(Default);
}

void UBattleHUDWidget::OnSkillClicked()
{
	if (!CurrentActor) { return; }
	ShowSkillPanel(CurrentActor);
}

void UBattleHUDWidget::OnSkillSelected(FName SkillID)
{
	ShowTargetPanel(SkillID);
}

void UBattleHUDWidget::OnTargetCancelClicked()
{
	ShowPanel(EBattlePanel::Command);
}

void UBattleHUDWidget::OnEnemyClicked(ABattleCharacter* Target)
{
	if (!TurnManager || !Target || SelectedSkillID.IsNone()) { return; }
	HideAllPanels();
	TurnManager->RequestPlayerAction(SelectedSkillID, Target);
}

void UBattleHUDWidget::OnContinueClicked()
{
	OnBattleResultConfirmed.Broadcast(bWonCached);
}
