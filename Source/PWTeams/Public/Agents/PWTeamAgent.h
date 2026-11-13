// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PWTeamAgent.generated.h"

class UPWTeamAgentComponent;

UINTERFACE(BlueprintType)
class PWTEAMS_API UPWTeamAgent : public UInterface
{
	GENERATED_BODY()
};

// For an actor whose team identity lives somewhere else. A character returns its player state's agent, so a player has one identity whatever it controls.
class PWTEAMS_API IPWTeamAgent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Team")
	UPWTeamAgentComponent* GetTeamAgent() const;
};
