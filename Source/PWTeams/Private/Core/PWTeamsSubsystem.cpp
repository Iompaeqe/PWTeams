// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "Core/PWTeamsSubsystem.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"

UPWTeamsSubsystem* UPWTeamsSubsystem::Get(const UObject* WorldContext)
{
	const UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull) : nullptr;
	return World ? World->GetSubsystem<UPWTeamsSubsystem>() : nullptr;
}

bool UPWTeamsSubsystem::HasAuthority() const
{
	const UWorld* World = GetWorld();
	return World && World->GetNetMode() != NM_Client;
}

FPWTeamParticipant* UPWTeamsSubsystem::FindParticipant(const FPWTeamParticipantHandle Participant)
{
	return Participant.IsValid() ? Participants.FindByPredicate([&](const FPWTeamParticipant& Each) { return Each.Handle == Participant; }) : nullptr;
}

const FPWTeamParticipant* UPWTeamsSubsystem::FindParticipant(const FPWTeamParticipantHandle Participant) const
{
	return Participant.IsValid() ? Participants.FindByPredicate([&](const FPWTeamParticipant& Each) { return Each.Handle == Participant; }) : nullptr;
}

FPWTeamParticipantHandle UPWTeamsSubsystem::RegisterParticipant(const FName Name, const EPWTeamParticipantType Type, const FName Faction, APlayerState* PlayerState)
{
	if (!HasAuthority())
		return FPWTeamParticipantHandle();

	FPWTeamParticipant& Participant = Participants.AddDefaulted_GetRef();
	Participant.Handle.Id = NextParticipantId++;
	Participant.Name = Name;
	Participant.Type = Type;
	Participant.PlayerState = PlayerState;

	const FPWTeamParticipantHandle Handle = Participant.Handle;
	OnParticipantRegistered.Broadcast(Participant);

	if (!Faction.IsNone())
		AssignParticipantToFaction(Handle, Faction);
	return Handle;
}

bool UPWTeamsSubsystem::UnregisterParticipant(const FPWTeamParticipantHandle Participant)
{
	if (!HasAuthority())
		return false;

	const int32 Index = Participants.IndexOfByPredicate([&](const FPWTeamParticipant& Each) { return Each.Handle == Participant; });
	if (!Participant.IsValid() || Index == INDEX_NONE)
		return false;

	const FPWTeamParticipant Removed = Participants[Index];
	Participants.RemoveAt(Index);
	OnParticipantUnregistered.Broadcast(Removed);
	return true;
}

bool UPWTeamsSubsystem::GetParticipant(const FPWTeamParticipantHandle Participant, FPWTeamParticipant& OutParticipant) const
{
	const FPWTeamParticipant* Found = FindParticipant(Participant);
	if (Found)
		OutParticipant = *Found;
	return Found != nullptr;
}

FPWTeamParticipantHandle UPWTeamsSubsystem::FindParticipantForPlayer(const APlayerState* PlayerState) const
{
	if (PlayerState)
		for (const FPWTeamParticipant& Each : Participants)
			if (Each.PlayerState == PlayerState)
				return Each.Handle;
	return FPWTeamParticipantHandle();
}

bool UPWTeamsSubsystem::CreateFaction(const FName Name, const bool bNeutral)
{
	FPWTeamFaction Existing;
	if (!HasAuthority() || Name.IsNone() || GetFaction(Name, Existing))
		return false;

	FPWTeamFaction& Faction = Factions.AddDefaulted_GetRef();
	Faction.Name = Name;
	Faction.bNeutral = bNeutral;
	OnFactionCreated.Broadcast(Faction);
	return true;
}

bool UPWTeamsSubsystem::DestroyFaction(const FName Name)
{
	if (!HasAuthority())
		return false;

	const int32 Index = Factions.IndexOfByPredicate([&](const FPWTeamFaction& Each) { return Each.Name == Name; });
	if (Name.IsNone() || Index == INDEX_NONE)
		return false;

	for (FPWTeamParticipant& Each : Participants)
		if (Each.Faction == Name)
			SetParticipantFaction(Each, NAME_None);

	const FPWTeamFaction Removed = Factions[Index];
	Factions.RemoveAt(Index);
	OnFactionDestroyed.Broadcast(Removed);
	return true;
}

bool UPWTeamsSubsystem::GetFaction(const FName Name, FPWTeamFaction& OutFaction) const
{
	const FPWTeamFaction* Found = Name.IsNone() ? nullptr : Factions.FindByPredicate([&](const FPWTeamFaction& Each) { return Each.Name == Name; });
	if (Found)
		OutFaction = *Found;
	return Found != nullptr;
}

TArray<FPWTeamParticipantHandle> UPWTeamsSubsystem::GetFactionParticipants(const FName Faction) const
{
	TArray<FPWTeamParticipantHandle> Members;
	if (!Faction.IsNone())
		for (const FPWTeamParticipant& Each : Participants)
			if (Each.Faction == Faction)
				Members.Add(Each.Handle);
	return Members;
}

bool UPWTeamsSubsystem::AssignParticipantToFaction(const FPWTeamParticipantHandle Participant, const FName Faction)
{
	FPWTeamParticipant* Found = HasAuthority() ? FindParticipant(Participant) : nullptr;
	if (!Found || Faction.IsNone())
		return false;

	CreateFaction(Faction);
	// Creating a faction tells listeners, who may register participants and move the array.
	Found = FindParticipant(Participant);
	if (Found)
		SetParticipantFaction(*Found, Faction);
	return Found != nullptr;
}

bool UPWTeamsSubsystem::RemoveParticipantFromFaction(const FPWTeamParticipantHandle Participant)
{
	FPWTeamParticipant* Found = HasAuthority() ? FindParticipant(Participant) : nullptr;
	if (!Found)
		return false;

	SetParticipantFaction(*Found, NAME_None);
	return true;
}

FName UPWTeamsSubsystem::GetParticipantFaction(const FPWTeamParticipantHandle Participant) const
{
	const FPWTeamParticipant* Found = FindParticipant(Participant);
	return Found ? Found->Faction : NAME_None;
}

void UPWTeamsSubsystem::SetParticipantFaction(FPWTeamParticipant& Participant, const FName NewFaction)
{
	if (Participant.Faction == NewFaction)
		return;

	const FName OldFaction = Participant.Faction;
	const FPWTeamParticipantHandle Handle = Participant.Handle;
	Participant.Faction = NewFaction;
	OnParticipantFactionChanged.Broadcast(Handle, OldFaction, NewFaction);
}
