#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleTypes.h"
#include "SkillComponent.generated.h"

class ABattleCharacter;

UCLASS(ClassGroup=(Battle), meta=(BlueprintSpawnableComponent))
class TEATIMEWITCH_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillComponent();

	void Init(class UDataTable* InSkillTable, const TArray<FName>& InSkillIDs);

	bool TryUseSkill(FName SkillID, ABattleCharacter* User, ABattleCharacter* Target);

	UFUNCTION(BlueprintPure)
	bool HasSkill(FName SkillID) const { return SkillIDs.Contains(SkillID); }

	UFUNCTION(BlueprintPure)
	const TArray<FName>& GetSkillIDs() const { return SkillIDs; }

	FName PickFirstAvailableSkill(int32 CurrentMP) const;

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable> SkillTable;
	UPROPERTY(EditAnywhere)
	TArray<FName> SkillIDs;
};
