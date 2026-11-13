// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Core/PWTeamsTypes.h"
#include "Relationships/PWTeamRelationship.h"
#include "PWTeamSetup.generated.h"

class UPWTeamRelationshipProvider;

// The team rules of one game: its factions, how different factions regard each other, and any rules of the game's own. Nothing here is project-wide. A world gets its rules by having a setup applied to it, from a game mode or from a PW Team Setup actor placed in the level, so one project can hold a free-for-all arena, a two-sided battle and a diplomacy map side by side.
UCLASS(BlueprintType)
class PWTEAMS_API UPWTeamSetup : public UDataAsset
{
	GENERATED_BODY()

public:
	// The factions the game starts with, under whatever names it likes. More can be created while the game runs.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Teams", meta=(TitleProperty="Name"))
	TArray<FPWTeamFaction> Factions;

	// How two different factions regard each other when neither is neutral and no provider says otherwise.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Teams")
	EPWTeamAttitude DefaultAttitude = EPWTeamAttitude::Hostile;

	// The game's own relationship rules, run in this order after the default ones.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Teams")
	TArray<TSubclassOf<UPWTeamRelationshipProvider>> RelationshipProviders;

	// The faction names of every team setup in the project, for dropdowns on placed actors.
	static TArray<FName> GetAllFactionNames();
};
