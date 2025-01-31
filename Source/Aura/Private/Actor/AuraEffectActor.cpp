// Copyright Ivy


#include "Actor/AuraEffectActor.h"

// Sets default values
AAuraEffectActor::AAuraEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;


	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());


}

void AAuraEffectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	//TODO:Change this to Apply a Gameplay Effect, For now,Using const_cast as a Hack!
	if (IAbilitySystemInterface* ASCInterface = Cast< IAbilitySystemInterface>(OtherActor))
	{
		const UAuraAttributeSet* AuraAttributeSet=Cast< UAuraAttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UAuraAttributeSet::StaticClass()));
	
		//��Ϊ����AuraAttributeSet��const�ģ������������ܸı����ԣ����ǲ�Ӧ����ô���� һ�㲻����const_cast����ʦ����ὲ�÷���
		//�����������Ҫʹ�� const_cast ���޸Ĳ�Ӧ���޸ĵĶ�����Ӧ��Ѱ��һ�ַ������޸����Զ���Υ������ĳ����ԡ�
		// ͨ��������ζ������Ҫ�� UAuraAttributeSet �������һ���� const �ķ������޸Ľ���ֵ����AuraAttributeSet��const�ģ�
		UAuraAttributeSet* MutableAuraAttributeSet = const_cast<UAuraAttributeSet*>(AuraAttributeSet);
		MutableAuraAttributeSet->SetHealth(AuraAttributeSet->GetHealth() + 25.f);
		MutableAuraAttributeSet->SetMana(AuraAttributeSet->GetMana() + 25.f);
		Destroy();
	}

}

void AAuraEffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraEffectActor::OnOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AAuraEffectActor::EndOverlap);
}


