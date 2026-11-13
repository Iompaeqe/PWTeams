// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "Core/PWTeamsSubsystem.h"

#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"
#include "Agents/PWTeamAgent.h"
#include "Agents/PWTeamAgentComponent.h"
#include "Core/PWTeamsSettings.h"
#include "Core/PWTeamSetup.h"
#include "Core/PWTeamsTags.h"
#include "Relationships/PWTeamRelationshipProvider.h"
#include "Replication/PWTeamRegistry.h"

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
	PushToRegistry();
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
	RefreshAgentsOf(Participant);
	PushToRegistry();
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
	PushToRegistry();
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
	PushToRegistry();
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
	RefreshAgentsOf(Handle);
	PushToRegistry();
}

void UPWTeamsSubsystem::RefreshAgentsOf(const FPWTeamParticipantHandle Participant)
{
	// Agents that inherit their faction follow their owner.
	const TArray<TWeakObjectPtr<UPWTeamAgentComponent>> Snapshot = Agents;
	for (const TWeakObjectPtr<UPWTeamAgentComponent>& Agent : Snapshot)
		if (Agent.IsValid() && Agent->GetOwnerParticipant() == Participant)
			Agent->RefreshFaction();
}

UPWTeamAgentComponent* UPWTeamsSubsystem::FindAgent(const AActor* Actor)
{
	if (!Actor)
		return nullptr;

	if (Actor->Implements<UPWTeamAgent>())
		if (UPWTeamAgentComponent* Agent = IPWTeamAgent::Execute_GetTeamAgent(Actor))
			return Agent;
	if (UPWTeamAgentComponent* Agent = Actor->FindComponentByClass<UPWTeamAgentComponent>())
		return Agent;

	const APawn* Pawn = Cast<APawn>(Actor);
	const APlayerState* PlayerState = Pawn ? Pawn->GetPlayerState() : nullptr;
	return PlayerState ? PlayerState->FindComponentByClass<UPWTeamAgentComponent>() : nullptr;
}

FPWTeamParticipantHandle UPWTeamsSubsystem::GetAgentOwner(const AActor* Actor)
{
	const UPWTeamAgentComponent* Agent = FindAgent(Actor);
	return Agent ? Agent->GetOwnerParticipant() : FPWTeamParticipantHandle();
}

FName UPWTeamsSubsystem::GetAgentFaction(const AActor* Actor)
{
	const UPWTeamAgentComponent* Agent = FindAgent(Actor);
	return Agent ? Agent->GetFaction() : NAME_None;
}

bool UPWTeamsSubsystem::SetAgentOwner(AActor* Actor, const FPWTeamParticipantHandle Participant)
{
	UPWTeamAgentComponent* Agent = FindAgent(Actor);
	if (Agent)
		Agent->SetOwnerParticipant(Participant);
	return Agent && Agent->GetOwnerParticipant() == Participant;
}

bool UPWTeamsSubsystem::SetAgentFaction(AActor* Actor, const FName Faction)
{
	UPWTeamAgentComponent* Agent = FindAgent(Actor);
	if (Agent)
		Agent->SetExplicitFaction(Faction);
	return Agent && Agent->GetFactionSource() == EPWTeamFactionSource::Explicit && Agent->GetFaction() == Faction;
}

bool UPWTeamsSubsystem::SetAgentFactionSource(AActor* Actor, const EPWTeamFactionSource Source)
{
	UPWTeamAgentComponent* Agent = FindAgent(Actor);
	if (Agent)
		Agent->SetFactionSource(Source);
	return Agent && Agent->GetFactionSource() == Source;
}

TArray<AActor*> UPWTeamsSubsystem::GetAgentsOwnedBy(const FPWTeamParticipantHandle Participant) const
{
	TArray<AActor*> Actors;
	if (Participant.IsValid())
		for (const TWeakObjectPtr<UPWTeamAgentComponent>& Agent : Agents)
			if (Agent.IsValid() && Agent->GetOwnerParticipant() == Participant)
				Actors.Add(Agent->GetOwner());
	return Actors;
}

