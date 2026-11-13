// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)

#pragma once

#include "NativeGameplayTags.h"

// Tags that describe a target from the point of view of whoever is asking. Ability filters match on these, on an agent's own tags, and on whatever a game's relationship providers add.
namespace PWTeamTags
{
	PWTEAMS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Relation);
	PWTEAMS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Relation_Self);
	PWTEAMS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Relation_SameOwner);
	PWTEAMS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Relation_SameFaction);
	PWTEAMS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Relation_Ally);
	PWTEAMS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Relation_Enemy);
	PWTEAMS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Relation_Neutral);
	PWTEAMS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Relation_Unaffiliated);

	PWTEAMS_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Role_Primary);
}
