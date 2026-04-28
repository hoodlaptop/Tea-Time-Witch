#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleTypes.h"
#include "StatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHPChanged, int32, NewHP, int32, MaxHP);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDied);

UCLASS(ClassGroup=(Battle), meta=(BlueprintSpawnableComponent))
class TEATIMEWITCH_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatComponent();

	void InitFromData(const FCharacterStatData& Data);

	int32 TakeDamage(int32 RawDamage, EEmotionType DamageAroma);
	void Heal(int32 Amount);
	bool ConsumeMP(int32 Amount);

	UFUNCTION(BlueprintPure)
	bool IsDead() const { return CurrentHP <= 0; }

	UFUNCTION(BlueprintPure)
	int32 GetCurrentHP() const { return CurrentHP; }

	UFUNCTION(BlueprintPure)
	int32 GetMaxHP() const { return Stats.MaxHP; }

	UFUNCTION(BlueprintPure)
	int32 GetCurrentMP() const { return CurrentMP; }

	UFUNCTION(BlueprintPure)
	int32 GetSpeed() const { return Stats.Speed; }

	UFUNCTION(BlueprintPure)
	int32 GetAttack() const { return Stats.Attack; }

	UFUNCTION(BlueprintPure)
	FText GetDisplayName() const { return Stats.DisplayName; }

	UFUNCTION(BlueprintPure)
	const FCharacterStatData& GetStatData() const { return Stats; }

	UPROPERTY(BlueprintAssignable)
	FOnHPChanged OnHPChanged;
	UPROPERTY(BlueprintAssignable)
	FOnDied OnDied;

protected:
	UPROPERTY(VisibleAnywhere)
	FCharacterStatData Stats;
	UPROPERTY(VisibleAnywhere)
	int32 CurrentHP = 0;
	UPROPERTY(VisibleAnywhere)
	int32 CurrentMP = 0;
	bool bDeathBroadcast = false;
};
