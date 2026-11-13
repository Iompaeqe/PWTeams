// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "PWTeamTypes.h"
#include "Engine/DataAsset.h"
#include "PWTeamColorData.generated.h"

UCLASS()
class PWTEAMS_API UPWTeamColorData : public UDataAsset
{
	GENERATED_BODY()

public:
	// Color palette for each team id.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<int32, FPWTeamPalette> Teams;
};
