// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/PWTeamsTypes.h"
#include "PWTeamSetup.generated.h"

// The factions a game starts with, under whatever names it likes. Point the PW Teams project settings at one; the server creates these factions when a match begins, and placed actors can pick from them in a dropdown. More factions can still be created while the game runs.
UCLASS(BlueprintType)
class PWTEAMS_API UPWTeamSetup : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Teams", meta=(TitleProperty="Name"))
	TArray<FPWTeamFaction> Factions;
};
