// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/PWTeamsTypes.h"
#include "GameplayTagContainer.h"
#include "PWTeamAgentComponent.generated.h"

// Connects an actor to PWTeams: which participant it is attributed to and which faction it is in. Who controls the actor plays no part. Set on the server, replicated to clients.
UCLASS(ClassGroup=(PWTeams), meta=(BlueprintSpawnableComponent, DisplayName="Team Agent"))
class PWTEAMS_API UPWTeamAgentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPWTeamAgentComponent();

	// On a player state: registers a participant for that player on the server and makes it this agent's owner.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Team")
	bool bRegisterParticipant = true;

	UFUNCTION(BlueprintPure, Category="Team")
	FPWTeamParticipantHandle GetOwnerParticipant() const { return OwnerParticipant; }

	UFUNCTION(BlueprintPure, Category="Team")
	EPWTeamFactionSource GetFactionSource() const { return FactionSource; }

	// The faction this agent is in right now, after applying its faction source; None for no faction.
	UFUNCTION(BlueprintPure, Category="Team")
	FName GetFaction() const;

	// Server only.
	UFUNCTION(BlueprintCallable, Category="Team")
	void SetOwnerParticipant(FPWTeamParticipantHandle NewOwner);

	// Server only.
	UFUNCTION(BlueprintCallable, Category="Team")
	void SetFactionSource(EPWTeamFactionSource NewSource);

	// Server only. Also switches the faction source to Explicit.
	UFUNCTION(BlueprintCallable, Category="Team")
	void SetExplicitFaction(FName Faction);

	UFUNCTION(BlueprintPure, Category="Team")
	const FGameplayTagContainer& GetAgentTags() const { return AgentTags; }

	// Server only.
	UFUNCTION(BlueprintCallable, Category="Team")
	void SetAgentTags(const FGameplayTagContainer& NewTags);

	UFUNCTION(BlueprintPure, Category="Team")
	bool IsPrimary() const { return bPrimary; }

	// Server only.
	UFUNCTION(BlueprintCallable, Category="Team")
	void SetPrimary(bool bNewPrimary);

	// Server and clients.
	UPROPERTY(BlueprintAssignable, Category="Team")
	FPWTeamAgentOwnerChanged OnOwnerChanged;

	// Server and clients. Also fires when an inherited faction changes because the owner changed sides.
	UPROPERTY(BlueprintAssignable, Category="Team")
	FPWTeamAgentFactionChanged OnFactionChanged;

	// Works out the faction again and tells listeners if it changed.
	void RefreshFaction();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void OnRegister() override;
	virtual void OnUnregister() override;

	bool HasAuthority() const;

	// The participant this actor is attributed to: the player behind a summon, a totem or a turret.
	UPROPERTY(ReplicatedUsing=OnRep_OwnerParticipant, BlueprintReadOnly, Category="Team")
	FPWTeamParticipantHandle OwnerParticipant;

	UPROPERTY(ReplicatedUsing=OnRep_Faction, EditAnywhere, BlueprintReadOnly, Category="Team")
	EPWTeamFactionSource FactionSource = EPWTeamFactionSource::InheritFromOwner;

	// Used when the faction source is Explicit. Lists the factions of every team setup asset in the project.
	UPROPERTY(ReplicatedUsing=OnRep_Faction, EditAnywhere, BlueprintReadOnly, Category="Team", meta=(EditCondition="FactionSource == EPWTeamFactionSource::Explicit", GetOptions="GetFactionOptions"))
	FName ExplicitFaction;

	UFUNCTION()
	TArray<FName> GetFactionOptions() const;

	// What this actor is, for ability filters to match on. Add your own tags under Team, for example Team.Unit.Hero or Team.Unit.Minion.
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category="Team", meta=(Categories="Team"))
	FGameplayTagContainer AgentTags;

	// The participant's main character, such as a player's hero. Set for you on a player state's agent.
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category="Team")
	bool bPrimary = false;

	UFUNCTION()
	void OnRep_OwnerParticipant(FPWTeamParticipantHandle OldOwner);
	UFUNCTION()
	void OnRep_Faction();

private:
	FName ResolvedFaction;
	bool bRegisteredParticipant = false;
};
