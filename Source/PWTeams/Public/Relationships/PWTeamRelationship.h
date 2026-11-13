// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Core/PWTeamsTypes.h"
#include "PWTeamRelationship.generated.h"

UENUM(BlueprintType)
enum class EPWTeamAttitude : uint8
{
	Friendly,
	Neutral,
	Hostile,
	// Not enough is known to say, for example one side has no faction.
	Unknown
};

// What PWTeams knows about one side of a question. Built from an actor, or from a participant when the actor is gone.
USTRUCT(BlueprintType)
struct PWTEAMS_API FPWTeamIdentity
{
	GENERATED_BODY()

	// May be empty, for example a caster that died before its spell landed.
	UPROPERTY(BlueprintReadWrite, Category="Teams")
	TObjectPtr<const AActor> Actor;

	UPROPERTY(BlueprintReadWrite, Category="Teams")
	FPWTeamParticipantHandle Owner;

	UPROPERTY(BlueprintReadWrite, Category="Teams")
	FName Faction;

	UPROPERTY(BlueprintReadWrite, Category="Teams")
	bool bNeutralFaction = false;

	UPROPERTY(BlueprintReadWrite, Category="Teams")
	bool bPrimary = false;

	UPROPERTY(BlueprintReadWrite, Category="Teams")
	FGameplayTagContainer AgentTags;
};

// How a source relates to a target. The properties are independent: a captured turret can have the same owner and still be hostile.
USTRUCT(BlueprintType)
struct PWTEAMS_API FPWTeamRelationship
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Teams")
	bool bSameActor = false;

	UPROPERTY(BlueprintReadWrite, Category="Teams")
	bool bSameOwner = false;

	UPROPERTY(BlueprintReadWrite, Category="Teams")
	bool bSameFaction = false;

	UPROPERTY(BlueprintReadWrite, Category="Teams")
	EPWTeamAttitude Attitude = EPWTeamAttitude::Unknown;

	// Extra tags for ability filters to match on. A relationship provider adds its own here, such as "same alliance".
	UPROPERTY(BlueprintReadWrite, Category="Teams")
	FGameplayTagContainer Tags;
};
