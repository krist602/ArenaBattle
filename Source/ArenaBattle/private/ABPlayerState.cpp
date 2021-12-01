// Fill out your copyright notice in the Description page of Project Settings.

#include "ABPlayerState.h"
#include "ABGameInstance.h"
#include "ABSaveGame.h"

AABPlayerState::AABPlayerState()
{
	CharacterLevel = 1;
	GameScore = 0;
	GameHighScore = 0;
	Exp = 0;
	SaveSlotName = TEXT("Player1");
}

int32 AABPlayerState::GetGameScore() const
{
	return GameScore;
}

int32 AABPlayerState::GetGameHighScore() const
{
	return GameHighScore;
}

int32 AABPlayerState::GetCharacterLevel() const
{
	return CharacterLevel;
}

void AABPlayerState::InitPlayerData()
{
	auto ABSaveGame = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0)); //ABSaveGame ������ ���� ����. �� ������ SaveSlotName�� UserIndex�� ���ؼ� ������ �����´�. ����� 1���� ������ 0�ε�
	if (nullptr == ABSaveGame)
	{
		ABSaveGame = GetMutableDefault<UABSaveGame>();
	}
	//�Ʒ� �κ��� SaveGame�� ������ �����ͼ� ���� ���ӿ� �����ִ� �κ�.
	SetPlayerName(ABSaveGame->PlayerName); 
	SetCharacterLevel(ABSaveGame->Level);
	GameScore = 0;
	GameHighScore = ABSaveGame->HighScore;
	Exp = ABSaveGame->Exp;
	SavePlayerData();
}

void AABPlayerState::SavePlayerData()
{
	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>(); //���� ��ü�� �����ؼ�
	NewPlayerData->PlayerName = GetPlayerName(); //���� ������ �����͸� ��ü�� �־��ְ�
	NewPlayerData->Level = CharacterLevel;
	NewPlayerData->Exp = Exp;
	NewPlayerData->HighScore = GameHighScore;

	if (!UGameplayStatics::SaveGameToSlot(NewPlayerData, SaveSlotName, 0)) //Save�Ѵ�. Save�� �ȵǸ� ���� ����
	{
		ABLOG(Error, TEXT("SaveGame Error!"));
	}
}

float AABPlayerState::GetExpRatio() const
{
	if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER) //�������� ������ 0?
		return 0.0f;

	float Result = (float)Exp / (float)CurrentStatData->NextExp;
	ABLOG(Warning, TEXT("Ratio : %f, Current : %d, Next : %d"), Result, Exp, CurrentStatData->NextExp);
	return Result; //����ġ ������ return���ش�.
}

bool AABPlayerState::AddExp(int32 IncomeExp)
{
	if (CurrentStatData->NextExp == -1)
		return false;

	bool DidLevelUp = false;
	Exp = Exp + IncomeExp;
	if (Exp >= CurrentStatData->NextExp) //����ġ �ѷ��� �������� �ʿ��� ��ŭ���� ������
	{
		Exp -= CurrentStatData->NextExp; //�ѷ����� �������� �ʿ��� ���� ���ְ�
		SetCharacterLevel(CharacterLevel + 1); //������ �÷��ְ�
		DidLevelUp = true; //�������� �ߴٰ� ǥ�����ش�.
	}

	OnPlayerStateChanged.Broadcast();
	SavePlayerData(); //����ġ�� ������ ����� �ٷ� �ݿ�
	return DidLevelUp;
}

void AABPlayerState::AddGameScore()
{
	GameScore++;
	if (GameScore >= GameHighScore)
	{
		GameHighScore = GameScore;
	}
	OnPlayerStateChanged.Broadcast();
	SavePlayerData(); //������ ������ ����� �ٷ� �ݿ�
}

void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel)
{
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(nullptr != ABGameInstance);

	CurrentStatData = ABGameInstance->GetABCharacterData(NewCharacterLevel);
	ABCHECK(nullptr != CurrentStatData);

	CharacterLevel = NewCharacterLevel;
}