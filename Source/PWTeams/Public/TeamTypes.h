// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "TeamTypes.generated.h" 

USTRUCT(BlueprintType)
struct PWTEAMS_API FTeamColor
{
	GENERATED_BODY()
	// Main team color.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Primary = FLinearColor::White;
	// Accent color used next to the primary.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor Secondary = FLinearColor::Black;
};

USTRUCT(BlueprintType)
struct PWTEAMS_API FTeamPalette
{
	GENERATED_BODY()
	// Colors shared by the whole team.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTeamColor TeamBase;
	// Colors handed out to individual players of the team.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLinearColor> PlayerSwatch;
};
