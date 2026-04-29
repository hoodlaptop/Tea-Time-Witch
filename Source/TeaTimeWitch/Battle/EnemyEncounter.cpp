#include "Battle/EnemyEncounter.h"

#include "DungeonProgressSubsystem.h"
#include "Characters/EliaCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AEnemyEncounter::AEnemyEncounter()
{
	PrimaryActorTick.bCanEverTick = false;

	PlaceholderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaceholderMesh"));
	SetRootComponent(PlaceholderMesh);
	PlaceholderMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlaceholderMesh->SetCollisionResponseToAllChannels(ECR_Block);
	PlaceholderMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	EncounterVolume = CreateDefaultSubobject<USphereComponent>(TEXT("EncounterVolume"));
	EncounterVolume->SetupAttachment(PlaceholderMesh);
	EncounterVolume->SetSphereRadius(180.0f);
	EncounterVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	EncounterVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	EncounterVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	EncounterVolume->SetGenerateOverlapEvents(true);
}

bool AEnemyEncounter::TryInteract(AEliaCharacter* Player)
{
	if (bDefeated || !bPlayerInRange || !Player) return false;

	TriggerBattle(Player);
	return true;
}

void AEnemyEncounter::BeginPlay()
{
	Super::BeginPlay();

	EncounterVolume->OnComponentBeginOverlap.AddDynamic(this, &AEnemyEncounter::OnVolumeBeginOverlap);
	EncounterVolume->OnComponentEndOverlap.AddDynamic(this, &AEnemyEncounter::OnVolumeEndOverlap);

	// 이미 클리어된 인카운터는 비활성화
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UDungeonProgressSubsystem* DP = GI->GetSubsystem<UDungeonProgressSubsystem>())
		{
			if (!EncounterID.IsNone() && DP->IsEncounterCleared(EncounterID))
			{
				bDefeated = true;
				SetActorHiddenInGame(true);
				SetActorEnableCollision(false);
			}
		}
	}
}

void AEnemyEncounter::OnVolumeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                           const FHitResult& Sweep)
{
	if (bDefeated) return;

	AEliaCharacter* Player = Cast<AEliaCharacter>(OtherActor);
	if (Player == nullptr) return;

	bPlayerInRange = true;

	if (bAutoTrigger)
	{
		TriggerBattle(Player);
	}
}

void AEnemyEncounter::OnVolumeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AEliaCharacter>(OtherActor))
	{
		bPlayerInRange = false;
	}
}

void AEnemyEncounter::TriggerBattle(AEliaCharacter* Player)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UDungeonProgressSubsystem* DP = GI->GetSubsystem<UDungeonProgressSubsystem>();
	if (!DP) return;

	const FVector ReturnLoc = Player ? Player->GetActorLocation() : GetActorLocation();
	const FName ReturnLevel = FName(*UGameplayStatics::GetCurrentLevelName(this, true));

	DP->RequestBattleTransition(EncounterID, EnemyIDs, ReturnLoc, ReturnLevel, bIsFinalEncounter);
}
