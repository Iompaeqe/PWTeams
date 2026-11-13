// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Core/PWTeamsTypes.h"
#include "Relationships/PWTeamRelationship.h"
#include "PWTeamRegistry.generated.h"

// Carries the participants and factions from the server to every client. The teams subsystem spawns one on the server by itself; nothing needs to be placed or added to the game state.
UCLASS(NotBlueprintable, NotPlaceable)
class PWTEAMS_API APWTeamRegistry : public AInfo
{
	GENERATED_BODY()

public:
	APWTeamRegistry();

	UPROPERTY(ReplicatedUsing=OnRep_State)
	TArray<FPWTeamParticipant> Participants;

	UPROPERTY(ReplicatedUsing=OnRep_State)
	TArray<FPWTeamFaction> Factions;

	UPROPERTY(ReplicatedUsing=OnRep_State)
	EPWTeamAttitude DefaultAttitude = EPWTeamAttitude::Hostile;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UFUNCTION()
	void OnRep_State();
};
