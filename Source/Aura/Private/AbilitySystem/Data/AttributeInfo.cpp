// Copyright Ivy


#include "AbilitySystem/Data/AttributeInfo.h"


// UAttributeInfo 类的成员函数实现，根据属性标签查找对应的属性信息
FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound)
{

	// 遍历属性信息数组(遍历AttributeInformation数组中的每一个元素，并在每次循环中将当前元素赋值给Info变量。)
	//const FAuraAttributeInfo& Info：在每次循环中，Info是对AttributeInformation数组中当前元素的引用。使用const表示在循环中不会修改Info的内容。
	for (const FAuraAttributeInfo& Info : AttributeInformation)
	{

		// 如果找到与传入标签完全匹配的属性信息(MatchesTagExact是Unreal Engine中用于比较两个FGameplayTag（游戏性标签）是否完全相等的方法。)
		if (Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			// 返回匹配的属性信息
			return Info;
		}
		return FAuraAttributeInfo();//这行自己加的
	}

	// 如果没有找到匹配的属性信息，并且需要打印日志
	if (bLogNotFound)
	{
		// 在屏幕上添加调试信息，显示未找到的标签和所属对象名称
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Can't find Info for AttributeTag [%s] on AttributeInfo [%s]."), *AttributeTag.ToString(), *GetNameSafe(this)));
	}

	// 如果未找到匹配的属性信息，返回一个空的 FAuraAttributeInfo 默认值
	return FAuraAttributeInfo();
}
