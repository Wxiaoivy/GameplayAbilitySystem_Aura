// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AttributeInfo.generated.h"


USTRUCT(BlueprintType)
struct FAuraAttributeInfo
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeScription = FText();

	UPROPERTY(BlueprintReadOnly)
	float  AttributeValue = 0.f;



};
/**
 * 
 */


UCLASS()
class AURA_API UAttributeInfo : public UDataAsset
{
	GENERATED_BODY()

public:

	// 根据属性标签查找对应的属性信息，如果找不到，可以选择是否打印日志
	FAuraAttributeInfo FindAttributeInfoForTag(const FGameplayTag&AttributeTag, bool bLogNotFound = false);


	// 属性信息数组，存储多个 FAuraAttributeInfo 实例，仅在编辑时默认设置，蓝图只读
	// 这个数组会在UE编辑器的DA_AttributeInfo里初始化
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraAttributeInfo>AttributeInformation;


	
};
