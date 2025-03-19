// Copyright Ivy

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "Input/AuraInputConfig.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	//模板函数，参考EnhancedInputComponent->BindAction函数(这个BindAction中的模板类是可以带参数一起传入的）
	   //template<class UserClass, typename... VarTypes>																																					
	  // FEnhancedInputActionEventBinding& BindAction(const UInputAction* Action, ETriggerEvent TriggerEvent, UserClass* Object, typename HANDLER_SIG::template TMethodPtr< UserClass, VarTypes... > Func, VarTypes... Vars) 
 

	//这里InputConfig里面有个数组AbilityInputActions,数组里存的是结构体包含InputAction和InputTag,我们自己写的这个模板函数里有所有绑定需要的参数，是通用的
	//回调函数中我们通过传递Tag参数来实现逻辑
    template<class Userclass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const UAuraInputConfig* InputConfig, Userclass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
	
};

template<class Userclass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
inline void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig, Userclass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{

	check(InputConfig);

	for (const FAuraInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				//PressedFunc就是回调函数，Action.InputTag就是该函数的参数，这三个回调函数在AuraPlayerController声明定义，
		
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}
			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}
			if (HeldFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}
		}
	}




}
