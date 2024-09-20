// Copyright Epic Games, Inc. All Rights Reserved.

#include "SquadGameMode.h"
#include "character/SquadCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASquadGameMode::ASquadGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
