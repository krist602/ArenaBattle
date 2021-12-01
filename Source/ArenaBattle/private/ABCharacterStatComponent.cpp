// Fill out your copyright notice in the Description page of Project Settings.

#include "ABCharacterStatComponent.h"
#include "ABGameInstance.h"


// Sets default values for this component's properties
UABCharacterStatComponent::UABCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	Level = 1; //�����ڿ��� Level = 1�� �ʱ�ȭ����
	// ...
}


// Called when the game starts
void UABCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UABCharacterStatComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetNewLevel(Level); 
}

void UABCharacterStatComponent::SetNewLevel(int32 NewLevel)
{
	auto ABGameInstance = Cast<UABGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	ABCHECK(nullptr != ABGameInstance); // ������ �� �Ǿ��ٸ�
	CurrentStatData = ABGameInstance->GetABCharacterData(NewLevel); //CurrentStatData�� ABGameInstance���� NewLevel�� ���� �����͸� �����´�.
	if (nullptr != CurrentStatData)
	{
		Level = NewLevel;
		SetHP(CurrentStatData->MaxHP);
	}
	else
	{
		ABLOG(Error, TEXT("Level (%d) data doesn't exist"), NewLevel);
	}
}


// Called every frame  // �ż��� ȣ��Ǵ� Tick �Լ��� ���� StatComponent�� �ǹ̿����� �ʿ䰡 ����.
/*
void UABCharacterStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
*/

void UABCharacterStatComponent::SetDamage(float NewDamage)
{
	ABCHECK(nullptr != CurrentStatData);
	//Clamp�Լ� (auto value, auto min, auto max); �������� �����ϰ�, value���� min�� max ������ ��� �״�� ����ϰ� �� ���� ��� ����� ������ ����Ѵ�.
	SetHP(FMath::Clamp<float>(CurrentHP - NewDamage, 0.0f, CurrentStatData->MaxHP));
}

void UABCharacterStatComponent::SetHP(float NewHP)
{
	CurrentHP = NewHP;
	OnHPChanged.Broadcast();
	// KINDA_SMALL_NUMBER �� �ణ�� ������ ���� �� �ֱ⿡ �����ϴ� ��ũ��. 
	// 0.0001�� ���� ���̴�. CurrentHP<0 �̶�� �ߴٰ� ���������� 0�� �ȵ����� ���峯 �� �ֱ⶧���� ������ �� �� �ִ� ���� ������ �۵��ϱ� ���� ���.
	// ��� CurrentHP �� KINDA_SMALL_NUMBER���� �۾����� ���� �Ͱ� ���������̴�.
	if (CurrentHP < KINDA_SMALL_NUMBER) 
	{
		CurrentHP = 0.0f;
		OnHPIsZero.Broadcast();
	}
}

float UABCharacterStatComponent::GetAttack()
{
	ABCHECK(nullptr != CurrentStatData, 0.0f);

	return CurrentStatData->Attack;
}

float UABCharacterStatComponent::GetHPRatio()
{
	ABCHECK(nullptr != CurrentStatData, 0.0f);

	return (CurrentStatData->MaxHP < KINDA_SMALL_NUMBER) ? 0.0f : (CurrentHP / CurrentStatData->MaxHP);
}

int32 UABCharacterStatComponent::GetDropExp() const
{
	return CurrentStatData->DropExp;
}
