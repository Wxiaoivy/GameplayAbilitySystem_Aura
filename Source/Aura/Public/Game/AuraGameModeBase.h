// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/GameFramework/SaveGame.h>
#include "AuraGameModeBase.generated.h"

/**
 * 
 */
class UCharacterClassInfo;
class UAbilityInfo;
class UMVVM_LoadSlot;
class ULoadScreenSaveGame;

UCLASS()
class AURA_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, Category = "CharacterClassDefaultInfo")
	TObjectPtr<UCharacterClassInfo>CharacterClassInfo;


	UPROPERTY(EditDefaultsOnly, Category = "AbilityInfo")
	TObjectPtr<UAbilityInfo>AbilityInfo;


	void SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex);
	ULoadScreenSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex)const;
	static void DeleteSlot(const FString& SlotName, int32 SlotIndex);



	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame>LoadScreenSaveGameClass;

};
