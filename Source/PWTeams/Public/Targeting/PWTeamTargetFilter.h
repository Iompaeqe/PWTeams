// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "PWTeamTargetFilter.generated.h"

// Chooses targets by the tags that describe them: relation tags such as Team.Relation.Enemy, Team.Role.Primary, an agent's own tags, and any tags a game adds. An empty filter accepts everyone.
USTRUCT(BlueprintType)
struct PWTEAMS_API FPWTeamTargetFilter
{
	GENERATED_BODY()

	// The target needs at least one of these.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Teams")
	FGameplayTagContainer AnyOf;

	// The target needs every one of these.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Teams")
	FGameplayTagContainer AllOf;

	// The target must have none of these.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Teams")
	FGameplayTagContainer NoneOf;

	bool IsEmpty() const { return AnyOf.IsEmpty() && AllOf.IsEmpty() && NoneOf.IsEmpty(); }

	bool Matches(const FGameplayTagContainer& TargetTags) const
	{
		if (!AnyOf.IsEmpty() && !TargetTags.HasAny(AnyOf))
			return false;
		return TargetTags.HasAll(AllOf) && !TargetTags.HasAny(NoneOf);
	}
};
