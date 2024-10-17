// Copyright Epic Games, Inc. All Rights Reserved.

#include "SquadGameMode.h"
#include "character/SquadCharacter.h"
#include "GenericTeamAgentInterface.h"
#include "UObject/ConstructorHelpers.h"

ASquadGameMode::ASquadGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	FGenericTeamId::SetAttitudeSolver([](FGenericTeamId A, FGenericTeamId B) -> ETeamAttitude::Type {
		if (!A.GetId() || !B.GetId()) return ETeamAttitude::Neutral;
		if (A.GetId() == B.GetId()) return ETeamAttitude::Friendly;
		return ETeamAttitude::Hostile;
	});
}
