// Copyright Ivy


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound)const
{
	//ͬ���Ĳ������Կ�AttributeInfo�����Ҳ������
	for (const FAuraInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction&& Action.InputTag.MatchesTagExact(InputTag))
		{
			return Action.InputAction;
		}
	}
	if (bLogNotFound)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Can't find AbilityInputAction for InputTag [%s], OnInputConfig [%s]"), *InputTag.ToString(), *GetNameSafe(this)));
	}
	return nullptr;
}
