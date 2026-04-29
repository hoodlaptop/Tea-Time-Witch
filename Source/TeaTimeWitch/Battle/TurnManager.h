#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BattleTypes.h"
#include "TurnManager.generated.h"

class ABattleCharacter;
class ABattleManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBattleStateChanged, EBattleState, NewState);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerActionRequested, ABattleCharacter*, Actor);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionExecuted, const FBattleActionResult&, Result);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBattleEnded, bool, bWon);

UCLASS()
class TEATIMEWITCH_API ATurnManager : public AActor
{
	GENERATED_BODY()

public:
	ATurnManager();

	virtual void Tick(float DeltaTime) override;

	void Initialize(ABattleManager* InManager);
	void StartBattle();

	UFUNCTION(BlueprintPure)
	EBattleState GetState() const { return CurrentState; }

	UPROPERTY(BlueprintAssignable)
	FOnBattleStateChanged OnStateChanged;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerActionRequested OnPlayerActionRequested;

	UPROPERTY(BlueprintAssignable)
	FOnActionExecuted OnActionExecuted;

	UPROPERTY(BlueprintAssignable)
	FOnBattleEnded OnBattleEnded;

	// 외부(UI)에서 호출. PlayerTurn 상태에서만 유효.
	UFUNCTION(BlueprintCallable)
	void RequestPlayerAction(FName SkillID, ABattleCharacter* Target);

protected:
	void ChangeState(EBattleState NewState);

	void Handle_BattleStart(float Dt);
	void Handle_TurnStart(float Dt);
	void Handle_PlayerTurn(float Dt);
	void Handle_EnemyTurn(float Dt);
	void Handle_ActionExecuting(float Dt);
	void Handle_TurnEnd(float Dt);

	void BuildActionOrder();
	void AdvanceToNextActor();
	bool CheckBattleEnd();
	void ExecuteQueuedAction();

	// B9에서 override → 입력 대기 모드로 변경
	virtual void DecidePlayerAction(ABattleCharacter* Actor);
	virtual void DecideEnemyAction(ABattleCharacter* Actor);

protected:
	UPROPERTY()
	TObjectPtr<ABattleManager> BattleManager;

	UPROPERTY(VisibleAnywhere)
	EBattleState CurrentState = EBattleState::None;
	UPROPERTY(VisibleAnywhere)
	int32 CurrentActorIndex = -1;
	UPROPERTY(VisibleAnywhere)
	int32 TurnCount = 0;

	UPROPERTY()
	TArray<TObjectPtr<ABattleCharacter>> ActionOrder;

	UPROPERTY()
	TObjectPtr<ABattleCharacter> QueuedActor;
	UPROPERTY()
	TObjectPtr<ABattleCharacter> QueuedTarget;
	FName QueuedSkillID = NAME_None;

	float StateTimer = 0.f;

	UPROPERTY(EditAnywhere)
	float Delay_BattleStart = 1.0f;
	UPROPERTY(EditAnywhere)
	float Delay_TurnStart = 0.3f;
	UPROPERTY(EditAnywhere)
	float Delay_ActionExecuting = 1.0f;
	UPROPERTY(EditAnywhere)
	float Delay_TurnEnd = 0.5f;
	UPROPERTY(EditAnywhere)
	int32 MaxTurns = 100;

	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDebugTurnLog = false;
	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDebugAutoBattle = false;

	bool bActionExecuted = false;
	bool bWaitingForPlayerInput = false;
};
