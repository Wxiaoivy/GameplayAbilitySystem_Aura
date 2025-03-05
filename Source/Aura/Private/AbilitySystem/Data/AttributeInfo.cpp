// Copyright Ivy


#include "AbilitySystem/Data/AttributeInfo.h"


// UAttributeInfo ��ĳ�Ա����ʵ�֣��������Ա�ǩ���Ҷ�Ӧ��������Ϣ
FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound)
{

	// ����������Ϣ����(����AttributeInformation�����е�ÿһ��Ԫ�أ�����ÿ��ѭ���н���ǰԪ�ظ�ֵ��Info������)
	//const FAuraAttributeInfo& Info����ÿ��ѭ���У�Info�Ƕ�AttributeInformation�����е�ǰԪ�ص����á�ʹ��const��ʾ��ѭ���в����޸�Info�����ݡ�
	for (const FAuraAttributeInfo& Info : AttributeInformation)
	{

		// ����ҵ��봫���ǩ��ȫƥ���������Ϣ(MatchesTagExact��Unreal Engine�����ڱȽ�����FGameplayTag����Ϸ�Ա�ǩ���Ƿ���ȫ��ȵķ�����)
		if (Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			// ����ƥ���������Ϣ
			return Info;
		}
		return FAuraAttributeInfo();//�����Լ��ӵ�
	}

	// ���û���ҵ�ƥ���������Ϣ��������Ҫ��ӡ��־
	if (bLogNotFound)
	{
		// ����Ļ����ӵ�����Ϣ����ʾδ�ҵ��ı�ǩ��������������
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Can't find Info for AttributeTag [%s] on AttributeInfo [%s]."), *AttributeTag.ToString(), *GetNameSafe(this)));
	}

	// ���δ�ҵ�ƥ���������Ϣ������һ���յ� FAuraAttributeInfo Ĭ��ֵ
	return FAuraAttributeInfo();
}