TArray<AActor*> UPWTeamsSubsystem::GetAgentsInFaction(const FName Faction) const
{
	TArray<AActor*> Actors;
	if (!Faction.IsNone())
		for (const TWeakObjectPtr<UPWTeamAgentComponent>& Agent : Agents)
			if (Agent.IsValid() && Agent->GetFaction() == Faction)
				Actors.Add(Agent->GetOwner());
	return Actors;
}

void UPWTeamsSubsystem::RegisterAgent(UPWTeamAgentComponent* Agent)
{
	Agents.AddUnique(Agent);
}

void UPWTeamsSubsystem::UnregisterAgent(UPWTeamAgentComponent* Agent)
{
	Agents.Remove(Agent);
}

void UPWTeamsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UPWTeamsSettings* Settings = UPWTeamsSettings::Get();
	DefaultAttitude = Settings->DefaultAttitude;
	for (const TSoftClassPtr<UPWTeamRelationshipProvider>& ProviderClass : Settings->RelationshipProviders)
		AddRelationshipProvider(ProviderClass.LoadSynchronous());
}

UPWTeamRelationshipProvider* UPWTeamsSubsystem::AddRelationshipProvider(const TSubclassOf<UPWTeamRelationshipProvider> ProviderClass)
{
	if (!ProviderClass || ProviderClass->HasAnyClassFlags(CLASS_Abstract))
		return nullptr;

	UPWTeamRelationshipProvider* Provider = NewObject<UPWTeamRelationshipProvider>(this, ProviderClass);
	Providers.Add(Provider);
	return Provider;
}

void UPWTeamsSubsystem::RemoveRelationshipProvider(UPWTeamRelationshipProvider* Provider)
{
	Providers.Remove(Provider);
}

FPWTeamIdentity UPWTeamsSubsystem::GetIdentity(const AActor* Actor)
{
	FPWTeamIdentity Identity;
	Identity.Actor = Actor;

	const UPWTeamAgentComponent* Agent = FindAgent(Actor);
	if (!Agent)
		return Identity;

	Identity.Owner = Agent->GetOwnerParticipant();
	Identity.Faction = Agent->GetFaction();
	Identity.bPrimary = Agent->IsPrimary();
	Identity.AgentTags = Agent->GetAgentTags();

	FPWTeamFaction Faction;
	if (const UPWTeamsSubsystem* Teams = Get(Actor); Teams && Teams->GetFaction(Identity.Faction, Faction))
		Identity.bNeutralFaction = Faction.bNeutral;
	return Identity;
}

FPWTeamIdentity UPWTeamsSubsystem::GetParticipantIdentity(const FPWTeamParticipantHandle Participant) const
{
	FPWTeamIdentity Identity;
	Identity.Owner = Participant;
	Identity.Faction = GetParticipantFaction(Participant);

	FPWTeamFaction Faction;
	if (GetFaction(Identity.Faction, Faction))
		Identity.bNeutralFaction = Faction.bNeutral;
	return Identity;
}

FPWTeamRelationship UPWTeamsSubsystem::GetRelationship(const FPWTeamIdentity& Source, const FPWTeamIdentity& Target) const
{
	FPWTeamRelationship Relationship;
	Relationship.bSameActor = Source.Actor && Source.Actor == Target.Actor;
	Relationship.bSameOwner = Source.Owner.IsValid() && Source.Owner == Target.Owner;
	Relationship.bSameFaction = !Source.Faction.IsNone() && Source.Faction == Target.Faction;

	if (Relationship.bSameActor || Relationship.bSameFaction)
		Relationship.Attitude = EPWTeamAttitude::Friendly;
	else if (Source.Faction.IsNone() || Target.Faction.IsNone())
		Relationship.Attitude = EPWTeamAttitude::Unknown;
	else if (Source.bNeutralFaction || Target.bNeutralFaction)
		Relationship.Attitude = EPWTeamAttitude::Neutral;
	else
		Relationship.Attitude = DefaultAttitude;

	for (const UPWTeamRelationshipProvider* Provider : Providers)
		if (Provider)
			Provider->ResolveRelationship(Source, Target, Relationship);
	return Relationship;
}

