// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FastNoiseEditor.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "Editor.h"
#include "Widgets/Layout/SBorder.h"
#include "Misc/FeedbackContext.h"
#include "Modules/ModuleManager.h"
#include "SlateOptMacros.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "EditorStyleSet.h"
#include "EditorReimportHandler.h"
#include "DeviceProfiles/DeviceProfileManager.h"
#include "Engine/LightMapTexture2D.h"
#include "Engine/ShadowMapTexture2D.h"
#include "Engine/Texture2DDynamic.h"
#include "Engine/TextureCube.h"
#include "Engine/TextureRenderTarget.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/TextureRenderTargetCube.h"
#include "FastNoisePluginEditor.h"
#include "Slate/SceneViewport.h"
#include "PropertyEditorModule.h"
#include "FastNoiseEditorConstants.h"
#include "Models/FastNoiseEditorCommands.h"
#include "Widgets/SFastNoiseEditorViewport.h"
#include "ISettingsModule.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "DeviceProfiles/DeviceProfile.h"

#define LOCTEXT_NAMESPACE "FFastNoiseEditor"

DEFINE_LOG_CATEGORY_STATIC(LogFastNoiseEditor, Log, All);

const FName FFastNoiseEditor::FastNoisePluginEditorAppIdentifier(TEXT("FastNoiseEditorApp"));
const FName FFastNoiseEditor::ViewportTabId(TEXT("FastNoiseEditor_Viewport"));
const FName FFastNoiseEditor::PropertiesTabId(TEXT("FastNoiseEditor_Properties"));


/* FFastNoiseEditor structors
*****************************************************************************/

FFastNoiseEditor::FFastNoiseEditor()
	: FastNoise(nullptr)
{
	//TODO:: Edit dimensions
	// Does this GC?
	int32 delta = 512;
	FastNoiseTexture = UTexture2D::CreateTransient(delta, delta);
	int32 dataSize = delta * delta;
	TArray<FColor> DataArray;
	DataArray.AddDefaulted(dataSize);

	FTexture2DMipMap& Mip = FastNoiseTexture->PlatformData->Mips[0];
	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(Data, DataArray.GetData(), dataSize * sizeof(FColor));
	Mip.BulkData.Unlock();
}

FFastNoiseEditor::~FFastNoiseEditor()
{
	GEditor->UnregisterForUndo(this);
}


/* FAssetEditorToolkit interface
*****************************************************************************/

FString FFastNoiseEditor::GetDocumentationLink() const
{
	//TODO::
	return FString(TEXT(""));
	//return FString(TEXT("Engine/Content/Types/Textures/Properties/Interface"));
}


void FFastNoiseEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_FastNoiseEditor", "FastNoise Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(ViewportTabId, FOnSpawnTab::CreateSP(this, &FFastNoiseEditor::HandleTabSpawnerSpawnViewport))
		.SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(PropertiesTabId, FOnSpawnTab::CreateSP(this, &FFastNoiseEditor::HandleTabSpawnerSpawnProperties))
		.SetDisplayName(LOCTEXT("PropertiesTab", "Details"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}


void FFastNoiseEditor::UnregisterTabSpawners(const TSharedRef<class FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(ViewportTabId);
	InTabManager->UnregisterTabSpawner(PropertiesTabId);
}


void FFastNoiseEditor::InitFastNoiseEditor(const EToolkitMode::Type Mode, const TSharedPtr< class IToolkitHost >& InitToolkitHost, UObject* ObjectToEdit)
{
	FastNoise = CastChecked<UFastNoise>(ObjectToEdit);

	// Support undo/redo
	FastNoise->SetFlags(RF_Transactional);
	GEditor->RegisterForUndo(this);

	// initialize view options
	bIsRedChannel = true;
	bIsGreenChannel = true;
	bIsBlueChannel = true;
	bIsAlphaChannel = true;

	Zoom = 1.0f;

	// Register our commands. This will only register them if not previously registered
	FFastNoiseEditorCommands::Register();

	BindCommands();

	CreateInternalWidgets();

	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_FastNoiseEditor_Layout")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Vertical)
				->SetSizeCoefficient(0.66f)
				->Split
				(
					FTabManager::NewStack()
					->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
					->SetHideTabWell(true)
					->SetSizeCoefficient(0.1f)

				)
				->Split
				(
					FTabManager::NewStack()
					->AddTab(ViewportTabId, ETabState::OpenedTab)
					->SetHideTabWell(true)
					->SetSizeCoefficient(0.9f)
				)
			)
			->Split
			(
				FTabManager::NewStack()
				->AddTab(PropertiesTabId, ETabState::OpenedTab)
				->SetSizeCoefficient(0.33f)
			)
		);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;

	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, FastNoisePluginEditorAppIdentifier, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ObjectToEdit);

	IFastNoisePluginEditor* FastNoiseEditorModule = &FModuleManager::LoadModuleChecked<IFastNoisePluginEditor>("FastNoisePluginEditor");
	AddMenuExtender(FastNoiseEditorModule->GetMenuExtensibilityManager()->GetAllExtenders(GetToolkitCommands(), GetEditingObjects()));

	ExtendToolBar();

	RegenerateMenusAndToolbars();

	// @todo toolkit world centric editing
	/*if(IsWorldCentricAssetEditor())
	{
	SpawnToolkitTab(GetToolbarTabId(), FString(), EToolkitTabSpot::ToolBar);
	SpawnToolkitTab(ViewportTabId, FString(), EToolkitTabSpot::Viewport);
	SpawnToolkitTab(PropertiesTabId, FString(), EToolkitTabSpot::Details);
	}*/
}


