// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "PWTeamSetupActor.generated.h"

class UPWTeamSetup;

// Place one in a level to give that level its team rules, with no code. A game mode can do the same by calling Apply Team Setup on the teams subsystem.
UCLASS(meta=(DisplayName="PW Team Setup"))
class PWTEAMS_API APWTeamSetupActor : public AInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Teams")
	TObjectPtr<UPWTeamSetup> Setup;

protected:
	// Applied this early so the rules are in place before the first player joins.
	virtual void PostInitializeComponents() override;
};
