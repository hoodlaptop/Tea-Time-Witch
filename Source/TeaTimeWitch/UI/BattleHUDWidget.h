#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "../Battle/BattleTypes.h"
#include "BattleHUDWidget.generated.h"

class UHorizontalBox;
class UVerticalBox;
class UTextBlock;
class UButton;
class UWidgetSwitcher;
class UBorder;
class UBattleCharacterWidget;
class USkillButtonWidget;
class ABattleManager;
class ATurnManager;
class ABattleCharacter;

UENUM()
enum class EBattlePanel : uint8
{
	Command = 0,
	Skill = 1,
	Target = 2
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBattleResultConfirmed, bool, bWon);

UCLASS(Abstract, BlueprintType)
class TEATIMEWITCH_API UBattleHUDWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitWithBattle(ABattleManager* InBattle, ATurnManager* InTurn);

	UPROPERTY(BlueprintAssignable)
	FOnBattleResultConfirmed OnBattleResultConfirmed;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// === BindWidgets ===
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> AllyContainer;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> EnemyContainer;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> PanelSwitcher;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> AttackButton;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> SkillButton;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> SkillContainer;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> TargetCancelButton;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> BattleLogText;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> ResultPanel;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ResultText;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> ContinueButton;

	// === 동적 위젯 클래스 ===
	UPROPERTY(EditDefaultsOnly, Category="Battle UI")
	TSubclassOf<UBattleCharacterWidget> CharacterWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Battle UI")
	TSubclassOf<USkillButtonWidget> SkillButtonClass;

	// === Refs ===
	UPROPERTY()
	TObjectPtr<ABattleManager> BattleManager;
	UPROPERTY()
	TObjectPtr<ATurnManager> TurnManager;
	UPROPERTY()
	TObjectPtr<ABattleCharacter> CurrentActor;
	UPROPERTY()
	TArray<TObjectPtr<UBattleCharacterWidget>> AllyWidgets;
	UPROPERTY()
	TArray<TObjectPtr<UBattleCharacterWidget>> EnemyWidgets;

	FName SelectedSkillID;
	bool bWonCached = false;

	// === TurnManager 응답 ===
	UFUNCTION()
	void HandlePlayerActionRequested(ABattleCharacter* Actor);
	UFUNCTION()
	void HandleActionExecuted(const FBattleActionResult& Result);
	UFUNCTION()
	void HandleBattleEnded(bool bWon);

	// === UI 흐름 ===
	void RebuildCharacterPanels();
	void ShowPanel(EBattlePanel Panel);
	void HideAllPanels();
	void ShowSkillPanel(ABattleCharacter* User);
	void ShowTargetPanel(FName SkillID);
	void EnableEnemyTargets(bool bEnable);
	void ClearActorHighlights();
	void ShowResultPanel(bool bWon);
	void AppendBattleLog(const FString& Message, FLinearColor Color = FLinearColor::White);

	// === 입력 응답 ===
	UFUNCTION()
	void OnAttackClicked();
	UFUNCTION()
	void OnSkillClicked();
	UFUNCTION()
	void OnSkillSelected(FName SkillID);
	UFUNCTION()
	void OnTargetCancelClicked();
	UFUNCTION()
	void OnEnemyClicked(ABattleCharacter* Target);
	UFUNCTION()
	void OnContinueClicked();
};
