#include "StatComponent.h"

UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UStatComponent::InitFromData(const FCharacterStatData& Data)
{
	Stats = Data;
	CurrentHP = Stats.MaxHP;
	CurrentMP = Stats.MaxMP;
	bDeathBroadcast = false;
}

int32 UStatComponent::TakeDamage(int32 RawDamage, EEmotionType DamageAroma)
{
	if (IsDead()) { return 0; }

	float Damage = static_cast<float>(RawDamage);
	if (Stats.WeaknessAroma != EEmotionType::None && Stats.WeaknessAroma == DamageAroma)
	{
		Damage *= 1.5f;
	}

	const int32 Final = FMath::Max(1, FMath::FloorToInt(Damage) - Stats.Defense);
	const int32 OldHP = CurrentHP;
	CurrentHP = FMath::Clamp(CurrentHP - Final, 0, Stats.MaxHP);

	OnHPChanged.Broadcast(CurrentHP, Stats.MaxHP);

	if (CurrentHP <= 0 && !bDeathBroadcast)
	{
		bDeathBroadcast = true;
		OnDied.Broadcast();
	}
	return Final;
}

void UStatComponent::Heal(int32 Amount)
{
	if (IsDead()) { return; }
	CurrentHP = FMath::Clamp(CurrentHP + Amount, 0, Stats.MaxHP);
	OnHPChanged.Broadcast(CurrentHP, Stats.MaxHP);
}

bool UStatComponent::ConsumeMP(int32 Amount)
{
	if (CurrentMP < Amount) { return false; }
	CurrentMP -= Amount;
	return true;
}
