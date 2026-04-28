#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "TTWDialogueTypes.h"
#include "DialogueSystem.generated.h"

class ANPCBase;
class AEliaCharacter;
class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLineChanged, const FDialogueLine&, Line);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConversationStarted, ANPCBase*, Speaker);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnConversationEnded);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnActionTriggered, EDialogueAction, Action, FName, Param);

UCLASS()
class TEATIMEWITCH_API UDialogueSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	bool bIsActive = false;
	UPROPERTY(BlueprintReadOnly)
	FName CurrentLineID;
	UPROPERTY(BlueprintReadOnly)
	FDialogueLine CurrentLine;
	UPROPERTY()
	TObjectPtr<ANPCBase> CurrentSpeaker;
	UPROPERTY()
	TObjectPtr<UDataTable> CurrentTable;

	UFUNCTION(BlueprintCallable)
	bool StartConversation(FName StartLineID, ANPCBase* Speaker, UDataTable* Table);

	UFUNCTION(BlueprintCallable)
	void AdvanceLine();
	UFUNCTION(BlueprintCallable)
	void JumpToLine(FName LineID);
	UFUNCTION(BlueprintCallable)
	void EndConversation();

	UFUNCTION(BlueprintCallable)
	void NotifyActionResult(bool bSuccess);

	UPROPERTY(BlueprintAssignable)
	FOnLineChanged OnLineChanged;
	UPROPERTY(BlueprintAssignable)
	FOnConversationStarted OnStarted;
	UPROPERTY(BlueprintAssignable)
	FOnConversationEnded OnEnded;
	UPROPERTY(BlueprintAssignable)
	FOnActionTriggered OnAction;

private:
	bool bWaitingForActionResult = false;
	void ProcessCurrentLineAction();
	bool LoadLine(FName LineID, FDialogueLine& OutLine) const;
};
