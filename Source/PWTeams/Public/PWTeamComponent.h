// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PWTeamComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PWTEAMS_API UPWTeamComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Team the owner belongs to; 0 means no team, which makes it neutral to everyone.
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	int32 TeamId = 0;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual int32 GetTeamId() const { return TeamId; }
};
