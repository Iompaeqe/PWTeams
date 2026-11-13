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

protected:
	bool HasAuthority() const;
	FPWTeamParticipant* FindParticipant(FPWTeamParticipantHandle Participant);
	const FPWTeamParticipant* FindParticipant(FPWTeamParticipantHandle Participant) const;
	void SetParticipantFaction(FPWTeamParticipant& Participant, FName NewFaction);

private:
	UPROPERTY()
	TArray<FPWTeamParticipant> Participants;
	UPROPERTY()
	TArray<FPWTeamFaction> Factions;

	int32 NextParticipantId = 1;
};
