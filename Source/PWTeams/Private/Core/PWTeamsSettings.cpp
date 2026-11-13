// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "Core/PWTeamsSettings.h"

#include "Core/PWTeamSetup.h"

TArray<FName> UPWTeamsSettings::GetFactionNames()
{
	TArray<FName> Names;
	if (const UPWTeamSetup* Setup = Get()->TeamSetup.LoadSynchronous())
		for (const FPWTeamFaction& Faction : Setup->Factions)
			if (!Faction.Name.IsNone())
				Names.AddUnique(Faction.Name);
	return Names;
}