/* ITextureEditorToolkit interface
*****************************************************************************/

void FFastNoiseEditor::CalculateTextureDimensions(uint32& Width, uint32& Height) const
{
	uint32 ImportedWidth = FastNoiseTexture->Source.GetSizeX();
	uint32 ImportedHeight = FastNoiseTexture->Source.GetSizeY();

	// if Original Width and Height are 0, use the saved current width and height
	if ((ImportedWidth == 0) && (ImportedHeight == 0))
	{
		ImportedWidth = FastNoiseTexture->GetSurfaceWidth();
		ImportedHeight = FastNoiseTexture->GetSurfaceHeight();
	}

	Width = ImportedWidth;
	Height = ImportedHeight;


	// catch if the Width and Height are still zero for some reason
	if ((Width == 0) || (Height == 0))
	{
		Width = 0;
		Height = 0;

		return;
	}

	// See if we need to uniformly scale it to fit in viewport
	// Cap the size to effective dimensions
	uint32 ViewportW = FastNoiseViewport->GetViewport()->GetSizeXY().X;
	uint32 ViewportH = FastNoiseViewport->GetViewport()->GetSizeXY().Y;
	uint32 MaxWidth;
	uint32 MaxHeight;

	const bool bFitToViewport = GetFitToViewport();
	if (bFitToViewport)
	{
		// Subtract off the viewport space devoted to padding (2 * PreviewPadding)
		// so that the texture is padded on all sides
		MaxWidth = ViewportW;
		MaxHeight = ViewportH;

		// First, scale up based on the size of the viewport
		if (MaxWidth > MaxHeight)
		{
			Height = Height * MaxWidth / Width;
			Width = MaxWidth;
		}
		else
		{
			Width = Width * MaxHeight / Height;
			Height = MaxHeight;
		}

		// then, scale again if our width and height is impacted by the scaling
		if (Width > MaxWidth)
		{
			Height = Height * MaxWidth / Width;
			Width = MaxWidth;
		}
		if (Height > MaxHeight)
		{
			Width = Width * MaxHeight / Height;
			Height = MaxHeight;
		}
	}
	else
	{
		Width = PreviewEffectiveTextureWidth * Zoom;
		Height = PreviewEffectiveTextureHeight * Zoom;
	}
}


ESimpleElementBlendMode FFastNoiseEditor::GetColourChannelBlendMode() const
{
	if (FastNoiseTexture && (FastNoiseTexture->CompressionSettings == TC_Grayscale || FastNoiseTexture->CompressionSettings == TC_Alpha))
	{
		return SE_BLEND_Opaque;
	}

	// Add the red, green, blue, alpha and desaturation flags to the enum to identify the chosen filters
	uint32 Result = (uint32)SE_BLEND_RGBA_MASK_START;
	Result += bIsRedChannel ? (1 << 0) : 0;
	Result += bIsGreenChannel ? (1 << 1) : 0;
	Result += bIsBlueChannel ? (1 << 2) : 0;
	Result += bIsAlphaChannel ? (1 << 3) : 0;

	return (ESimpleElementBlendMode)Result;
}


