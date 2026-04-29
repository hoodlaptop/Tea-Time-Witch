#include "SkillComponent.h"

#include "BattleCharacter.h"
#include "StatComponent.h"
#include "Engine/DataTable.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USkillComponent::Init(UDataTable* InSkillTable, const TArray<FName>& InSkillIDs)
{
	SkillTable = InSkillTable;
	SkillIDs = InSkillIDs;
}

bool USkillComponent::TryUseSkill(FName SkillID, ABattleCharacter* User, ABattleCharacter* Target)
{
	if (!SkillTable || !User || !Target) { return false; }

	const FSkillData* Row = SkillTable->FindRow<FSkillData>(SkillID, TEXT("USkillComponent::TryUseSkill"));
	if (!Row) { return false; }

	UStatComponent* UserStats = User->GetStatComponent();
	UStatComponent* TargetStats = Target->GetStatComponent();
	if (!UserStats || !TargetStats) { return false; }

	if (Row->MPCost > 0 && !UserStats->ConsumeMP(Row->MPCost)) { return false; }

	const int32 RawDamage = UserStats->GetAttack() + Row->BasePower;
	TargetStats->TakeDamage(RawDamage, Row->AromaType);

	return true;
}

FName USkillComponent::PickFirstAvailableSkill(int32 CurrentMP) const
{
	if (!SkillTable) { return NAME_None; }
	for (const FName& ID : SkillIDs)
	{
		const FSkillData* Row = SkillTable->FindRow<FSkillData>(ID, TEXT("PickFirstAvailableSkill"));
		if (Row && Row->MPCost <= CurrentMP) { return ID; }
	}
	return NAME_None;
}
