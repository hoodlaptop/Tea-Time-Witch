#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPCBase.generated.h"

class AEliaCharacter;
class USphereComponent;

UCLASS()
class TEATIMEWITCH_API ANPCBase : public ACharacter
{
	GENERATED_BODY()

public:
	ANPCBase();
	
	UPROPERTY(EditAnywhere, Category="Dialogue")
	FName StartLineID;
	UPROPERTY(EditAnywhere, Category="Dialogue")
	FText DisplayName;
	UPROPERTY(EditAnywhere, Category="Dialogue")
	TObjectPtr<UDataTable> DialogueTable;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="NPC")
	TObjectPtr<USphereComponent> InteractionVolume;
	
	UFUNCTION(BlueprintCallable)
	bool TryStartDialogue(AEliaCharacter* Player);
};