bool FFastNoiseEditor::GetFitToViewport() const
{
	const UFastNoiseEditorSettings& Settings = *GetDefault<UFastNoiseEditorSettings>();
	return Settings.FitToViewport;
}


UFastNoise* FFastNoiseEditor::GetFastNoise() const
{
	return FastNoise;
}


UTexture2D* FFastNoiseEditor::GetFastNoiseTexture() const
{
	return FastNoiseTexture;
}


bool FFastNoiseEditor::HasValidTextureResource() const
{
	return FastNoiseTexture != nullptr && FastNoiseTexture->Resource != nullptr;
}


bool FFastNoiseEditor::GetUseSpecifiedMip() const
{
	return false;
}


double FFastNoiseEditor::GetZoom() const
{
	return Zoom;
}


void FFastNoiseEditor::PopulateQuickInfo()
{
	UTexture2D* Texture2D = Cast<UTexture2D>(FastNoiseTexture);

	const uint32 SurfaceWidth = (uint32)FastNoiseTexture->GetSurfaceWidth();
	const uint32 SurfaceHeight = (uint32)FastNoiseTexture->GetSurfaceHeight();

	const uint32 ImportedWidth = FMath::Max<uint32>(SurfaceWidth, FastNoiseTexture->Source.GetSizeX());
	const uint32 ImportedHeight = FMath::Max<uint32>(SurfaceHeight, FastNoiseTexture->Source.GetSizeY());

	const int32 ActualMipBias = Texture2D->GetNumMips() - Texture2D->GetNumResidentMips();
	const uint32 ActualWidth = FMath::Max<uint32>(SurfaceWidth >> ActualMipBias, 1);
	const uint32 ActualHeight = FMath::Max<uint32>(SurfaceHeight >> ActualMipBias, 1);

	PreviewEffectiveTextureWidth = 0;
	PreviewEffectiveTextureHeight = 0;

	// In game max bias and dimensions
	const int32 MaxResMipBias = Texture2D->GetNumMips() - Texture2D->GetNumMipsAllowed(true);
	const uint32 MaxInGameWidth = FMath::Max<uint32>(SurfaceWidth >> MaxResMipBias, 1);
	const uint32 MaxInGameHeight = FMath::Max<uint32>(SurfaceHeight >> MaxResMipBias, 1);

	// Texture asset size
	const uint32 Size = (FastNoiseTexture->GetResourceSizeBytes(EResourceSizeMode::Exclusive) + 512) / 1024;

	FNumberFormattingOptions SizeOptions;
	SizeOptions.UseGrouping = false;
	SizeOptions.MaximumFractionalDigits = 0;

	FNumberFormattingOptions Options;
	Options.UseGrouping = false;

	FText CubemapAdd;

	ImportedText->SetText(FText::Format(NSLOCTEXT("FastNoisePlugingEditor", "QuickInfo_Imported", "Imported: {0}x{1}"), FText::AsNumber(ImportedWidth, &Options), FText::AsNumber(ImportedHeight, &Options)));
	CurrentText->SetText(FText::Format(NSLOCTEXT("FastNoisePlugingEditor", "QuickInfo_Displayed", "Displayed: {0}x{1}{2}"), FText::AsNumber(PreviewEffectiveTextureWidth, &Options), FText::AsNumber(PreviewEffectiveTextureHeight, &Options), CubemapAdd));
	MaxInGameText->SetText(FText::Format(NSLOCTEXT("FastNoisePlugingEditor", "QuickInfo_MaxInGame", "Max In-Game: {0}x{1}{2}"), FText::AsNumber(MaxInGameWidth, &Options), FText::AsNumber(MaxInGameHeight, &Options), CubemapAdd));
	SizeText->SetText(FText::Format(NSLOCTEXT("FastNoisePlugingEditor", "QuickInfo_ResourceSize", "Resource Size: {0} Kb"), FText::AsNumber(Size, &SizeOptions)));
	MethodText->SetText(FText::Format(NSLOCTEXT("FastNoisePlugingEditor", "QuickInfo_Method", "Method: {0}"), FastNoiseTexture->NeverStream ? NSLOCTEXT("FastNoisePluginEditor", "QuickInfo_MethodNotStreamed", "Not Streamed") : NSLOCTEXT("FastNoisePluginEditor", "QuickInfo_MethodStreamed", "Streamed")));
	LODBiasText->SetText(FText::Format(NSLOCTEXT("FastNoisePlugingEditor", "QuickInfo_LODBias", "Combined LOD Bias: {0}"), FText::AsNumber(FastNoiseTexture->GetCachedLODBias())));

	int32 TextureFormatIndex = PF_MAX;

	TextureFormatIndex = Texture2D->GetPixelFormat();

	if (TextureFormatIndex != PF_MAX)
	{
		FormatText->SetText(FText::Format(NSLOCTEXT("FastNoisePlugingEditor", "QuickInfo_Format", "Format: {0}"), FText::FromString(GPixelFormats[TextureFormatIndex].Name)));
	}

	int32 NumMips = Texture2D->GetNumMips();

	NumMipsText->SetText(FText::Format(NSLOCTEXT("FastNoisePlugingEditor", "QuickInfo_NumMips", "Number of Mips: {0}"), FText::AsNumber(NumMips)));

	HasAlphaChannelText->SetText(FText::Format(NSLOCTEXT("FastNoisePlugingEditor", "QuickInfo_HasAlphaChannel", "Has Alpha Channel: {0}"),
		Texture2D->HasAlphaChannel() ? NSLOCTEXT("FastNoisePlugingEditor", "True", "True") : NSLOCTEXT("FastNoisePlugingEditor", "False", "False")));

	HasAlphaChannelText->SetVisibility(EVisibility::Visible);
}


