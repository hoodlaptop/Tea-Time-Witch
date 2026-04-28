#include "TurnManager.h"

#include "BattleManager.h"
#include "BattleCharacter.h"
#include "StatComponent.h"
#include "SkillComponent.h"

ATurnManager::ATurnManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATurnManager::Initialize(ABattleManager* InManager)
{
	BattleManager = InManager;
}

void ATurnManager::StartBattle()
{
	ChangeState(EBattleState::BattleStart);
}

void ATurnManager::ChangeState(EBattleState NewState)
{
	CurrentState = NewState;
	StateTimer = 0.f;
	bActionExecuted = false;

	if (GEngine)
	{
		const UEnum* EnumPtr = StaticEnum<EBattleState>();
		const FString Name = EnumPtr ? EnumPtr->GetNameStringByValue((int64)NewState) : TEXT("?");
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green,
		                                 FString::Printf(TEXT("[State] -> %s"), *Name));
	}
	OnStateChanged.Broadcast(NewState);
}

void ATurnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	StateTimer += DeltaTime;

	switch (CurrentState)
	{
	case EBattleState::BattleStart: Handle_BattleStart(DeltaTime);
		break;
	case EBattleState::TurnStart: Handle_TurnStart(DeltaTime);
		break;
	case EBattleState::PlayerTurn: Handle_PlayerTurn(DeltaTime);
		break;
	case EBattleState::EnemyTurn: Handle_EnemyTurn(DeltaTime);
		break;
	case EBattleState::ActionExecuting: Handle_ActionExecuting(DeltaTime);
		break;
	case EBattleState::TurnEnd: Handle_TurnEnd(DeltaTime);
		break;
	default: break;
	}
}

void ATurnManager::Handle_BattleStart(float)
{
	if (StateTimer >= Delay_BattleStart) { ChangeState(EBattleState::TurnStart); }
}

void ATurnManager::Handle_TurnStart(float)
{
	if (StateTimer < Delay_TurnStart) { return; }

	if (++TurnCount > MaxTurns)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TurnManager] Max turns reached, force end."));
		ChangeState(EBattleState::BattleEnd_Lose);
		SetActorTickEnabled(false);
		return;
	}

	BuildActionOrder();
	CurrentActorIndex = -1;
	AdvanceToNextActor();
}

void ATurnManager::BuildActionOrder()
{
	ActionOrder.Reset();
	if (!BattleManager) { return; }

	TArray<ABattleCharacter*> All;
	for (const TObjectPtr<ABattleCharacter>& C : BattleManager->GetAllies())
	{
		if (C && C->CanAct()) { All.Add(C); }
	}
	for (const TObjectPtr<ABattleCharacter>& C : BattleManager->GetEnemies())
	{
		if (C && C->CanAct()) { All.Add(C); }
	}

	All.Sort([](const ABattleCharacter& A, const ABattleCharacter& B)
	{
		return A.GetStatComponent()->GetSpeed() > B.GetStatComponent()->GetSpeed();
	});

	for (ABattleCharacter* C : All) { ActionOrder.Add(C); }

	if (GEngine)
	{
		FString Order = TEXT("Order: ");
		for (const TObjectPtr<ABattleCharacter>& C : ActionOrder)
		{
			Order += FString::Printf(TEXT("%s(%d) "),
			                         *C->GetCharacterID().ToString(), C->GetStatComponent()->GetSpeed());
		}
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, Order);
	}
}

void ATurnManager::AdvanceToNextActor()
{
	++CurrentActorIndex;
	while (ActionOrder.IsValidIndex(CurrentActorIndex)
		&& (!ActionOrder[CurrentActorIndex] || !ActionOrder[CurrentActorIndex]->CanAct()))
	{
		++CurrentActorIndex;
	}

	if (!ActionOrder.IsValidIndex(CurrentActorIndex))
	{
		// 라운드 종료 → 새 라운드
		ChangeState(EBattleState::TurnStart);
		return;
	}

	if (CheckBattleEnd()) { return; }

	QueuedActor = ActionOrder[CurrentActorIndex];
	QueuedTarget = nullptr;
	QueuedSkillID = NAME_None;

	ChangeState(QueuedActor->GetSide() == EBattleSide::Ally
		            ? EBattleState::PlayerTurn
		            : EBattleState::EnemyTurn);
}

