// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "Relationships/PWTeamRelationshipProvider.h"

UWorld* UPWTeamRelationshipProvider::GetWorld() const
{
	const UObject* Outer = HasAnyFlags(RF_ClassDefaultObject) ? nullptr : GetOuter();
	return Outer ? Outer->GetWorld() : nullptr;
}
