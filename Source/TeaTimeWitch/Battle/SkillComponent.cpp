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

FBattleActionResult USkillComponent::TryUseSkill(FName SkillID, ABattleCharacter* User, ABattleCharacter* Target)
{
	FBattleActionResult Result;
	Result.Source = User;
	Result.Target = Target;
	Result.SkillID = SkillID;

	if (!SkillTable || !User || !Target) { return Result; }

	const FSkillData* Row = SkillTable->FindRow<FSkillData>(SkillID, TEXT("USkillComponent::TryUseSkill"));
	if (!Row) { return Result; }

	UStatComponent* UserStats = User->GetStatComponent();
	UStatComponent* TargetStats = Target->GetStatComponent();
	if (!UserStats || !TargetStats) { return Result; }

	if (Row->MPCost > 0 && !UserStats->ConsumeMP(Row->MPCost))
	{
		Result.bMissed = true; // MP 부족 → 행동 실패로 표시
		return Result;
	}

	// 약점 사전 계산 (매니저 함정 #4: None == None 회피)
	const FCharacterStatData& TargetData = TargetStats->GetStatData();
	Result.bWeakness = (Row->AromaType != EEmotionType::None
		&& TargetData.WeaknessAroma == Row->AromaType);

	const int32 RawDamage = UserStats->GetAttack() + Row->BasePower;
	Result.Damage = TargetStats->TakeDamage(RawDamage, Row->AromaType);

	return Result;
}

const FSkillData* USkillComponent::GetSkillData(FName SkillID) const
{
	if (!SkillTable) { return nullptr; }
	return SkillTable->FindRow<FSkillData>(SkillID, TEXT("USkillComponent::GetSkillData"));
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
