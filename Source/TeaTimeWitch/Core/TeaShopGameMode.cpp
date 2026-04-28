#include "Core/TeaShopGameMode.h"

#include "TeaTimeWitchPlayerController.h"
#include "../Characters/EliaCharacter.h"
#include "../Characters/NPCBase.h"
#include "../Dialogue/DialogueSystem.h"
#include "../UI/TeaShopHUDWidget.h"
#include "../UI/DialogueWidget.h"
#include "../UI/TeaCraftingWidget.h"
#include "../TeaCraft/TeaCraftingComponent.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

ATeaShopGameMode::ATeaShopGameMode()
{
	DefaultPawnClass = AEliaCharacter::StaticClass();
	PlayerControllerClass = ATeaTimeWitchPlayerController::StaticClass();
}

void ATeaShopGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) { return; }

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UTeaShopHUDWidget>(PC, HUDWidgetClass);
		if (HUDWidget) { HUDWidget->AddToViewport(0); }
	}

	if (UDialogueSystem* DS = GetWorld()->GetSubsystem<UDialogueSystem>())
	{
		DS->OnStarted.AddDynamic(this, &ATeaShopGameMode::HandleDialogueStarted);
		DS->OnEnded.AddDynamic(this, &ATeaShopGameMode::HandleDialogueEnded);
		DS->OnAction.AddDynamic(this, &ATeaShopGameMode::HandleDialogueAction);
	}

	if (UTeaCraftingComponent* Comp = GetElia() ? GetElia()->GetCraftingComp() : nullptr)
	{
		if (!bBrewBound)
		{
			Comp->OnBrewComplete.AddDynamic(this, &ATeaShopGameMode::HandleBrewComplete);
			bBrewBound = true;
		}
	}
}

void ATeaShopGameMode::HandleDialogueStarted(ANPCBase* /*Speaker*/)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC || !DialogueWidgetClass || !HUDWidget) { return; }

	ActiveDialogueWidget = CreateWidget<UDialogueWidget>(PC, DialogueWidgetClass);
	if (!ActiveDialogueWidget) { return; }

	HUDWidget->SetDialogueWidget(ActiveDialogueWidget);

	if (auto* TPC = Cast<ATeaTimeWitchPlayerController>(PC))
	{
		TPC->SetTeaShopInputState(ETeaShopInputState::Dialogue, ActiveDialogueWidget);
	}
}

void ATeaShopGameMode::HandleDialogueEnded()
{
	if (HUDWidget) { HUDWidget->ClearDialogueWidget(); }
	ActiveDialogueWidget = nullptr;
	ExpectedRecipeID = NAME_None;

	if (auto* TPC = Cast<ATeaTimeWitchPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		TPC->SetTeaShopInputState(ETeaShopInputState::Game, nullptr);
	}
}

void ATeaShopGameMode::HandleDialogueAction(EDialogueAction Action, FName Param)
{
	if (Action == EDialogueAction::StartBattle)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GM] StartBattle requested (Param=%s) — B10 예정"), *Param.ToString());
		return;
	}
	if (Action != EDialogueAction::OpenTeaCraft) { return; }

	AEliaCharacter* Elia = GetElia();
	UTeaCraftingComponent* Comp = Elia ? Elia->GetCraftingComp() : nullptr;
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!Comp || !PC || !TeaCraftingWidgetClass || !HUDWidget) { return; }

	ExpectedRecipeID = Param;

	ActiveCraftingWidget = CreateWidget<UTeaCraftingWidget>(PC, TeaCraftingWidgetClass);
	if (!ActiveCraftingWidget) { return; }
	ActiveCraftingWidget->InitWithComponent(Comp);
	HUDWidget->SetTeaCraftWidget(ActiveCraftingWidget);

	if (auto* TPC = Cast<ATeaTimeWitchPlayerController>(PC))
	{
		TPC->SetTeaShopInputState(ETeaShopInputState::Crafting, ActiveCraftingWidget);
	}
}

void ATeaShopGameMode::HandleBrewComplete(const FBrewResult& Result)
{
	UDialogueSystem* DS = GetWorld()->GetSubsystem<UDialogueSystem>();
	if (!DS || !DS->bIsActive) { return; } // T키로 직접 연 일반 조합은 무시

	const bool bMatched = Result.bSuccess && Result.MatchedRecipeID == ExpectedRecipeID;

	if (HUDWidget) { HUDWidget->ClearTeaCraftWidget(); }
	ActiveCraftingWidget = nullptr;

	if (auto* TPC = Cast<ATeaTimeWitchPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		TPC->SetTeaShopInputState(ETeaShopInputState::Dialogue, ActiveDialogueWidget);
	}

	DS->NotifyActionResult(bMatched);
}

void ATeaShopGameMode::CloseTeaCraft()
{
	if (HUDWidget) { HUDWidget->ClearTeaCraftWidget(); }
	ActiveCraftingWidget = nullptr;
}

void ATeaShopGameMode::OpenTeaCraftDirect(AEliaCharacter* Elia)
{
	if (!Elia || !Elia->GetCraftingComp() || !TeaCraftingWidgetClass || !HUDWidget) { return; }
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) { return; }

	ActiveCraftingWidget = CreateWidget<UTeaCraftingWidget>(PC, TeaCraftingWidgetClass);
	if (!ActiveCraftingWidget) { return; }
	ActiveCraftingWidget->InitWithComponent(Elia->GetCraftingComp());
	HUDWidget->SetTeaCraftWidget(ActiveCraftingWidget);

	if (auto* TPC = Cast<ATeaTimeWitchPlayerController>(PC))
	{
		TPC->SetTeaShopInputState(ETeaShopInputState::Crafting, ActiveCraftingWidget);
	}
	ExpectedRecipeID = NAME_None; // 대화 컨텍스트 없음
}

AEliaCharacter* ATeaShopGameMode::GetElia() const
{
	return Cast<AEliaCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}
