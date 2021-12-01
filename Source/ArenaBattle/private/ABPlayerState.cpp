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
	auto ABSaveGame = Cast<UABSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0)); //ABSaveGame 형식의 변수 선언. 그 변수엔 SaveSlotName과 UserIndex를 비교해서 정보를 가져온다. 현재는 1개기 때문에 0인듯
	if (nullptr == ABSaveGame)
	{
		ABSaveGame = GetMutableDefault<UABSaveGame>();
	}
	//아래 부분은 SaveGame의 정보를 가져와서 현재 게임에 입혀주는 부분.
	SetPlayerName(ABSaveGame->PlayerName); 
	SetCharacterLevel(ABSaveGame->Level);
	GameScore = 0;
	GameHighScore = ABSaveGame->HighScore;
	Exp = ABSaveGame->Exp;
	SavePlayerData();
}

void AABPlayerState::SavePlayerData()
{
	UABSaveGame* NewPlayerData = NewObject<UABSaveGame>(); //새로 객체를 선언해서
	NewPlayerData->PlayerName = GetPlayerName(); //현재 게임의 데이터를 객체에 넣어주고
	NewPlayerData->Level = CharacterLevel;
	NewPlayerData->Exp = Exp;
	NewPlayerData->HighScore = GameHighScore;

	if (!UGameplayStatics::SaveGameToSlot(NewPlayerData, SaveSlotName, 0)) //Save한다. Save가 안되면 오류 선언
	{
		ABLOG(Error, TEXT("SaveGame Error!"));
	}
}

float AABPlayerState::GetExpRatio() const
{
	if (CurrentStatData->NextExp <= KINDA_SMALL_NUMBER) //레벨업에 근접시 0?
		return 0.0f;

	float Result = (float)Exp / (float)CurrentStatData->NextExp;
	ABLOG(Warning, TEXT("Ratio : %f, Current : %d, Next : %d"), Result, Exp, CurrentStatData->NextExp);
	return Result; //경험치 비율을 return해준다.
}

bool AABPlayerState::AddExp(int32 IncomeExp)
{
	if (CurrentStatData->NextExp == -1)
		return false;

	bool DidLevelUp = false;
	Exp = Exp + IncomeExp;
	if (Exp >= CurrentStatData->NextExp) //경험치 총량이 레벨업에 필요한 만큼보다 많으면
	{
		Exp -= CurrentStatData->NextExp; //총량에서 레벨업이 필요한 양을 빼주고
		SetCharacterLevel(CharacterLevel + 1); //레벨을 올려주고
		DidLevelUp = true; //레벨업을 했다고 표시해준다.
	}

	OnPlayerStateChanged.Broadcast();
	SavePlayerData(); //경험치에 변동이 생기면 바로 반영
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
	SavePlayerData(); //점수에 변동이 생기면 바로 반영
}

void AABPlayerState::SetCharacterLevel(int32 NewCharacterLevel)
{
	auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(nullptr != ABGameInstance);

	CurrentStatData = ABGameInstance->GetABCharacterData(NewCharacterLevel);
	ABCHECK(nullptr != CurrentStatData);

	CharacterLevel = NewCharacterLevel;
}