FPWTeamRelationship UPWTeamsSubsystem::GetActorRelationship(const AActor* Source, const AActor* Target)
{
	const UPWTeamsSubsystem* Teams = Get(Target ? Target : Source);
	return Teams ? Teams->GetRelationship(GetIdentity(Source), GetIdentity(Target)) : FPWTeamRelationship();
}

FGameplayTagContainer UPWTeamsSubsystem::GetTargetTags(const FPWTeamIdentity& Source, const FPWTeamIdentity& Target) const
{
	const FPWTeamRelationship Relationship = GetRelationship(Source, Target);

	FGameplayTagContainer Tags = Target.AgentTags;
	Tags.AppendTags(Relationship.Tags);
	if (Target.bPrimary)
		Tags.AddTag(PWTeamTags::Role_Primary);

	// Self is the asking actor and nothing else, so it is not also an ally of itself.
	if (Relationship.bSameActor)
	{
		Tags.AddTag(PWTeamTags::Relation_Self);
		return Tags;
	}

	if (Relationship.bSameOwner)
		Tags.AddTag(PWTeamTags::Relation_SameOwner);
	if (Relationship.bSameFaction)
		Tags.AddTag(PWTeamTags::Relation_SameFaction);

	switch (Relationship.Attitude)
	{
	case EPWTeamAttitude::Friendly: Tags.AddTag(PWTeamTags::Relation_Ally); break;
	case EPWTeamAttitude::Hostile: Tags.AddTag(PWTeamTags::Relation_Enemy); break;
	case EPWTeamAttitude::Neutral: Tags.AddTag(PWTeamTags::Relation_Neutral); break;
	default: Tags.AddTag(PWTeamTags::Relation_Unaffiliated); break;
	}
	return Tags;
}

bool UPWTeamsSubsystem::MatchesFilter(const FPWTeamIdentity& Source, const FPWTeamIdentity& Target, const FPWTeamTargetFilter& Filter) const
{
	return Filter.IsEmpty() || Filter.Matches(GetTargetTags(Source, Target));
}

bool UPWTeamsSubsystem::ActorMatchesFilter(const AActor* Source, const AActor* Target, const FPWTeamTargetFilter& Filter)
{
	if (Filter.IsEmpty())
		return true;

	const UPWTeamsSubsystem* Teams = Get(Target ? Target : Source);
	return Teams && Teams->MatchesFilter(GetIdentity(Source), GetIdentity(Target), Filter);
}

TArray<AActor*> UPWTeamsSubsystem::FilterActors(const AActor* Source, const TArray<AActor*>& Actors, const FPWTeamTargetFilter& Filter)
{
	TArray<AActor*> Matching;
	const UPWTeamsSubsystem* Teams = Get(Source);
	if (!Teams)
		return Matching;

	const FPWTeamIdentity SourceIdentity = GetIdentity(Source);
	for (AActor* Actor : Actors)
		if (Actor && Teams->MatchesFilter(SourceIdentity, GetIdentity(Actor), Filter))
			Matching.Add(Actor);
	return Matching;
}

bool UPWTeamsSubsystem::IsAlly(const AActor* Source, const AActor* Target)
{
	return GetActorRelationship(Source, Target).Attitude == EPWTeamAttitude::Friendly;
}

bool UPWTeamsSubsystem::IsEnemy(const AActor* Source, const AActor* Target)
{
	return GetActorRelationship(Source, Target).Attitude == EPWTeamAttitude::Hostile;
}

bool UPWTeamsSubsystem::IsSameOwner(const AActor* Source, const AActor* Target)
{
	return GetActorRelationship(Source, Target).bSameOwner;
}

bool UPWTeamsSubsystem::IsSameFaction(const AActor* Source, const AActor* Target)
{
	return GetActorRelationship(Source, Target).bSameFaction;
}

void UPWTeamsSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	CreateFactionsFromSetup();
	PushToRegistry();
}