void FFastNoiseEditor::SetFitToViewport(const bool bFitToViewport)
{
	UFastNoiseEditorSettings& Settings = *GetMutableDefault<UFastNoiseEditorSettings>();
	Settings.FitToViewport = bFitToViewport;
	Settings.PostEditChange();
}


void FFastNoiseEditor::SetZoom(double ZoomValue)
{
	Zoom = FMath::Clamp(ZoomValue, MinZoom, MaxZoom);
	SetFitToViewport(false);
}


void FFastNoiseEditor::ZoomIn()
{
	SetZoom(Zoom + ZoomStep);
}


void FFastNoiseEditor::ZoomOut()
{
	SetZoom(Zoom - ZoomStep);
}


/* IToolkit interface
*****************************************************************************/

FText FFastNoiseEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "FastNoise Editor");
}


FName FFastNoiseEditor::GetToolkitFName() const
{
	return FName("FastNoisePlugingEditor");
}


FLinearColor FFastNoiseEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}


FString FFastNoiseEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Texture ").ToString();
}


/* FGCObject interface
*****************************************************************************/

void FFastNoiseEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(FastNoiseTexture);
	FastNoiseViewport->AddReferencedObjects(Collector);
}


/* FEditorUndoClient interface
*****************************************************************************/

void FFastNoiseEditor::PostUndo(bool bSuccess)
{
}


void FFastNoiseEditor::PostRedo(bool bSuccess)
{
	PostUndo(bSuccess);
}


/* FFastNoiseEditor implementation
*****************************************************************************/

