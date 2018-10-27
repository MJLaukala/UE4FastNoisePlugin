// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/GCObject.h"
#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "EditorUndoClient.h"
#include "Toolkits/IToolkitHost.h"
#include "IDetailsView.h"
#include "FastNoiseEditorSettings.h"
#include "IFastNoiseEditor.h"
#include "Widgets/SFastNoiseEditorViewport.h"

class SDockableTab;
class STextBlock;
class STextureEditorViewport;
class UFactory;
class UFastNoise;
class UTexture2D;

extern const FName FastNoisePluginEditorAppIdentifier;

/**
* Implements an Editor toolkit for textures.
*/
class FFastNoiseEditor
	: public IFastNoiseEditor
	, public FEditorUndoClient
	, public FGCObject
{
public:
	static const FName FastNoisePluginEditorAppIdentifier;

	FFastNoiseEditor();

	/**
	* Destructor.
	*/
	virtual ~FFastNoiseEditor();

public:

	/**
	* Edits the specified FastNoise object.
	*
	* @param Mode The tool kit mode.
	* @param InitToolkitHost
	* @param ObjectToEdit The texture object to edit.
	*/
	void InitFastNoiseEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* ObjectToEdit);

public:

	// FAssetEditorToolkit interface

	virtual FString GetDocumentationLink() const override;
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

public:

	// IFastNoiseEditor interface

	virtual void CalculateTextureDimensions(uint32& Width, uint32& Height) const override;
	virtual ESimpleElementBlendMode GetColourChannelBlendMode() const override;
	virtual bool GetFitToViewport() const override;
	virtual UFastNoise* GetFastNoise() const override;
	virtual UTexture2D* GetFastNoiseTexture() const override;
	virtual bool HasValidTextureResource() const override;
	virtual bool GetUseSpecifiedMip() const override;
	virtual double GetZoom() const override;
	virtual void PopulateQuickInfo() override;
	virtual void SetFitToViewport(const bool bFitToViewport) override;
	virtual void SetZoom(double ZoomValue) override;
	virtual void ZoomIn() override;
	virtual void ZoomOut() override;

public:

	// IToolkit interface

	virtual FText GetBaseToolkitName() const override;
	virtual FName GetToolkitFName() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetWorldCentricTabPrefix() const override;

public:

	// FGCObject interface

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

protected:

	// FEditorUndoClient interface

	virtual void PostUndo(bool bSuccess) override;
	virtual void PostRedo(bool bSuccess) override;

protected:

	/**
	* Binds the UI commands to delegates.
	*/
	void BindCommands();

	/**
	* Creates the texture properties details widget.
	*
	* @return The widget.
	*/
	TSharedRef<SWidget> BuildFastNoisePropertiesWidget();

	/**
	* Creates all internal widgets for the tabs to point at.
	*/
	void CreateInternalWidgets();

	/**
	* Builds the toolbar widget for the Texture editor.
	*/
	void ExtendToolBar();

	/**
	* Gets the highest mip map level that this texture supports.
	*
	* @return Mip map level.
	*/
	TOptional<int32> GetMaxMipLevel() const;

private:

	// Callback for toggling the Alpha channel action.
	void HandleAlphaChannelActionExecute();

	// Callback for getting the checked state of the Alpha channel action.
	bool HandleAlphaChannelActionIsChecked() const;

	// Callback for getting the enabled state of the Alpha channel action.
	bool HandleAlphaChannelActionCanExecute() const;

	// Callback for toggling the Blue channel action.
	void HandleBlueChannelActionExecute();

	// Callback for getting the checked state of the Blue channel action.
	bool HandleBlueChannelActionIsChecked() const;

	// Callback for toggling the Checkered Background action.
	void HandleCheckeredBackgroundActionExecute(EFastNoiseEditorBackgrounds Background);

	// Callback for getting the checked state of the Checkered Background action.
	bool HandleCheckeredBackgroundActionIsChecked(EFastNoiseEditorBackgrounds Background);

	// Callback for toggling the Fit To Viewport action.
	void HandleFitToViewportActionExecute();

	// Callback for getting the checked state of the Fit To Viewport action.
	bool HandleFitToViewportActionIsChecked() const;

	// Callback for toggling the Green channel action.
	void HandleGreenChannelActionExecute();

	// Callback for getting the checked state of the Green Channel action.
	bool HandleGreenChannelActionIsChecked() const;

	// Callback for toggling the Red channel action.
	void HandleRedChannelActionExecute();

	// Callback for getting the checked state of the Red Channel action.
	bool HandleRedChannelActionIsChecked() const;

	// Callback for determining whether the Settings action can execute.
	void HandleSettingsActionExecute();

	// Callback for spawning the Properties tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnProperties(const FSpawnTabArgs& Args);

	// Callback for spawning the Viewport tab.
	TSharedRef<SDockTab> HandleTabSpawnerSpawnViewport(const FSpawnTabArgs& Args);

	// Callback for toggling the Texture Border action.
	void HandleTextureBorderActionExecute();

	// Callback for getting the checked state of the Texture Border action.
	bool HandleTextureBorderActionIsChecked() const;

private:

	/** The Texture asset being inspected */
	UTexture2D* FastNoiseTexture;
	UFastNoise* FastNoise;

	/** List of open tool panels; used to ensure only one exists at any one time */
	TMap<FName, TWeakPtr<SDockableTab>> SpawnedToolPanels;

	/** Viewport */
	TSharedPtr<SFastNoiseEditorViewport> FastNoiseViewport;

	/** Properties tab */
	TSharedPtr<SVerticalBox> FastNoiseProperties;

	/** Properties tree view */
	TSharedPtr<class IDetailsView> FastNoisePropertiesWidget;

	/** Quick info text blocks */
	TSharedPtr<STextBlock> ImportedText;
	TSharedPtr<STextBlock> CurrentText;
	TSharedPtr<STextBlock> MaxInGameText;
	TSharedPtr<STextBlock> SizeText;
	TSharedPtr<STextBlock> MethodText;
	TSharedPtr<STextBlock> FormatText;
	TSharedPtr<STextBlock> LODBiasText;
	TSharedPtr<STextBlock> HasAlphaChannelText;
	TSharedPtr<STextBlock> NumMipsText;

	/** If true, displays the red channel */
	bool bIsRedChannel;

	/** If true, displays the green channel */
	bool bIsGreenChannel;

	/** If true, displays the blue channel */
	bool bIsBlueChannel;

	/** If true, displays the alpha channel */
	bool bIsAlphaChannel;

	/** The maximum width/height at which the texture will render in the preview window */
	uint32 PreviewEffectiveTextureWidth;
	uint32 PreviewEffectiveTextureHeight;

	/** The texture's zoom factor. */
	double Zoom;

private:

	// The name of the Viewport tab.
	static const FName ViewportTabId;

	// The name of the Properties tab.
	static const FName PropertiesTabId;
};
