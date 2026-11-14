// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "Agents/PWTeamAgentComponent.h"

#include "Core/PWTeamsSubsystem.h"
#include "Core/PWTeamSetup.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"

UPWTeamAgentComponent::UPWTeamAgentComponent()
{
	SetIsReplicatedByDefault(true);
	FactionSetting = PWTeamFactions::InheritFromOwner;
}

void UPWTeamAgentComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPWTeamAgentComponent, OwnerParticipant);
	DOREPLIFETIME(UPWTeamAgentComponent, FactionSetting);
	DOREPLIFETIME(UPWTeamAgentComponent, AgentTags);
	DOREPLIFETIME(UPWTeamAgentComponent, bPrimary);
}

bool UPWTeamAgentComponent::HasAuthority() const
{
	const AActor* Owner = GetOwner();
	return Owner && Owner->HasAuthority();
}

void UPWTeamAgentComponent::OnRegister()
{
	Super::OnRegister();

	const UWorld* World = GetWorld();
	UPWTeamsSubsystem* Teams = World && World->IsGameWorld() ? World->GetSubsystem<UPWTeamsSubsystem>() : nullptr;
	if (!Teams)
		return;

	Teams->RegisterAgent(this);

	APlayerState* PlayerState = Cast<APlayerState>(GetOwner());
	if (bRegisterParticipant && PlayerState && HasAuthority() && !OwnerParticipant.IsValid())
	{
		// The bot flag is set after the player state spawns, so ask who owns it.
		const AController* Controller = Cast<AController>(PlayerState->GetOwner());
		const EPWTeamParticipantType Type = PlayerState->IsABot() || (Controller && !Controller->IsPlayerController()) ? EPWTeamParticipantType::AI : EPWTeamParticipantType::Player;
		OwnerParticipant = Teams->RegisterParticipant(FName(*PlayerState->GetPlayerName()), Type, NAME_None, PlayerState);
		bRegisteredParticipant = OwnerParticipant.IsValid();
		bPrimary = true;
	}

	ResolvedFaction = GetFaction();
}

void UPWTeamAgentComponent::OnUnregister()
{
	const UWorld* World = GetWorld();
	if (UPWTeamsSubsystem* Teams = World ? World->GetSubsystem<UPWTeamsSubsystem>() : nullptr)
	{
		Teams->UnregisterAgent(this);
		if (bRegisteredParticipant && HasAuthority())
			Teams->UnregisterParticipant(OwnerParticipant);
	}
	bRegisteredParticipant = false;

	Super::OnUnregister();
}

EPWTeamFactionSource UPWTeamAgentComponent::GetFactionSource() const
{
	if (FactionSetting.IsNone())
		return EPWTeamFactionSource::None;
	return FactionSetting == PWTeamFactions::InheritFromOwner ? EPWTeamFactionSource::InheritFromOwner : EPWTeamFactionSource::Explicit;
}

FName UPWTeamAgentComponent::GetFaction() const
{
	if (FactionSetting != PWTeamFactions::InheritFromOwner)
		return FactionSetting;

	const UPWTeamsSubsystem* Teams = UPWTeamsSubsystem::Get(this);
	return Teams ? Teams->GetParticipantFaction(OwnerParticipant) : NAME_None;
}

void UPWTeamAgentComponent::SetOwnerParticipant(const FPWTeamParticipantHandle NewOwner)
{
	if (!HasAuthority() || OwnerParticipant == NewOwner)
		return;

	const FPWTeamParticipantHandle OldOwner = OwnerParticipant;
	OwnerParticipant = NewOwner;
	OnRep_OwnerParticipant(OldOwner);
}

void UPWTeamAgentComponent::SetFaction(const FName NewFaction)
{
	if (!HasAuthority() || FactionSetting == NewFaction)
		return;

	FactionSetting = NewFaction;
	RefreshFaction();
}

void UPWTeamAgentComponent::InheritFactionFromOwner()
{
	SetFaction(PWTeamFactions::InheritFromOwner);
}

void UPWTeamAgentComponent::RefreshFaction()
{
	const FName NewFaction = GetFaction();
	if (NewFaction == ResolvedFaction)
		return;

	const FName OldFaction = ResolvedFaction;
	ResolvedFaction = NewFaction;
	OnFactionChanged.Broadcast(this, OldFaction, NewFaction);
	if (UPWTeamsSubsystem* Teams = UPWTeamsSubsystem::Get(this))
		Teams->OnAgentFactionChanged.Broadcast(this, OldFaction, NewFaction);
}

void UPWTeamAgentComponent::OnRep_OwnerParticipant(const FPWTeamParticipantHandle OldOwner)
{
	OnOwnerChanged.Broadcast(this, OldOwner, OwnerParticipant);
	if (UPWTeamsSubsystem* Teams = UPWTeamsSubsystem::Get(this))
		Teams->OnAgentOwnerChanged.Broadcast(this, OldOwner, OwnerParticipant);
	RefreshFaction();
}

void UPWTeamAgentComponent::OnRep_Faction()
{
	RefreshFaction();
}

void UPWTeamAgentComponent::SetAgentTags(const FGameplayTagContainer& NewTags)
{
	if (HasAuthority())
		AgentTags = NewTags;
}

void UPWTeamAgentComponent::SetPrimary(const bool bNewPrimary)
{
	if (HasAuthority())
		bPrimary = bNewPrimary;
}

TArray<FName> UPWTeamAgentComponent::GetFactionOptions() const
{
	TArray<FName> Options = { NAME_None, PWTeamFactions::InheritFromOwner };
	for (const FName& Name : UPWTeamSetup::GetAllFactionNames())
		Options.AddUnique(Name);
	// Keep a name that is in no setup selectable, so opening the actor never loses it.
	Options.AddUnique(FactionSetting);
	return Options;
}
