// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Core/PWTeamsTypes.h"
#include "Relationships/PWTeamRelationship.h"
#include "Targeting/PWTeamTargetFilter.h"
#include "PWTeamsSubsystem.generated.h"

class UPWTeamRelationshipProvider;
class APWTeamRegistry;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPWTeamParticipantEvent, const FPWTeamParticipant&, Participant);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPWTeamFactionEvent, const FPWTeamFaction&, Faction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPWTeamParticipantFactionChanged, FPWTeamParticipantHandle, Participant, FName, OldFaction, FName, NewFaction);

// The entry point of PWTeams: who takes part in the match, which factions exist and who is a member of which. Changes are made on the server.
UCLASS()
class PWTEAMS_API UPWTeamsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	static UPWTeamsSubsystem* Get(const UObject* WorldContext);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	// ---- Replication

	// The actor that carries the team state to clients. The server spawns it by itself.
	APWTeamRegistry* GetRegistry() const { return Registry; }

	// Called on a client when team state arrives from the server. Tells listeners what changed.
	void ApplyReplicatedState(const APWTeamRegistry& From);

	// ---- Participants

	// Server only. Joins the faction when one is given, creating it if it does not exist yet.
	UFUNCTION(BlueprintCallable, Category="Teams|Participants")
	FPWTeamParticipantHandle RegisterParticipant(FName Name, EPWTeamParticipantType Type, FName Faction = NAME_None, APlayerState* PlayerState = nullptr);

	// Server only.
	UFUNCTION(BlueprintCallable, Category="Teams|Participants")
	bool UnregisterParticipant(FPWTeamParticipantHandle Participant);

	UFUNCTION(BlueprintPure, Category="Teams|Participants")
	bool GetParticipant(FPWTeamParticipantHandle Participant, FPWTeamParticipant& OutParticipant) const;

	UFUNCTION(BlueprintPure, Category="Teams|Participants")
	FPWTeamParticipantHandle FindParticipantForPlayer(const APlayerState* PlayerState) const;

	UFUNCTION(BlueprintPure, Category="Teams|Participants")
	TArray<FPWTeamParticipant> GetParticipants() const { return Participants; }

	// ---- Factions

	// Server only. False when a faction of that name already exists.
	UFUNCTION(BlueprintCallable, Category="Teams|Factions")
	bool CreateFaction(FName Name, bool bNeutral = false);

	// Server only. Its members are left without a faction.
	UFUNCTION(BlueprintCallable, Category="Teams|Factions")
	bool DestroyFaction(FName Name);

	UFUNCTION(BlueprintPure, Category="Teams|Factions")
	bool GetFaction(FName Name, FPWTeamFaction& OutFaction) const;

	UFUNCTION(BlueprintPure, Category="Teams|Factions")
	TArray<FPWTeamFaction> GetFactions() const { return Factions; }

	UFUNCTION(BlueprintPure, Category="Teams|Factions")
	TArray<FPWTeamParticipantHandle> GetFactionParticipants(FName Faction) const;

	// ---- Membership

	// Server only. Creates the faction if it does not exist yet. A participant is in one faction at a time.
	UFUNCTION(BlueprintCallable, Category="Teams|Membership")
	bool AssignParticipantToFaction(FPWTeamParticipantHandle Participant, FName Faction);

	// Server only.
	UFUNCTION(BlueprintCallable, Category="Teams|Membership")
	bool RemoveParticipantFromFaction(FPWTeamParticipantHandle Participant);

	UFUNCTION(BlueprintPure, Category="Teams|Membership")
	FName GetParticipantFaction(FPWTeamParticipantHandle Participant) const;

	// ---- Agents

	// The agent component that speaks for an actor: through IPWTeamAgent, then a component on the actor, then a pawn's player state.
	UFUNCTION(BlueprintPure, Category="Teams|Agents")
	static UPWTeamAgentComponent* FindAgent(const AActor* Actor);

	// The participant an actor is attributed to. Use it to credit damage and kills to the player behind a summon.
	UFUNCTION(BlueprintPure, Category="Teams|Agents")
	static FPWTeamParticipantHandle GetAgentOwner(const AActor* Actor);

	// The faction an actor is in right now; None for no faction or no agent.
	UFUNCTION(BlueprintPure, Category="Teams|Agents")
	static FName GetAgentFaction(const AActor* Actor);

	// Server only.
	UFUNCTION(BlueprintCallable, Category="Teams|Agents")
	static bool SetAgentOwner(AActor* Actor, FPWTeamParticipantHandle Participant);

	// Server only. Gives the actor a faction of its own, whatever its owner's is.
	UFUNCTION(BlueprintCallable, Category="Teams|Agents")
	static bool SetAgentFaction(AActor* Actor, FName Faction);

	// Server only.
	UFUNCTION(BlueprintCallable, Category="Teams|Agents")
	static bool SetAgentFactionSource(AActor* Actor, EPWTeamFactionSource Source);

	UFUNCTION(BlueprintPure, Category="Teams|Agents")
	TArray<AActor*> GetAgentsOwnedBy(FPWTeamParticipantHandle Participant) const;

	UFUNCTION(BlueprintPure, Category="Teams|Agents")
	TArray<AActor*> GetAgentsInFaction(FName Faction) const;

	void RegisterAgent(UPWTeamAgentComponent* Agent);
	void UnregisterAgent(UPWTeamAgentComponent* Agent);

	// ---- Relationships

	// What PWTeams knows about an actor: owner, faction, role and tags.
	UFUNCTION(BlueprintPure, Category="Teams|Relationships")
	static FPWTeamIdentity GetIdentity(const AActor* Actor);

	// The same for a participant with no actor to ask, for example the caster of a spell that outlived it.
	UFUNCTION(BlueprintPure, Category="Teams|Relationships")
	FPWTeamIdentity GetParticipantIdentity(FPWTeamParticipantHandle Participant) const;

	// How Source regards Target: the default rules first, then every relationship provider in order.
	UFUNCTION(BlueprintPure, Category="Teams|Relationships")
	FPWTeamRelationship GetRelationship(const FPWTeamIdentity& Source, const FPWTeamIdentity& Target) const;

	UFUNCTION(BlueprintPure, Category="Teams|Relationships")
	static FPWTeamRelationship GetActorRelationship(const AActor* Source, const AActor* Target);

	// Every tag that describes Target as seen by Source. This is what ability filters match against.
	UFUNCTION(BlueprintPure, Category="Teams|Relationships")
	FGameplayTagContainer GetTargetTags(const FPWTeamIdentity& Source, const FPWTeamIdentity& Target) const;

	UFUNCTION(BlueprintPure, Category="Teams|Relationships")
	bool MatchesFilter(const FPWTeamIdentity& Source, const FPWTeamIdentity& Target, const FPWTeamTargetFilter& Filter) const;

	// The one check abilities, effects and collision all use.
	UFUNCTION(BlueprintPure, Category="Teams|Relationships")
	static bool ActorMatchesFilter(const AActor* Source, const AActor* Target, const FPWTeamTargetFilter& Filter);

	UFUNCTION(BlueprintPure, Category="Teams|Relationships")
	static TArray<AActor*> FilterActors(const AActor* Source, const TArray<AActor*>& Actors, const FPWTeamTargetFilter& Filter);

	UFUNCTION(BlueprintPure, Category="Teams|Relationships")
	static bool IsAlly(const AActor* Source, const AActor* Target);
	UFUNCTION(BlueprintPure, Category="Teams|Relationships")
	static bool IsEnemy(const AActor* Source, const AActor* Target);
	UFUNCTION(BlueprintPure, Category="Teams|Relationships")
	static bool IsSameOwner(const AActor* Source, const AActor* Target);
	UFUNCTION(BlueprintPure, Category="Teams|Relationships")
	static bool IsSameFaction(const AActor* Source, const AActor* Target);

	// Adds a game's own rules at runtime, after the ones from the project settings. Add the same ones on every machine that asks about relationships.
	UFUNCTION(BlueprintCallable, Category="Teams|Relationships")
	UPWTeamRelationshipProvider* AddRelationshipProvider(TSubclassOf<UPWTeamRelationshipProvider> ProviderClass);

	UFUNCTION(BlueprintCallable, Category="Teams|Relationships")
	void RemoveRelationshipProvider(UPWTeamRelationshipProvider* Provider);

	// Between two different factions when neither is neutral. Starts from the project settings; change it with Set Default Attitude.
	UPROPERTY(BlueprintReadOnly, Category="Teams|Relationships")
	EPWTeamAttitude DefaultAttitude = EPWTeamAttitude::Hostile;

	// Server only. Reaches clients with the rest of the team state.
	UFUNCTION(BlueprintCallable, Category="Teams|Relationships")
	void SetDefaultAttitude(EPWTeamAttitude NewAttitude);

	// ---- Events

	UPROPERTY(BlueprintAssignable, Category="Teams")
	FPWTeamParticipantEvent OnParticipantRegistered;
	UPROPERTY(BlueprintAssignable, Category="Teams")
	FPWTeamParticipantEvent OnParticipantUnregistered;
	UPROPERTY(BlueprintAssignable, Category="Teams")
	FPWTeamFactionEvent OnFactionCreated;
	UPROPERTY(BlueprintAssignable, Category="Teams")
	FPWTeamFactionEvent OnFactionDestroyed;
	UPROPERTY(BlueprintAssignable, Category="Teams")
	FPWTeamParticipantFactionChanged OnParticipantFactionChanged;
	UPROPERTY(BlueprintAssignable, Category="Teams")
	FPWTeamAgentOwnerChanged OnAgentOwnerChanged;
	UPROPERTY(BlueprintAssignable, Category="Teams")
	FPWTeamAgentFactionChanged OnAgentFactionChanged;

protected:
	bool HasAuthority() const;
	FPWTeamParticipant* FindParticipant(FPWTeamParticipantHandle Participant);
	const FPWTeamParticipant* FindParticipant(FPWTeamParticipantHandle Participant) const;
	void SetParticipantFaction(FPWTeamParticipant& Participant, FName NewFaction);
	void RefreshAgentsOf(FPWTeamParticipantHandle Participant);
	void RefreshAllAgents();

	// Server: copies the current state to the registry actor so it reaches clients.
	void PushToRegistry();

	// Server: creates the factions listed in the project's team setup.
	void CreateFactionsFromSetup();

private:
	UPROPERTY()
	TArray<FPWTeamParticipant> Participants;
	UPROPERTY()
	TArray<FPWTeamFaction> Factions;

	TArray<TWeakObjectPtr<UPWTeamAgentComponent>> Agents;

	UPROPERTY()
	TArray<TObjectPtr<UPWTeamRelationshipProvider>> Providers;

	UPROPERTY()
	TObjectPtr<APWTeamRegistry> Registry;

	int32 NextParticipantId = 1;
};
