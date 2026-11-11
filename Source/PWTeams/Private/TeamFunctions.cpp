// copyright mkurtt96


#include "TeamFunctions.h"

#include "TeamAgent.h"
#include "TeamComponent.h"
#include "Ability/SpellParams.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerState.h"

int32 UTeamFunctions::GetTeamId(const AActor* Actor)
{
	if (!Actor) return 0;

	if (Actor->Implements<UTeamAgent>())
		return ITeamAgent::Execute_GetTeamId_BP(Actor);
	if (const UTeamComponent* Team = Actor->FindComponentByClass<UTeamComponent>())
		return Team->GetTeamId();

	const APawn* Pawn = Cast<APawn>(Actor);
	const APlayerState* PlayerState = Pawn ? Pawn->GetPlayerState() : nullptr;
	if (const UTeamComponent* Team = PlayerState ? PlayerState->FindComponentByClass<UTeamComponent>() : nullptr)
		return Team->GetTeamId();

	return 0;
}

bool UTeamFunctions::AreFriendly(const AActor* A, const AActor* B)
{
	const int32 TA = GetTeamId(A), TB = GetTeamId(B);
	return TA != 0 && TA == TB;
}

bool UTeamFunctions::AreEnemies(const AActor* A, const AActor* B)
{
	const int32 TA = GetTeamId(A), TB = GetTeamId(B);
	return TA != 0 && TB != 0 && TA != TB;
}

bool UTeamFunctions::EnumMaskHas(ERelationMask Mask, ERelationMask Flag)
{
	return EnumHasAnyFlags(Mask, Flag);
}

ERelation UTeamFunctions::GetRelation(const AActor* A, const AActor* B)
{
	if (!A || !B) return ERelation::Unknown;
	if (A == B)   return ERelation::Self;

	const int32 TA = GetTeamId(A);
	const int32 TB = GetTeamId(B);
	if (TA == 0 || TB == 0) return ERelation::Unknown;

	return (TA == TB) ? ERelation::Ally : ERelation::Enemy;
}

bool UTeamFunctions::MatchesMask(const AActor* A, const AActor* B, uint8 MaskByte)
{
	const ERelationMask Mask = static_cast<ERelationMask>(MaskByte);
	switch (GetRelation(A, B))
	{
	case ERelation::Self:  return EnumMaskHas(Mask, ERelationMask::Self);
	case ERelation::Ally:  return EnumMaskHas(Mask, ERelationMask::Ally);
	case ERelation::Enemy: return EnumMaskHas(Mask, ERelationMask::Enemy);
	default:               return true;
	}
}
