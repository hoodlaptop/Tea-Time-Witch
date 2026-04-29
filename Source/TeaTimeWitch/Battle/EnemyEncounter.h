#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyEncounter.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class AEliaCharacter;

UCLASS()
class TEATIMEWITCH_API AEnemyEncounter : public AActor
{
	GENERATED_BODY()

public:
	AEnemyEncounter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<USphereComponent> EncounterVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> PlaceholderMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Encounter")
	FName EncounterID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Encounter")
	TArray<FName> EnemyIDs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Encounter")
	FText EncounterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Encounter",
		meta=(ToolTip="마지막/보스 슬롯. 처치 시 던전 자동 클리어"))
	bool bIsFinalEncounter = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Encounter",
		meta=(ToolTip="true면 진입 즉시 전투. false면 E키 필요"))
	bool bAutoTrigger = false;

	UPROPERTY(BlueprintReadOnly, Category="Encounter")
	bool bDefeated = false;

	UPROPERTY(BlueprintReadOnly, Category="Encounter")
	bool bPlayerInRange = false;

	UFUNCTION(BlueprintCallable, Category="Encounter")
	bool TryInteract(AEliaCharacter* Player);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnVolumeBeginOverlap(
		UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex,
		bool bFromSweep, 
		const FHitResult& Sweep);

	UFUNCTION()
	void OnVolumeEndOverlap(
		UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	void TriggerBattle(AEliaCharacter* Player);
};
