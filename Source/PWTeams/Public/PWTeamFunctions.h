// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PWTeamFunctions.generated.h"

UENUM(BlueprintType)
// Neutral is an actor without a team, such as a prop.
enum class EPWRelation : uint8 { Unknown, Self, Ally, Enemy, Neutral };

UCLASS()
class PWTEAMS_API UPWTeamFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category="Team")
	static int32 GetTeamId(const AActor* Actor);

	UFUNCTION(BlueprintPure, Category="Team")
	static bool AreFriendly(const AActor* A, const AActor* B);

	UFUNCTION(BlueprintPure, Category="Team")
	static bool AreEnemies(const AActor* A, const AActor* B);

	UFUNCTION(BlueprintPure, Category="Team")
	static bool EnumMaskHas(EPWRelationMask Mask, EPWRelationMask Flag);

	UFUNCTION(BlueprintPure, Category="Team")
	static EPWRelation GetRelation(const AActor* A, const AActor* B);

	UFUNCTION(BlueprintPure, Category="Team")
	static bool MatchesMask(const AActor* A, const AActor* B, uint8 MaskByte);
};