void UPWTeamsSubsystem::PushToRegistry()
{
	UWorld* World = GetWorld();
	if (!HasAuthority() || !World || !World->IsGameWorld() || !World->AreActorsInitialized())
		return;

	if (!Registry)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.ObjectFlags |= RF_Transient;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Registry = World->SpawnActor<APWTeamRegistry>(SpawnParameters);
	}
	if (!Registry)
		return;

	Registry->Participants = Participants;
	Registry->Factions = Factions;
	Registry->DefaultAttitude = DefaultAttitude;
	Registry->ForceNetUpdate();
}

void UPWTeamsSubsystem::SetDefaultAttitude(const EPWTeamAttitude NewAttitude)
{
	if (!HasAuthority() || DefaultAttitude == NewAttitude)
		return;

	DefaultAttitude = NewAttitude;
	PushToRegistry();
}

void UPWTeamsSubsystem::RefreshAllAgents()
{
	const TArray<TWeakObjectPtr<UPWTeamAgentComponent>> Snapshot = Agents;
	for (const TWeakObjectPtr<UPWTeamAgentComponent>& Agent : Snapshot)
		if (Agent.IsValid())
			Agent->RefreshFaction();
}

void UPWTeamsSubsystem::ApplyReplicatedState(const APWTeamRegistry& From)
{
	const TArray<FPWTeamParticipant> OldParticipants = MoveTemp(Participants);
	const TArray<FPWTeamFaction> OldFactions = MoveTemp(Factions);

	Participants = From.Participants;
	Factions = From.Factions;
	DefaultAttitude = From.DefaultAttitude;

	const auto FindFaction = [](const TArray<FPWTeamFaction>& In, const FName Name) { return In.FindByPredicate([&](const FPWTeamFaction& Each) { return Each.Name == Name; }); };
	const auto FindIn = [](const TArray<FPWTeamParticipant>& In, const FPWTeamParticipantHandle Handle) { return In.FindByPredicate([&](const FPWTeamParticipant& Each) { return Each.Handle == Handle; }); };

	// Listeners may change nothing here, but work from copies so a callback cannot disturb the loops.
	const TArray<FPWTeamParticipant> NewParticipants = Participants;
	const TArray<FPWTeamFaction> NewFactions = Factions;

	for (const FPWTeamFaction& Faction : NewFactions)
		if (!FindFaction(OldFactions, Faction.Name))
			OnFactionCreated.Broadcast(Faction);

	for (const FPWTeamParticipant& Participant : NewParticipants)
	{
		const FPWTeamParticipant* Old = FindIn(OldParticipants, Participant.Handle);
		if (!Old)
			OnParticipantRegistered.Broadcast(Participant);
		if ((Old ? Old->Faction : NAME_None) != Participant.Faction)
			OnParticipantFactionChanged.Broadcast(Participant.Handle, Old ? Old->Faction : NAME_None, Participant.Faction);
	}

	for (const FPWTeamParticipant& Participant : OldParticipants)
		if (!FindIn(NewParticipants, Participant.Handle))
			OnParticipantUnregistered.Broadcast(Participant);

	for (const FPWTeamFaction& Faction : OldFactions)
		if (!FindFaction(NewFactions, Faction.Name))
			OnFactionDestroyed.Broadcast(Faction);

	// Agents may have arrived before the registry did; their inherited factions resolve now.
	RefreshAllAgents();
}

void UPWTeamsSubsystem::CreateFactionsFromSetup()
{
	const UWorld* World = GetWorld();
	if (!HasAuthority() || !World || !World->IsGameWorld())
		return;

	// A faction someone already joined keeps its place; the setup only decides whether it is neutral.
	if (const UPWTeamSetup* Setup = UPWTeamsSettings::Get()->TeamSetup.LoadSynchronous())
		for (const FPWTeamFaction& Wanted : Setup->Factions)
		{
			if (Wanted.Name.IsNone())
				continue;
			if (FPWTeamFaction* Existing = Factions.FindByPredicate([&](const FPWTeamFaction& Each) { return Each.Name == Wanted.Name; }))
				Existing->bNeutral = Wanted.bNeutral;
			else
				CreateFaction(Wanted.Name, Wanted.bNeutral);
		}
}
