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

	// �������Ա�ǩ���Ҷ�Ӧ��������Ϣ������Ҳ���������ѡ���Ƿ��ӡ��־
	FAuraAttributeInfo FindAttributeInfoForTag(const FGameplayTag&AttributeTag, bool bLogNotFound = false);


	// ������Ϣ���飬�洢��� FAuraAttributeInfo ʵ�������ڱ༭ʱĬ�����ã���ͼֻ��
	// ����������UE�༭����DA_AttributeInfo���ʼ��
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraAttributeInfo>AttributeInformation;


	
};
