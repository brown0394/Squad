// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadPlayerController.h"
#include "../SquadPlayerWidget.h"
#include "AICharacter.h"
#include "SquadCharacter.h"

void ASquadPlayerController::BeginPlay() {
	Super::BeginPlay();

	if (SBWidgetClass) {
		sbWidget = CreateWidget<USquadPlayerWidget>(this, SBWidgetClass);
		sbWidget->AddToViewport();
	}
}

void ASquadPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	if (sbWidget) {
		sbWidget->RemoveFromParent();
		sbWidget = nullptr;
	}
	if (SBWidgetClass) {
		SBWidgetClass = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}

void ASquadPlayerController::InitSquad() {
	FString str;
	FActorSpawnParameters ActorSpawnParams;
	TObjectPtr<ASquadCharacter> OwningPlayer = Cast<ASquadCharacter>(Owner);
	for (int i = 1; i <= SquadNames.Num(); ++i) {
		TObjectPtr<AAICharacter> Spawned = Cast<AAICharacter>(GetWorld()->SpawnActor<AAICharacter>(SquadClassToSpawn,
			OwningPlayer->GetActorLocation() + (OwningPlayer->GetActorRightVector() * 50 * i), OwningPlayer->GetActorRotation()));
		SquadMembers.Add(Spawned);
		Spawned->SetGenericTeamId(OwningPlayer->GetGenericTeamId());
		Spawned->SpawnDefaultController();
		
		str.AppendChar('0' + i);
		str += ". ";
		str += SquadNames[i - 1];
		str += "     ";
	}
	str.AppendChar('0' + SquadNames.Num() + 1);
	str += ". All";
	sbWidget->InitializeText(str);
}

void ASquadPlayerController::MakeOrderUI(bool on) {
	sbWidget->TextOrderToOnOff(on);
}

void ASquadPlayerController::OrderListOnOff(bool on) {
	sbWidget->TextOrderListOnOff(on);
}

void ASquadPlayerController::CrosshairOnOff(bool on) {
	sbWidget->CorsshairOnOff(on);
}

void ASquadPlayerController::SetMemberTarget(TObjectPtr<AActor> target, int idx) {
	IGenericTeamAgentInterface* ITargetTeamAgent = Cast<IGenericTeamAgentInterface>(target);
	if (ITargetTeamAgent != nullptr && ITargetTeamAgent->GetTeamAttitudeTowards(*this) == ETeamAttitude::Hostile) {
		if (SquadMembers.Num() <= idx) {
			for (auto member : SquadMembers) {
				
			}
		}
	}
}