void ATurnManager::Handle_PlayerTurn(float)
{
	if (!QueuedActor)
	{
		ChangeState(EBattleState::TurnEnd);
		return;
	}
	DecidePlayerAction(QueuedActor);
	ChangeState(EBattleState::ActionExecuting);
}

void ATurnManager::Handle_EnemyTurn(float)
{
	if (!QueuedActor)
	{
		ChangeState(EBattleState::TurnEnd);
		return;
	}
	DecideEnemyAction(QueuedActor);
	ChangeState(EBattleState::ActionExecuting);
}

void ATurnManager::DecidePlayerAction(ABattleCharacter* Actor)
{
	if (!Actor || !BattleManager) { return; }
	QueuedSkillID = Actor->GetSkillComponent()->PickFirstAvailableSkill(
		Actor->GetStatComponent()->GetCurrentMP());
	for (const TObjectPtr<ABattleCharacter>& C : BattleManager->GetEnemies())
	{
		if (C && C->CanAct())
		{
			QueuedTarget = C;
			break;
		}
	}
}

void ATurnManager::DecideEnemyAction(ABattleCharacter* Actor)
{
	if (!Actor || !BattleManager) { return; }
	QueuedSkillID = Actor->GetSkillComponent()->PickFirstAvailableSkill(
		Actor->GetStatComponent()->GetCurrentMP());

	TArray<ABattleCharacter*> Alive;
	for (const TObjectPtr<ABattleCharacter>& C : BattleManager->GetAllies())
	{
		if (C && C->CanAct()) { Alive.Add(C); }
	}
	if (Alive.Num() > 0)
	{
		QueuedTarget = Alive[FMath::RandRange(0, Alive.Num() - 1)];
	}
}

void ATurnManager::Handle_ActionExecuting(float)
{
	if (!bActionExecuted)
	{
		bActionExecuted = true;
		ExecuteQueuedAction();
	}
	if (StateTimer >= Delay_ActionExecuting)
	{
		ChangeState(EBattleState::TurnEnd);
	}
}

void ATurnManager::ExecuteQueuedAction()
{
	if (!QueuedActor || !QueuedTarget || QueuedSkillID == NAME_None) { return; }
	QueuedActor->GetSkillComponent()->TryUseSkill(QueuedSkillID, QueuedActor, QueuedTarget);
}

void ATurnManager::Handle_TurnEnd(float)
{
	if (StateTimer < Delay_TurnEnd) { return; }
	if (CheckBattleEnd()) { return; }
	AdvanceToNextActor();
}

bool ATurnManager::CheckBattleEnd()
{
	if (!BattleManager) { return false; }

	bool AllyAlive = false, EnemyAlive = false;
	for (const TObjectPtr<ABattleCharacter>& C : BattleManager->GetAllies())
	{
		if (C && C->CanAct())
		{
			AllyAlive = true;
			break;
		}
	}
	for (const TObjectPtr<ABattleCharacter>& C : BattleManager->GetEnemies())
	{
		if (C && C->CanAct())
		{
			EnemyAlive = true;
			break;
		}
	}

	if (!EnemyAlive)
	{
		ChangeState(EBattleState::BattleEnd_Win);
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Magenta, TEXT("BATTLE WON"));
		SetActorTickEnabled(false);
		return true;
	}
	if (!AllyAlive)
	{
		ChangeState(EBattleState::BattleEnd_Lose);
		if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("BATTLE LOST"));
		SetActorTickEnabled(false);
		return true;
	}
	return false;
}
