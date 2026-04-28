#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BattleTypes.h"
#include "BattleCharacter.generated.h"

class UStatComponent;
class USkillComponent;
class UPaperFlipbookComponent;
class UDataTable;

UCLASS()
class TEATIMEWITCH_API ABattleCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABattleCharacter();

	void InitializeBattle(FName InCharacterID, EBattleSide InSide,
	                      UDataTable* StatTable, UDataTable* SkillTable);

	UFUNCTION(BlueprintPure)
	UStatComponent* GetStatComponent() const { return Stats; }

	UFUNCTION(BlueprintPure)
	USkillComponent* GetSkillComponent() const { return Skills; }

	UFUNCTION(BlueprintPure)
	EBattleSide GetSide() const { return Side; }

	UFUNCTION(BlueprintPure)
	FName GetCharacterID() const { return CharacterID; }

	bool CanAct() const;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStatComponent> Stats;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkillComponent> Skills;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPaperFlipbookComponent> Flipbook;

	UPROPERTY(VisibleAnywhere)
	EBattleSide Side = EBattleSide::Ally;
	UPROPERTY(VisibleAnywhere)
	FName CharacterID;
};
