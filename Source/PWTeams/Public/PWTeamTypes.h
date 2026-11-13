// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "PWTeamTypes.generated.h" 

USTRUCT(BlueprintType)
struct PWTEAMS_API FPWTeamColor
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
struct PWTEAMS_API FPWTeamPalette
{
	GENERATED_BODY()
	// Colors shared by the whole team.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPWTeamColor TeamBase;
	// Colors handed out to individual players of the team.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FLinearColor> PlayerSwatch;
};
