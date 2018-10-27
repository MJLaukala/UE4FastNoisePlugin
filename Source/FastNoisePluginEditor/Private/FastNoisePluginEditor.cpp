// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FastNoisePluginEditor.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "FastNoise.h"
#include "FastNoiseThumbnailRenderer.h"
#include "AssetTypeActions_FastNoise.h"
#include "FastNoiseEditor.h"
#include "ISettingsModule.h"

#define LOCTEXT_NAMESPACE "FFastNoisePluginEditorModule"

void FFastNoisePluginEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	MenuExtensibilityManager = MakeShareable(new FExtensibilityManager);
	ToolBarExtensibilityManager = MakeShareable(new FExtensibilityManager);

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	FastNoiseAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Fast Noise")), LOCTEXT("FastNoiseAssetCategory", "Fast Noise"));
	RegisterAssetTypeAction(AssetTools, MakeShareable(new FAssetTypeActions_FastNoise(FastNoiseAssetCategoryBit)));

	// register settings
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->RegisterSettings("Editor", "ContentEditors", "FastNoisePluginEditor",
			LOCTEXT("FastNoiseEditorSettingsName", "FastNoise Editor"),
			LOCTEXT("FastNoiseEditorSettingsDescription", "Configure the look and feel of the Texture Editor."),
			GetMutableDefault<UFastNoiseEditorSettings>()
		);
	}

	UThumbnailManager::Get().RegisterCustomRenderer(UFastNoise::StaticClass(), UFastNoiseThumbnailRenderer::StaticClass());
}

void FFastNoisePluginEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// unregister settings
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings("Editor", "ContentEditors", "FastNoisePluginEditor");
	}

	if (UObjectInitialized())
	{
		UThumbnailManager::Get().UnregisterCustomRenderer(UFastNoise::StaticClass());
	}

	// Unregister all the asset types that we registered
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (int32 Index = 0; Index < CreatedAssetTypeActions.Num(); ++Index)
		{
			AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeActions[Index].ToSharedRef());
		}
	}
	CreatedAssetTypeActions.Empty();

	MenuExtensibilityManager.Reset();
	ToolBarExtensibilityManager.Reset();
}

TSharedRef<IFastNoiseEditor> FFastNoisePluginEditorModule::CreateFastNoiseEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UFastNoise* FastNoise)
{
	TSharedRef<FFastNoiseEditor> NewFastNoiseEditor(new FFastNoiseEditor());
	NewFastNoiseEditor->InitFastNoiseEditor(Mode, InitToolkitHost, FastNoise);

	return NewFastNoiseEditor;
}

void FFastNoisePluginEditorModule::RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action)
{
	AssetTools.RegisterAssetTypeActions(Action);
	CreatedAssetTypeActions.Add(Action);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFastNoisePluginEditorModule, FastNoisePluginEditor)