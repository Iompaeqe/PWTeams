// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Core/PWTeamsTypes.h"
#include "PWTeamsSubsystem.generated.h"

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

private:
	UPROPERTY()
	TArray<FPWTeamParticipant> Participants;
	UPROPERTY()
	TArray<FPWTeamFaction> Factions;

	TArray<TWeakObjectPtr<UPWTeamAgentComponent>> Agents;

	int32 NextParticipantId = 1;
};
