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
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_SquadPlayerController"));
	if (PlayerControllerBPClass.Class != NULL) {
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}

}

ETeamAttitude::Type AttitudeSolver(FGenericTeamId A, FGenericTeamId B) {
	if (!A.GetId() || !B.GetId() || A.GetId() == FGenericTeamId::NoTeam || B.GetId() == FGenericTeamId::NoTeam) return ETeamAttitude::Neutral;
	if (A.GetId() == B.GetId()) return ETeamAttitude::Friendly;
	return ETeamAttitude::Hostile;
}

void ASquadGameMode::BeginPlay() {
	Super::BeginPlay();
	FGenericTeamId::SetAttitudeSolver(AttitudeSolver);
	
}

