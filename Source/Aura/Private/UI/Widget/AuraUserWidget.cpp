// Copyright Ivy


#include "UI/Widget/AuraUserWidget.h"

void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	// 将传入的控制器对象（InWidgetController）赋值给 WidgetController 成员变量。
	// 这样，UAuraUserWidget 实例就可以使用这个控制器对象了。
	WidgetController = InWidgetController;

	// 调用 WidgetControllerSet 函数。这个函数是一个可以在蓝图中被重写的事件函数。
	// 在这里调用它，是为了通知可能关心 WidgetController 被设置这一事件的其他部分。
	// 注意：如果在 C++ 中没有重写这个函数，并且没有在蓝图中重写它，那么调用这个函数将不会有任何效果。
    WidgetControllerSet();
}
