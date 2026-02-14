// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SquadPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SQUAD_API ASquadPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class USquadPlayerWidget> SBWidgetClass;

	void MakeOrderUI(bool on);
	void OrderListOnOff(bool on);
	void CrosshairOnOff(bool on);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Squad)
	TArray<FString> SquadNames;
	UPROPERTY(EditDefaultsOnly, Category = Squad)
	TSubclassOf<class AAICharacter> SquadClassToSpawn;
	void InitSquad();

	void SetMemberTarget(TObjectPtr<AActor> target, int memberIdx);
	void SetMemberFollow(int memberIdx);
	void SetMemberMoveToPosition(FVector Position, int memberIdx);
	void SetMemberFreeWill(int memberIdx);
protected:
	virtual void BeginPlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	TArray<TObjectPtr<class AAICharacter>> SquadMembers;
	
private:
	UPROPERTY()
	TObjectPtr<class USquadPlayerWidget> sbWidget;
};
