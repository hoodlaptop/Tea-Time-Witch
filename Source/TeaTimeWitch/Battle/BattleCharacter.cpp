#include "BattleCharacter.h"

#include "StatComponent.h"
#include "SkillComponent.h"

#include "Components/CapsuleComponent.h"
#include "Engine/DataTable.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ABattleCharacter::ABattleCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(34.f, 80.f);
	GetCharacterMovement()->GravityScale = 0.f; // 전투는 그리드 위치 고정
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	Stats = CreateDefaultSubobject<UStatComponent>(TEXT("Stats"));
	Skills = CreateDefaultSubobject<USkillComponent>(TEXT("Skills"));

	Flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
	Flipbook->SetupAttachment(RootComponent);
	Flipbook->SetRelativeLocation(FVector(0.f, 0.f, -80.f));
	Flipbook->SetCastShadow(true);
}

void ABattleCharacter::InitializeBattle(FName InCharacterID, EBattleSide InSide,
                                        UDataTable* StatTable, UDataTable* SkillTable)
{
	CharacterID = InCharacterID;
	Side = InSide;

	if (!StatTable)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleCharacter] StatTable null for %s"),
		       *InCharacterID.ToString());
		return;
	}

	const FCharacterStatData* Row = StatTable->FindRow<FCharacterStatData>(
		InCharacterID, TEXT("BattleCharacter::InitializeBattle"));
	if (!Row)
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleCharacter] Missing row '%s' in DT_CharacterStats"),
		       *InCharacterID.ToString());
		return;
	}

	Stats->InitFromData(*Row);

	TArray<FName> SkillIDs;
	if (Row->DefaultSkillID != NAME_None) { SkillIDs.Add(Row->DefaultSkillID); }
	Skills->Init(SkillTable, SkillIDs);

	UE_LOG(LogTemp, Log, TEXT("[BattleCharacter] %s spawned (Side=%d, HP=%d, SPD=%d, Skill=%s)"),
	       *InCharacterID.ToString(), (int32)InSide, Row->MaxHP, Row->Speed,
	       *Row->DefaultSkillID.ToString());
}

bool ABattleCharacter::CanAct() const
{
	return Stats && !Stats->IsDead();
}
