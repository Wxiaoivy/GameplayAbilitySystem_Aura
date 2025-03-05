// Copyright Ivy


#include "AuraAssetManager.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	// TODO: 在此处插入 return 语句
	check(GEngine);
	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return*AuraAssetManager;

}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FAuraGameplayTags::InitializeNativeGameplayTags();
}
