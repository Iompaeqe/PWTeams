// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "Core/PWTeamSetupActor.h"

#include "Core/PWTeamsSubsystem.h"
#include "Engine/World.h"

void APWTeamSetupActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	const UWorld* World = GetWorld();
	if (World && World->IsGameWorld())
		if (UPWTeamsSubsystem* Teams = World->GetSubsystem<UPWTeamsSubsystem>())
			Teams->ApplyTeamSetup(Setup);
}
