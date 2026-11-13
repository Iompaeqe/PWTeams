// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "PWTeamsTypes.generated.h"

class APlayerState;
class UPWTeamAgentComponent;

// Identifies a participant. Given out by the server; an empty handle is nobody.
USTRUCT(BlueprintType)
struct PWTEAMS_API FPWTeamParticipantHandle
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Teams")
	int32 Id = 0;

	bool IsValid() const { return Id != 0; }
	bool operator==(const FPWTeamParticipantHandle& Other) const { return Id == Other.Id; }
	bool operator!=(const FPWTeamParticipantHandle& Other) const { return Id != Other.Id; }
	friend uint32 GetTypeHash(const FPWTeamParticipantHandle& Handle) { return ::GetTypeHash(Handle.Id); }
};

UENUM(BlueprintType)
enum class EPWTeamParticipantType : uint8
{
	Player,
	AI,
	Environment
};

// An identity in the match: a player, an AI player or the environment. It is not an actor, and what it owns can be controlled by anyone.
USTRUCT(BlueprintType)
struct PWTEAMS_API FPWTeamParticipant
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Teams")
	FPWTeamParticipantHandle Handle;

	// Shown to players; does not have to be unique.
	UPROPERTY(BlueprintReadOnly, Category="Teams")
	FName Name;

	UPROPERTY(BlueprintReadOnly, Category="Teams")
	EPWTeamParticipantType Type = EPWTeamParticipantType::Player;

	// The faction it is a member of; None for no faction.
	UPROPERTY(BlueprintReadOnly, Category="Teams")
	FName Faction;

	// The player this participant stands for, when there is one. Useful for crediting damage and kills.
	UPROPERTY(BlueprintReadOnly, Category="Teams")
	TObjectPtr<APlayerState> PlayerState;
};

// A side. Identified by its name, which the game chooses freely.
USTRUCT(BlueprintType)
struct PWTEAMS_API FPWTeamFaction
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Teams")
	FName Name;

	// A neutral faction is neither friend nor enemy to anyone.
	UPROPERTY(BlueprintReadOnly, Category="Teams")
	bool bNeutral = false;
};

// Where an agent gets its faction from.
UENUM(BlueprintType)
enum class EPWTeamFactionSource : uint8
{
	// No faction.
	None,
	// The faction of the participant that owns it, following that participant if it changes sides.
	InheritFromOwner,
	// A faction of its own, whatever its owner's is.
	Explicit
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPWTeamAgentOwnerChanged, UPWTeamAgentComponent*, Agent, FPWTeamParticipantHandle, OldOwner, FPWTeamParticipantHandle, NewOwner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPWTeamAgentFactionChanged, UPWTeamAgentComponent*, Agent, FName, OldFaction, FName, NewFaction);
