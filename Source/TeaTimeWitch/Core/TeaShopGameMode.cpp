#include "Core/TeaShopGameMode.h"

#include "DungeonProgressSubsystem.h"
#include "EngineUtils.h"
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
	// base class에서 처리중
}

void ATeaShopGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	ShopHUD = Cast<UTeaShopHUDWidget>(HUDWidget);
	if (!ShopHUD) return;

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
	
	if (auto* TPC = Cast<ATeaTimeWitchPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		TPC->SetTeaShopInputState(ETeaShopInputState::Game, nullptr);
	}
	
	if (auto* DP = GetGameInstance()->GetSubsystem<UDungeonProgressSubsystem>())
	{
		if (DP->bIsReturningFromDungeon && DP->LastClearedDungeonID == TEXT("Trench"))
		{
			for (TActorIterator<ANPCBase> It(GetWorld()); It; ++It)
			{
				ANPCBase* NPC = *It;
				if (!NPC) continue;

				// 아릭 식별: StartLineID prefix로 매칭 (또는 NPC 클래스/태그로)
				if (NPC->StartLineID == TEXT("Aric_001"))   // 실제 시작 라인 ID로 교체(현재는 가정)
				{
					NPC->StartLineID = TEXT("Aric_AfterDungeon");
					UE_LOG(LogTemp, Log, TEXT("[GM] Aric line switched to Aric_AfterDungeon"));
				}
			}
			DP->bIsReturningFromDungeon = false;
		}
	}
}

void ATeaShopGameMode::HandleDialogueStarted(ANPCBase* /*Speaker*/)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC || !DialogueWidgetClass || !ShopHUD) { return; }

	ActiveDialogueWidget = CreateWidget<UDialogueWidget>(PC, DialogueWidgetClass);
	if (!ActiveDialogueWidget) { return; }

	ShopHUD->SetDialogueWidget(ActiveDialogueWidget);

	if (auto* TPC = Cast<ATeaTimeWitchPlayerController>(PC))
	{
		TPC->SetTeaShopInputState(ETeaShopInputState::Dialogue, ActiveDialogueWidget);
	}
}

void ATeaShopGameMode::HandleDialogueEnded()
{
	if (ShopHUD) { ShopHUD->ClearDialogueWidget(); }
	ActiveDialogueWidget = nullptr;
	ExpectedRecipeID = NAME_None;

	if (auto* TPC = Cast<ATeaTimeWitchPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		TPC->SetTeaShopInputState(ETeaShopInputState::Game, nullptr);
	}
}

void ATeaShopGameMode::HandleDialogueAction(EDialogueAction Action, FName Param)
{
	if (Action == EDialogueAction::EnterDungeon)
	{
		if (Param.IsNone())
		{
			UE_LOG(LogTemp, Warning, TEXT("[GM] EnterDungeon: ActionParam is empty"));
			return;
		}

		// 대화 종료
		if (UDialogueSystem* DS = GetWorld()->GetSubsystem<UDialogueSystem>())
		{
			DS->EndConversation();
		}

		// 던전 ID 등록
		if (auto* DP = GetGameInstance()->GetSubsystem<UDungeonProgressSubsystem>())
		{
			DP->EnterDungeon(TEXT("Trench"));
		}

		UE_LOG(LogTemp, Log, TEXT("[GM] EnterDungeon -> %s"), *Param.ToString());
		UGameplayStatics::OpenLevel(this, Param);
		return;
	}
	if (Action == EDialogueAction::StartBattle)
	{
		UE_LOG(LogTemp, Warning, TEXT("[GM] StartBattle requested (Param=%s) — B10 예정"), *Param.ToString());
		return;
	}
	
	if (Action != EDialogueAction::OpenTeaCraft) { return; }
	
	AEliaCharacter* Elia = GetElia();
	UTeaCraftingComponent* Comp = Elia ? Elia->GetCraftingComp() : nullptr;
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!Comp || !PC || !TeaCraftingWidgetClass || !ShopHUD) { return; }

	ExpectedRecipeID = Param;

	ActiveCraftingWidget = CreateWidget<UTeaCraftingWidget>(PC, TeaCraftingWidgetClass);
	if (!ActiveCraftingWidget) { return; }
	ActiveCraftingWidget->InitWithComponent(Comp);
	ShopHUD->SetTeaCraftWidget(ActiveCraftingWidget);

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

	if (ShopHUD) { ShopHUD->ClearTeaCraftWidget(); }
	ActiveCraftingWidget = nullptr;

	if (auto* TPC = Cast<ATeaTimeWitchPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
	{
		TPC->SetTeaShopInputState(ETeaShopInputState::Dialogue, ActiveDialogueWidget);
	}

	DS->NotifyActionResult(bMatched);
}

void ATeaShopGameMode::CloseTeaCraft()
{
	if (ShopHUD) { ShopHUD->ClearTeaCraftWidget(); }
	ActiveCraftingWidget = nullptr;
}

void ATeaShopGameMode::OpenTeaCraftDirect(AEliaCharacter* Elia)
{
	if (!Elia || !Elia->GetCraftingComp() || !TeaCraftingWidgetClass || !ShopHUD) { return; }
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC) { return; }

	ActiveCraftingWidget = CreateWidget<UTeaCraftingWidget>(PC, TeaCraftingWidgetClass);
	if (!ActiveCraftingWidget) { return; }
	ActiveCraftingWidget->InitWithComponent(Elia->GetCraftingComp());
	ShopHUD->SetTeaCraftWidget(ActiveCraftingWidget);

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
