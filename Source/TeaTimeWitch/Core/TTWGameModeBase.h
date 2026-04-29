#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TTWGameModeBase.generated.h"

class UCommonUserWidget;

UCLASS()
class TEATIMEWITCH_API ATTWGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATTWGameModeBase();
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UCommonUserWidget> HUDWidgetClass;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TObjectPtr<UCommonUserWidget> HUDWidget;
};
