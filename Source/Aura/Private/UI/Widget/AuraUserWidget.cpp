// Copyright Ivy


#include "UI/Widget/AuraUserWidget.h"

void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	// ������Ŀ���������InWidgetController����ֵ�� WidgetController ��Ա������
	// ������UAuraUserWidget ʵ���Ϳ���ʹ����������������ˡ�
	WidgetController = InWidgetController;

	// ���� WidgetControllerSet ���������������һ����������ͼ�б���д���¼�������
	// ���������������Ϊ��֪ͨ���ܹ��� WidgetController ��������һ�¼����������֡�
	// ע�⣺����� C++ ��û����д�������������û������ͼ����д������ô��������������������κ�Ч����
    WidgetControllerSet();
}
