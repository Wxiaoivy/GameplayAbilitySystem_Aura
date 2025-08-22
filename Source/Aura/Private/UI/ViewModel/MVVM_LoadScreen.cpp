// Copyright Ivy


#include "UI/ViewModel/MVVM_LoadScreen.h"

UMVVM_LoadScreen::UMVVM_LoadScreen()//AI
{
	LoadScreenName = TEXT("Default Load Screen");//AI
}

void UMVVM_LoadScreen::SetLoadScreenName(const FString& NewName)//AI
{
	if (LoadScreenName == NewName)//AI
		return;//AI

	LoadScreenName = NewName;//AI

	// UE5.4 ����ȷ��ʽ - ʹ����򵥵Ĺ㲥����
	BroadcastFieldValueChanged(FFieldNotificationClassDescriptor::LoadScreenName);//AI
}