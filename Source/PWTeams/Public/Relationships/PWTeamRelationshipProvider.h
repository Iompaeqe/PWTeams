// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Relationships/PWTeamRelationship.h"
#include "PWTeamRelationshipProvider.generated.h"

// A game's own rules for how two sides relate: alliances, blocs, diplomacy, a duel between two players. Listed in the PW Teams project settings or added at runtime; each one runs after the default rules, in order, and may change the result.
UCLASS(Abstract, Blueprintable)
class PWTEAMS_API UPWTeamRelationshipProvider : public UObject
{
	GENERATED_BODY()

public:
	// Change the relationship, add tags to it, or leave it as it is.
	UFUNCTION(BlueprintNativeEvent, Category="Teams")
	void ResolveRelationship(const FPWTeamIdentity& Source, const FPWTeamIdentity& Target, UPARAM(ref) FPWTeamRelationship& Relationship) const;
	virtual void ResolveRelationship_Implementation(const FPWTeamIdentity& Source, const FPWTeamIdentity& Target, FPWTeamRelationship& Relationship) const {}

	virtual UWorld* GetWorld() const override;
};
