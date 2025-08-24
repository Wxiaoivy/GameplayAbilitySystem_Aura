// Copyright Ivy


#include "UI/HUD/LoadScreenHUD.h"
#include "UI/Widget/LoadScreenWidget.h"
#include "UI/ViewModel/MVVM_LoadScreen.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	// ����������Ļ��ViewModel����ͼģ�ͣ�
	// NewObject ��̬����һ��UMVVM_LoadScreen���͵Ķ���
	// LoadScreenViewModelClass ��Ԥ���ViewModel����ͼ��C++��
	LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this, LoadScreenViewModelClass);
	// ��ʼ��ViewModel�еļ��ز�λ
    // ��������������ò�λ��������ʼ״̬��
	LoadScreenViewModel->InitializeLoadSlots();


	// ����������Ļ��Widget��UI���棩
    // CreateWidget ��̬����һ��ULoadScreenWidget���͵�UI���
	LoadScreenWidget = CreateWidget<ULoadScreenWidget>(GetWorld(), LoadScreenWidgetClass);
	// ��Widget��ӵ��ӿڣ�ʹ��ɼ�
	LoadScreenWidget->AddToViewport();	
	// ����Widget����ͼ��ʼ������
	// ����������ܰ���UIԪ�صĳ�ʼ���á��������ŵ�
	LoadScreenWidget->BluePrintInitializeWidget();


	// �������ݵ�ViewModel
	// ����������ܴӴ浵ϵͳ��ȡ���ݲ�����ViewModel״̬
	LoadScreenViewModel->LoadData();




	//ִ��˳����Ҫ�ԣ�
	//	�ȴ���ViewModel�ͳ�ʼ����λ

	//	Ȼ�󴴽�����ʾUI

	//	���������ݣ�����UI���£�

	//	�����ʼ������ȷ����MVVM�ܹ�����ȷ���ú�����������
}
