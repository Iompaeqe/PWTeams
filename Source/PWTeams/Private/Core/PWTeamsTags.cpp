// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "Core/PWTeamsTags.h"

namespace PWTeamTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Relation, "Team.Relation", "How a target relates to whoever is asking.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Relation_Self, "Team.Relation.Self", "The asking actor itself, and nothing else.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Relation_SameOwner, "Team.Relation.SameOwner", "Attributed to the same participant, such as a player's own summon.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Relation_SameFaction, "Team.Relation.SameFaction", "In the same faction.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Relation_Ally, "Team.Relation.Ally", "Friendly.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Relation_Enemy, "Team.Relation.Enemy", "Hostile.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Relation_Neutral, "Team.Relation.Neutral", "A neutral faction is involved.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Relation_Unaffiliated, "Team.Relation.Unaffiliated", "No faction to judge by, such as a world object.");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Role_Primary, "Team.Role.Primary", "A participant's main character, such as a player's hero.");
}
