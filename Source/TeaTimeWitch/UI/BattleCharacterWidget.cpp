#include "BattleCharacterWidget.h"

#include "../Battle/BattleCharacter.h"
#include "../Battle/StatComponent.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UBattleCharacterWidget::BindCharacter(ABattleCharacter* InCharacter)
{
	if (TargetCharacter)
	{
		if (UStatComponent* PrevStat = TargetCharacter->GetStatComponent())
		{
			PrevStat->OnHPChanged.RemoveDynamic(this, &UBattleCharacterWidget::HandleHPChanged);
			PrevStat->OnDied.RemoveDynamic(this, &UBattleCharacterWidget::HandleDied);
		}
	}

	TargetCharacter = InCharacter;
	if (!TargetCharacter) { return; }

	if (UStatComponent* Stat = TargetCharacter->GetStatComponent())
	{
		Stat->OnHPChanged.AddDynamic(this, &UBattleCharacterWidget::HandleHPChanged);
		Stat->OnDied.AddDynamic(this, &UBattleCharacterWidget::HandleDied);
	}

	if (SelectButton && !SelectButton->OnClicked.IsAlreadyBound(this, &UBattleCharacterWidget::OnSelectButtonClicked))
	{
		SelectButton->OnClicked.AddDynamic(this, &UBattleCharacterWidget::OnSelectButtonClicked);
	}

	Refresh();
	SetHighlight(false);
	SetSelectable(false);
}

void UBattleCharacterWidget::NativeDestruct()
{
	if (TargetCharacter)
	{
		if (UStatComponent* Stat = TargetCharacter->GetStatComponent())
		{
			Stat->OnHPChanged.RemoveDynamic(this, &UBattleCharacterWidget::HandleHPChanged);
			Stat->OnDied.RemoveDynamic(this, &UBattleCharacterWidget::HandleDied);
		}
	}
	if (SelectButton)
	{
		SelectButton->OnClicked.RemoveDynamic(this, &UBattleCharacterWidget::OnSelectButtonClicked);
	}
	Super::NativeDestruct();
}

void UBattleCharacterWidget::Refresh()
{
	if (!TargetCharacter) { return; }
	UStatComponent* Stat = TargetCharacter->GetStatComponent();
	if (!Stat) { return; }

	if (NameText) { NameText->SetText(Stat->GetDisplayName()); }

	HandleHPChanged(Stat->GetCurrentHP(), Stat->GetMaxHP());

	const int32 MaxMP = Stat->GetStatData().MaxMP;
	if (MPBar)
	{
		MPBar->SetPercent(MaxMP > 0 ? (float)Stat->GetCurrentMP() / (float)MaxMP : 0.f);
	}
	if (MPText)
	{
		MPText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), Stat->GetCurrentMP(), MaxMP)));
	}

	SetRenderOpacity(Stat->IsDead() ? 0.5f : 1.f);
}

void UBattleCharacterWidget::HandleHPChanged(int32 NewHP, int32 MaxHP)
{
	const float Pct = MaxHP > 0 ? (float)NewHP / (float)MaxHP : 0.f;

	if (HPBar)
	{
		HPBar->SetPercent(Pct);

		FLinearColor BarColor;
		if (Pct > 0.5f) { BarColor = FLinearColor(0.2f, 0.85f, 0.3f); }
		else if (Pct > 0.25f) { BarColor = FLinearColor(0.95f, 0.85f, 0.1f); }
		else { BarColor = FLinearColor(0.9f, 0.2f, 0.2f); }
		HPBar->SetFillColorAndOpacity(BarColor);
	}
	if (HPText)
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), NewHP, MaxHP)));
	}
}

void UBattleCharacterWidget::HandleDied()
{
	SetRenderOpacity(0.5f);
	SetSelectable(false);
	SetHighlight(false);
}

void UBattleCharacterWidget::SetSelectable(bool bInSelectable)
{
	bSelectable = bInSelectable;
	if (SelectButton) { SelectButton->SetIsEnabled(bSelectable); }
}

void UBattleCharacterWidget::SetHighlight(bool bShow)
{
	if (HighlightBorder)
	{
		HighlightBorder->SetVisibility(bShow
			                               ? ESlateVisibility::HitTestInvisible
			                               : ESlateVisibility::Hidden);
	}
}

void UBattleCharacterWidget::OnSelectButtonClicked()
{
	if (!bSelectable || !TargetCharacter) { return; }
	OnCharacterClicked.Broadcast(TargetCharacter);
}
