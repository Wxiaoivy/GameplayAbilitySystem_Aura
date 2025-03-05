// Copyright Ivy


#include "AuraAssetManager.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	// TODO: �ڴ˴����� return ���
	check(GEngine);
	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return*AuraAssetManager;

}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FAuraGameplayTags::InitializeNativeGameplayTags();
}
