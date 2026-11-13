// Copyright (c) 2025 Metehan Goksel Kurtulan (Iompaeqe)


#include "Core/PWTeamSetup.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Relationships/PWTeamRelationshipProvider.h"

TArray<FName> UPWTeamSetup::GetAllFactionNames()
{
	TArray<FName> Names;

	TArray<FAssetData> Assets;
	FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")).Get().GetAssetsByClass(StaticClass()->GetClassPathName(), Assets, true);
	for (const FAssetData& Asset : Assets)
		if (const UPWTeamSetup* Setup = Cast<UPWTeamSetup>(Asset.GetAsset()))
			for (const FPWTeamFaction& Faction : Setup->Factions)
				if (!Faction.Name.IsNone())
					Names.AddUnique(Faction.Name);
	return Names;
}
