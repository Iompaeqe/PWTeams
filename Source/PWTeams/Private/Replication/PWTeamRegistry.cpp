// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "Replication/PWTeamRegistry.h"

#include "Core/PWTeamsSubsystem.h"
#include "Net/UnrealNetwork.h"

APWTeamRegistry::APWTeamRegistry()
{
	bReplicates = true;
	bAlwaysRelevant = true;
	NetPriority = 3.f;
}

void APWTeamRegistry::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APWTeamRegistry, Participants);
	DOREPLIFETIME(APWTeamRegistry, Factions);
	DOREPLIFETIME(APWTeamRegistry, DefaultAttitude);
}

void APWTeamRegistry::OnRep_State()
{
	if (UPWTeamsSubsystem* Teams = UPWTeamsSubsystem::Get(this))
		Teams->ApplyReplicatedState(*this);
}
