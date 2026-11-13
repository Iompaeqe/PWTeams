// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "PWTeamComponent.h"

#include "Net/UnrealNetwork.h"


void UPWTeamComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UPWTeamComponent, TeamId);
}
