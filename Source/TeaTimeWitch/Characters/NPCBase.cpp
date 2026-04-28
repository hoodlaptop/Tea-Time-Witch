#include "NPCBase.h"

#include "../Dialogue/DialogueSystem.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

ANPCBase::ANPCBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	GetCapsuleComponent()->InitCapsuleSize(34.0f, 80.0f);
	
	InteractionVolume = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetSphereRadius(150.0f);
	InteractionVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionVolume->SetGenerateOverlapEvents(true);
}

bool ANPCBase::TryStartDialogue(AEliaCharacter* Player)
{
	if (StartLineID.IsNone() || !DialogueTable) return false;
	
	UWorld* World = GetWorld();
	if (!World) return false;
	
	UDialogueSystem* DS = World->GetSubsystem<UDialogueSystem>();
	if (!DS) return false;
	
	return DS->StartConversation(StartLineID, this, DialogueTable);
}




