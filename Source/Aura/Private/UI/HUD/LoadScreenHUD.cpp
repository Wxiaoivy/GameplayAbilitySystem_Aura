// Copyright Ivy


#include "UI/HUD/LoadScreenHUD.h"
#include "UI/Widget/LoadScreenWidget.h"
#include "UI/ViewModel/MVVM_LoadScreen.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	// 创建加载屏幕的ViewModel（视图模型）
	// NewObject 动态创建一个UMVVM_LoadScreen类型的对象
	// LoadScreenViewModelClass 是预设的ViewModel类蓝图或C++类
	LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this, LoadScreenViewModelClass);
	// 初始化ViewModel中的加载槽位
    // 这个方法可能设置槽位数量、初始状态等
	LoadScreenViewModel->InitializeLoadSlots();


	// 创建加载屏幕的Widget（UI界面）
    // CreateWidget 动态创建一个ULoadScreenWidget类型的UI组件
	LoadScreenWidget = CreateWidget<ULoadScreenWidget>(GetWorld(), LoadScreenWidgetClass);
	// 将Widget添加到视口，使其可见
	LoadScreenWidget->AddToViewport();	
	// 调用Widget的蓝图初始化函数
	// 这个方法可能包含UI元素的初始设置、动画播放等
	LoadScreenWidget->BluePrintInitializeWidget();


	// 加载数据到ViewModel
	// 这个方法可能从存档系统读取数据并更新ViewModel状态
	LoadScreenViewModel->LoadData();




	//执行顺序重要性：
	//	先创建ViewModel和初始化槽位

	//	然后创建和显示UI

	//	最后加载数据（触发UI更新）

	//	这个初始化过程确保了MVVM架构的正确设置和数据流动。
}
