// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "AssetTypeActions_FastNoise.h"
#include "Toolkits/IToolkit.h"
#include "FastNoiseEditor.h"
#include "FastNoisePluginEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"


FAssetTypeActions_FastNoise::FAssetTypeActions_FastNoise(EAssetTypeCategories::Type InAssetCategory)
	: MyAssetCategory(InAssetCategory)
{

}

void FAssetTypeActions_FastNoise::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto FastNoise = Cast<UFastNoise>(*ObjIt);
		if (FastNoise != NULL)
		{
			IFastNoisePluginEditor* FastNoiseEditor = &FModuleManager::LoadModuleChecked<IFastNoisePluginEditor>("FastNoisePluginEditor");
			FastNoiseEditor->CreateFastNoiseEditor(Mode, EditWithinLevelEditor, FastNoise);
		}
	}
}

uint32 FAssetTypeActions_FastNoise::GetCategories()
{
	return MyAssetCategory;
}


#undef LOCTEXT_NAMESPACE