void FFastNoiseEditor::BindCommands()
{
	const FFastNoiseEditorCommands& Commands = FFastNoiseEditorCommands::Get();

	ToolkitCommands->MapAction(
		Commands.RedChannel,
		FExecuteAction::CreateSP(this, &FFastNoiseEditor::HandleRedChannelActionExecute),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FFastNoiseEditor::HandleRedChannelActionIsChecked));

	ToolkitCommands->MapAction(
		Commands.GreenChannel,
		FExecuteAction::CreateSP(this, &FFastNoiseEditor::HandleGreenChannelActionExecute),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FFastNoiseEditor::HandleGreenChannelActionIsChecked));

	ToolkitCommands->MapAction(
		Commands.BlueChannel,
		FExecuteAction::CreateSP(this, &FFastNoiseEditor::HandleBlueChannelActionExecute),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FFastNoiseEditor::HandleBlueChannelActionIsChecked));

	ToolkitCommands->MapAction(
		Commands.AlphaChannel,
		FExecuteAction::CreateSP(this, &FFastNoiseEditor::HandleAlphaChannelActionExecute),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FFastNoiseEditor::HandleAlphaChannelActionIsChecked));

	ToolkitCommands->MapAction(
		Commands.FitToViewport,
		FExecuteAction::CreateSP(this, &FFastNoiseEditor::HandleFitToViewportActionExecute),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FFastNoiseEditor::HandleFitToViewportActionIsChecked));

	ToolkitCommands->MapAction(
		Commands.CheckeredBackground,
		FExecuteAction::CreateSP(this, &FFastNoiseEditor::HandleCheckeredBackgroundActionExecute, FastNoiseEditorBackground_Checkered),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FFastNoiseEditor::HandleCheckeredBackgroundActionIsChecked, FastNoiseEditorBackground_Checkered));

	ToolkitCommands->MapAction(
		Commands.CheckeredBackgroundFill,
		FExecuteAction::CreateSP(this, &FFastNoiseEditor::HandleCheckeredBackgroundActionExecute, FastNoiseEditorBackground_CheckeredFill),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FFastNoiseEditor::HandleCheckeredBackgroundActionIsChecked, FastNoiseEditorBackground_CheckeredFill));

	ToolkitCommands->MapAction(
		Commands.SolidBackground,
		FExecuteAction::CreateSP(this, &FFastNoiseEditor::HandleCheckeredBackgroundActionExecute, FastNoiseEditorBackground_SolidColor),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FFastNoiseEditor::HandleCheckeredBackgroundActionIsChecked, FastNoiseEditorBackground_SolidColor));

	ToolkitCommands->MapAction(
		Commands.TextureBorder,
		FExecuteAction::CreateSP(this, &FFastNoiseEditor::HandleTextureBorderActionExecute),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FFastNoiseEditor::HandleTextureBorderActionIsChecked));

	ToolkitCommands->MapAction(
		Commands.Settings,
		FExecuteAction::CreateSP(this, &FFastNoiseEditor::HandleSettingsActionExecute));
}


TSharedRef<SWidget> FFastNoiseEditor::BuildFastNoisePropertiesWidget()
{
	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FastNoisePropertiesWidget = PropertyModule.CreateDetailView(Args);
	FastNoisePropertiesWidget->SetObject(FastNoise);

	return FastNoisePropertiesWidget.ToSharedRef();
}

