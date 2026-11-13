// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Relationships/PWTeamRelationship.h"
#include "PWTeamsSettings.generated.h"

class UPWTeamRelationshipProvider;
class UPWTeamSetup;

UCLASS(config=Game, defaultconfig, meta=(DisplayName="PW Teams"))
class PWTEAMS_API UPWTeamsSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static const UPWTeamsSettings* Get() { return GetDefault<UPWTeamsSettings>(); }

	virtual FName GetCategoryName() const override { return TEXT("Game"); }

	// The factions the game starts with. Also fills the faction dropdown on team agents.
	UPROPERTY(config, EditAnywhere, Category="Factions")
	TSoftObjectPtr<UPWTeamSetup> TeamSetup;

	// Names of the factions in the team setup, for dropdowns.
	static TArray<FName> GetFactionNames();

	// How two different factions regard each other when neither is neutral and no provider says otherwise.
	UPROPERTY(config, EditAnywhere, Category="Relationships")
	EPWTeamAttitude DefaultAttitude = EPWTeamAttitude::Hostile;

	// Your game's own relationship rules, run in this order after the default ones.
	UPROPERTY(config, EditAnywhere, Category="Relationships")
	TArray<TSoftClassPtr<UPWTeamRelationshipProvider>> RelationshipProviders;
};
