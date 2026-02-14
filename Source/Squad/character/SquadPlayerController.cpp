// Fill out your copyright notice in the Description page of Project Settings.


#include "SquadPlayerController.h"
#include "../SquadPlayerWidget.h"
#include "AICharacter.h"
#include "SquadCharacter.h"
#include "SquadAIController.h"
#include "HealthComponent.h"

constexpr float forwardDistance = 200;
constexpr float sideDistance = 20;

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
		ASquadAIController* memberAiController = Cast<ASquadAIController>(Spawned->GetController());
		if (memberAiController) {
			memberAiController->SquadLeader = Cast<AActor>(OwningPlayer);
		}
		
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

void ASquadPlayerController::SetMemberTarget(TObjectPtr<AActor> target, int memberIdx) {
	if (target->FindComponentByClass<UHealthComponent>() == nullptr)
		return;

	if (SquadMembers.Num() < memberIdx)
		return;

	if (SquadMembers.Num() == memberIdx)
	{
		for (TObjectPtr<class AAICharacter> member : SquadMembers) {
			ASquadAIController* memberAiController = Cast<ASquadAIController>(member->GetController());

			memberAiController->DesignateTarget(target);
		}
		return;
	}

	ASquadAIController* memberAiController = Cast<ASquadAIController>(SquadMembers[memberIdx]->GetController());
	memberAiController->DesignateTarget(target);
}

void ASquadPlayerController::SetMemberFollow(int memberIdx) {
	if (SquadMembers.Num() < memberIdx)
		return;

	if (SquadMembers.Num() == memberIdx) {

		float backwardDistance = -forwardDistance;

		// Order ALL squad members
		int32 SquadSize = SquadMembers.Num();
		for (int32 i = 0; i < SquadSize; ++i) {
			ASquadAIController* memberAiController = Cast<ASquadAIController>(SquadMembers[i]->GetController());
			FVector offset( ( backwardDistance * ( ( i / 2 ) + 1 ) ), sideDistance * ( -1 * ( i % 2 ) ), 0.f);
			memberAiController->FollowSquadLeader(offset);
		}
		return;
	}

	ASquadAIController* memberAiController = Cast<ASquadAIController>(SquadMembers[memberIdx]->GetController());
	memberAiController->FollowSquadLeader(FVector( -forwardDistance, 0, 0  ) );
}

void ASquadPlayerController::SetMemberMoveToPosition(FVector Position, int memberIdx) {
	if (SquadMembers.Num() < memberIdx)
		return;

	if (SquadMembers.Num() == memberIdx) {
		for (TObjectPtr<class AAICharacter> member : SquadMembers) {
			ASquadAIController* memberAiController = Cast<ASquadAIController>(member->GetController());
			memberAiController->MoveToPosition(Position);
		}
		return;
	}

	ASquadAIController* memberAiController = Cast<ASquadAIController>(SquadMembers[memberIdx]->GetController());
	memberAiController->MoveToPosition(Position);
}

void ASquadPlayerController::SetMemberFreeWill(int memberIdx) {
	if (SquadMembers.Num() < memberIdx) return;

	if (SquadMembers.Num() == memberIdx) {
		for (TObjectPtr<class AAICharacter> member : SquadMembers) {
			ASquadAIController* memberAiController = Cast<ASquadAIController>(member->GetController());
			memberAiController->FreeWill();
		}
		return;
	}

	ASquadAIController* memberAiController = Cast<ASquadAIController>(SquadMembers[memberIdx]->GetController());
	memberAiController->FreeWill();
}