void FFastNoiseEditor::CreateInternalWidgets()
{
	FastNoiseViewport = SNew(SFastNoiseEditorViewport, SharedThis(this));

	FastNoiseProperties = SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(2.0f)
		[
			SNew(SBorder)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
		.FillWidth(0.5f)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(4.0f)
		[
			SAssignNew(ImportedText, STextBlock)
		]

	+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(4.0f)
		[
			SAssignNew(CurrentText, STextBlock)
		]

	+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(4.0f)
		[
			SAssignNew(MaxInGameText, STextBlock)
		]

	+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(4.0f)
		[
			SAssignNew(SizeText, STextBlock)
		]

	+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(4.0f)
		[
			SAssignNew(HasAlphaChannelText, STextBlock)
		]
		]

	+ SHorizontalBox::Slot()
		.FillWidth(0.5f)
		[
			SNew(SVerticalBox)

			+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(4.0f)
		[
			SAssignNew(MethodText, STextBlock)
		]

	+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(4.0f)
		[
			SAssignNew(FormatText, STextBlock)
		]

	+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(4.0f)
		[
			SAssignNew(LODBiasText, STextBlock)
		]

	+ SVerticalBox::Slot()
		.AutoHeight()
		.VAlign(VAlign_Center)
		.Padding(4.0f)
		[
			SAssignNew(NumMipsText, STextBlock)
		]
		]
			]
		]

	+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.Padding(2.0f)
		[
			SNew(SBorder)
			.Padding(4.0f)
		[
			BuildFastNoisePropertiesWidget()
		]
		];
}


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void FFastNoiseEditor::ExtendToolBar()
{
	IFastNoisePluginEditor* TextureEditorModule = &FModuleManager::LoadModuleChecked<IFastNoisePluginEditor>("FastNoisePluginEditor");
	AddToolbarExtender(TextureEditorModule->GetToolBarExtensibilityManager()->GetAllExtenders(GetToolkitCommands(), GetEditingObjects()));
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION


TOptional<int32> FFastNoiseEditor::GetMaxMipLevel() const
{
	return FastNoiseTexture->GetNumMips() - 1;
}


/* FFastNoiseEditor callbacks
*****************************************************************************/

bool FFastNoiseEditor::HandleAlphaChannelActionCanExecute() const
{
	return FastNoiseTexture->HasAlphaChannel();
}


void FFastNoiseEditor::HandleAlphaChannelActionExecute()
{
	bIsAlphaChannel = !bIsAlphaChannel;
}


bool FFastNoiseEditor::HandleAlphaChannelActionIsChecked() const
{
	return bIsAlphaChannel;
}


void FFastNoiseEditor::HandleBlueChannelActionExecute()
{
	bIsBlueChannel = !bIsBlueChannel;
}


bool FFastNoiseEditor::HandleBlueChannelActionIsChecked() const
{
	return bIsBlueChannel;
}


void FFastNoiseEditor::HandleCheckeredBackgroundActionExecute(EFastNoiseEditorBackgrounds Background)
{
	UFastNoiseEditorSettings& Settings = *GetMutableDefault<UFastNoiseEditorSettings>();
	Settings.Background = Background;
	Settings.PostEditChange();
}


bool FFastNoiseEditor::HandleCheckeredBackgroundActionIsChecked(EFastNoiseEditorBackgrounds Background)
{
	const UFastNoiseEditorSettings& Settings = *GetDefault<UFastNoiseEditorSettings>();

	return (Background == Settings.Background);
}


void FFastNoiseEditor::HandleFitToViewportActionExecute()
{
	ToggleFitToViewport();
}


bool FFastNoiseEditor::HandleFitToViewportActionIsChecked() const
{
	return GetFitToViewport();
}


void FFastNoiseEditor::HandleGreenChannelActionExecute()
{
	bIsGreenChannel = !bIsGreenChannel;
}


bool FFastNoiseEditor::HandleGreenChannelActionIsChecked() const
{
	return bIsGreenChannel;
}


void FFastNoiseEditor::HandleRedChannelActionExecute()
{
	bIsRedChannel = !bIsRedChannel;
}


bool FFastNoiseEditor::HandleRedChannelActionIsChecked() const
{
	return bIsRedChannel;
}


void FFastNoiseEditor::HandleSettingsActionExecute()
{
	FModuleManager::LoadModuleChecked<ISettingsModule>("Settings").ShowViewer("Editor", "ContentEditors", "FastNoisePluginEditor");
}


TSharedRef<SDockTab> FFastNoiseEditor::HandleTabSpawnerSpawnProperties(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == PropertiesTabId);

	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("FastNoisePluginEditor.Tabs.Properties"))
		.Label(LOCTEXT("FastNoisePropertiesTitle", "Details"))
		[
			FastNoiseProperties.ToSharedRef()
		];

	PopulateQuickInfo();

	return SpawnedTab;
}


TSharedRef<SDockTab> FFastNoiseEditor::HandleTabSpawnerSpawnViewport(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == ViewportTabId);

	return SNew(SDockTab)
		.Label(LOCTEXT("FastNoiseViewportTitle", "Viewport"))
		[
			FastNoiseViewport.ToSharedRef()
		];
}


void FFastNoiseEditor::HandleTextureBorderActionExecute()
{
	UFastNoiseEditorSettings& Settings = *GetMutableDefault<UFastNoiseEditorSettings>();
	Settings.TextureBorderEnabled = !Settings.TextureBorderEnabled;
	Settings.PostEditChange();
}


bool FFastNoiseEditor::HandleTextureBorderActionIsChecked() const
{
	const UFastNoiseEditorSettings& Settings = *GetDefault<UFastNoiseEditorSettings>();

	return Settings.TextureBorderEnabled;
}


#undef LOCTEXT_NAMESPACE
