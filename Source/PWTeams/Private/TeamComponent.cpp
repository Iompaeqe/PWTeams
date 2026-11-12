// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "TeamComponent.h"

#include "Net/UnrealNetwork.h"


void UTeamComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UTeamComponent, TeamId);
}
