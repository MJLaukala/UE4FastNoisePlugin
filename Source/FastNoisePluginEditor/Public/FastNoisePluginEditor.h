// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "AssetTypeCategories.h"
#include "IFastNoiseEditor.h"

class IFastNoisePluginEditor
	: public IModuleInterface
	, public IHasMenuExtensibility
	, public IHasToolBarExtensibility
{
public:
	virtual TSharedRef<IFastNoiseEditor> CreateFastNoiseEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UFastNoise* FastNoise) = 0;
};

class FFastNoisePluginEditorModule : public IFastNoisePluginEditor
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual TSharedRef<IFastNoiseEditor> CreateFastNoiseEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UFastNoise* FastNoise) override;

	virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override
	{
		return MenuExtensibilityManager;
	}

	virtual TSharedPtr<FExtensibilityManager> GetToolBarExtensibilityManager() override
	{
		return ToolBarExtensibilityManager;
	}

private:
	void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action);

private:
	EAssetTypeCategories::Type FastNoiseAssetCategoryBit;
	TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;
	TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;
	TSharedPtr<FExtensibilityManager> ToolBarExtensibilityManager;
};