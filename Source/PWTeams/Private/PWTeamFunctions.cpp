// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "PWTeamFunctions.h"

#include "PWTeamAgent.h"
#include "PWTeamComponent.h"
#include "Ability/PWSpellParams.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

int32 UPWTeamFunctions::GetTeamId(const AActor* Actor)
{
	if (!Actor) return 0;

	if (Actor->Implements<UPWTeamAgent>())
		return IPWTeamAgent::Execute_GetTeamId_BP(Actor);
	if (const UPWTeamComponent* Team = Actor->FindComponentByClass<UPWTeamComponent>())
		return Team->GetTeamId();

	const APawn* Pawn = Cast<APawn>(Actor);
	const APlayerState* PlayerState = Pawn ? Pawn->GetPlayerState() : nullptr;
	if (const UPWTeamComponent* Team = PlayerState ? PlayerState->FindComponentByClass<UPWTeamComponent>() : nullptr)
		return Team->GetTeamId();

	return 0;
}

bool UPWTeamFunctions::AreFriendly(const AActor* A, const AActor* B)
{
	const int32 TA = GetTeamId(A), TB = GetTeamId(B);
	return TA != 0 && TA == TB;
}

bool UPWTeamFunctions::AreEnemies(const AActor* A, const AActor* B)
{
	const int32 TA = GetTeamId(A), TB = GetTeamId(B);
	return TA != 0 && TB != 0 && TA != TB;
}

bool UPWTeamFunctions::EnumMaskHas(EPWRelationMask Mask, EPWRelationMask Flag)
{
	return EnumHasAnyFlags(Mask, Flag);
}

EPWRelation UPWTeamFunctions::GetRelation(const AActor* A, const AActor* B)
{
	if (!A || !B) return EPWRelation::Unknown;
	if (A == B)   return EPWRelation::Self;

	const int32 TA = GetTeamId(A);
	const int32 TB = GetTeamId(B);
	if (TA == 0 || TB == 0) return EPWRelation::Neutral;

	return (TA == TB) ? EPWRelation::Ally : EPWRelation::Enemy;
}

bool UPWTeamFunctions::MatchesMask(const AActor* A, const AActor* B, uint8 MaskByte)
{
	const EPWRelationMask Mask = static_cast<EPWRelationMask>(MaskByte);
	switch (GetRelation(A, B))
	{
	case EPWRelation::Self:  return EnumMaskHas(Mask, EPWRelationMask::Self);
	case EPWRelation::Ally:  return EnumMaskHas(Mask, EPWRelationMask::Ally);
	case EPWRelation::Enemy: return EnumMaskHas(Mask, EPWRelationMask::Enemy);
	case EPWRelation::Neutral: return EnumMaskHas(Mask, EPWRelationMask::Neutral);
	default:               return true;
	}
}
