// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "SFastNoiseEditorViewport.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Widgets/SToolTip.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/SViewport.h"
#include "Widgets/Input/SSlider.h"
#include "Engine/Texture.h"
#include "Slate/SceneViewport.h"
#include "FastNoiseEditorConstants.h"
#include "Widgets/SFastNoiseEditorViewportToolbar.h"
#include "Widgets/Input/SNumericEntryBox.h"


#define LOCTEXT_NAMESPACE "SFastNoiseEditorViewport"

// Specifies the maximum allowed exposure bias.
const int32 MaxExposure = 10;

// Specifies the minimum allowed exposure bias.
const int32 MinExposure = -10;


/* SFastNoiseEditorViewport interface
 *****************************************************************************/

void SFastNoiseEditorViewport::AddReferencedObjects( FReferenceCollector& Collector )
{
	ViewportClient->AddReferencedObjects(Collector);
}


void SFastNoiseEditorViewport::Construct( const FArguments& InArgs, const TSharedRef<IFastNoiseEditor>& InToolkit )
{
	ExposureBias = 0;
	bIsRenderingEnabled = true;
	ToolkitPtr = InToolkit;
	
	// create zoom menu
	FMenuBuilder ZoomMenuBuilder(true, NULL);
	{
		FUIAction Zoom25Action(FExecuteAction::CreateSP(this, &SFastNoiseEditorViewport::HandleZoomMenuEntryClicked, 0.25));
		ZoomMenuBuilder.AddMenuEntry(LOCTEXT("Zoom25Action", "25%"), LOCTEXT("Zoom25ActionHint", "Show the texture at a quarter of its size."), FSlateIcon(), Zoom25Action);

		FUIAction Zoom50Action(FExecuteAction::CreateSP(this, &SFastNoiseEditorViewport::HandleZoomMenuEntryClicked, 0.5));
		ZoomMenuBuilder.AddMenuEntry(LOCTEXT("Zoom50Action", "50%"), LOCTEXT("Zoom50ActionHint", "Show the texture at half its size."), FSlateIcon(), Zoom50Action);

		FUIAction Zoom100Action(FExecuteAction::CreateSP(this, &SFastNoiseEditorViewport::HandleZoomMenuEntryClicked, 1.0));
		ZoomMenuBuilder.AddMenuEntry(LOCTEXT("Zoom100Action", "100%"), LOCTEXT("Zoom100ActionHint", "Show the texture in its original size."), FSlateIcon(), Zoom100Action);

		FUIAction Zoom200Action(FExecuteAction::CreateSP(this, &SFastNoiseEditorViewport::HandleZoomMenuEntryClicked, 2.0));
		ZoomMenuBuilder.AddMenuEntry(LOCTEXT("Zoom200Action", "200%"), LOCTEXT("Zoom200ActionHint", "Show the texture at twice its size."), FSlateIcon(), Zoom200Action);

		FUIAction Zoom400Action(FExecuteAction::CreateSP(this, &SFastNoiseEditorViewport::HandleZoomMenuEntryClicked, 4.0));
		ZoomMenuBuilder.AddMenuEntry(LOCTEXT("Zoom400Action", "400%"), LOCTEXT("Zoom400ActionHint", "Show the texture at four times its size."), FSlateIcon(), Zoom400Action);

		ZoomMenuBuilder.AddMenuSeparator();

		FUIAction ZoomFitAction(
			FExecuteAction::CreateSP(this, &SFastNoiseEditorViewport::HandleZoomMenuFitClicked), 
			FCanExecuteAction(), 
			FIsActionChecked::CreateSP(this, &SFastNoiseEditorViewport::IsZoomMenuFitChecked)
			);
		ZoomMenuBuilder.AddMenuEntry(LOCTEXT("ZoomFitAction", "Scale To Fit"), LOCTEXT("ZoomFillActionHint", "Scale the texture to fit the viewport."), FSlateIcon(), ZoomFitAction, NAME_None, EUserInterfaceActionType::ToggleButton);
	}

	FText TextureName = FText::GetEmpty();
	
	if (InToolkit->GetFastNoiseTexture() != nullptr)
	{
		FText FormattedText = InToolkit->HasValidTextureResource() ? FText::FromString(TEXT("{0}")) : LOCTEXT( "InvalidTexture", "{0} (Invalid Texture)");

		TextureName = FText::Format(FormattedText, FText::FromName(InToolkit->GetFastNoiseTexture()->GetFName()));
	}

	this->ChildSlot
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
			.FillHeight(1.0f)
			[
				SNew(SHorizontalBox)

				+ SHorizontalBox::Slot()
					.FillWidth(1.0f)
					[
						SNew(SVerticalBox)

						+ SVerticalBox::Slot()
							.FillHeight(1)
							[
								SNew(SOverlay)

								// viewport canvas
								+ SOverlay::Slot()
									[
										SAssignNew(ViewportWidget, SViewport)
											.EnableGammaCorrection(false)
											.IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute())
											.ShowEffectWhenDisabled(false)
											.EnableBlending(true)
											.ToolTip(SNew(SToolTip).Text(this, &SFastNoiseEditorViewport::GetDisplayedResolution))
									]
	
								// tool bar
								+ SOverlay::Slot()
									.Padding(2.0f)
									.VAlign(VAlign_Top)
									[
										SNew(SHorizontalBox)

										+ SHorizontalBox::Slot()
											.AutoWidth()
											[
												SNew(SFastNoiseEditorViewportToolbar, InToolkit->GetToolkitCommands())
											]

										+ SHorizontalBox::Slot()
											.FillWidth(1.0f)
											.Padding(4.0f, 0.0f)
											.HAlign(HAlign_Right)
											.VAlign(VAlign_Center)
											[
												SNew(STextBlock)
													.Text(TextureName)
											]
									]
							]
					]

				+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						// vertical scroll bar
						SAssignNew(TextureViewportVerticalScrollBar, SScrollBar)
							.Visibility(this, &SFastNoiseEditorViewport::HandleVerticalScrollBarVisibility)
							.OnUserScrolled(this, &SFastNoiseEditorViewport::HandleVerticalScrollBarScrolled)
					]
			]

		+ SVerticalBox::Slot()
			.AutoHeight()
			[
				// horizontal scrollbar
				SAssignNew(TextureViewportHorizontalScrollBar, SScrollBar)
					.Orientation( Orient_Horizontal )
					.Visibility(this, &SFastNoiseEditorViewport::HandleHorizontalScrollBarVisibility)
					.OnUserScrolled(this, &SFastNoiseEditorViewport::HandleHorizontalScrollBarScrolled)
			]

		+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0.0f, 2.0f, 0.0f, 0.0f)
			[
				SNew(SHorizontalBox)

				// exposure bias
				+ SHorizontalBox::Slot()
					.FillWidth(0.3f)
					[
						SNew(SHorizontalBox)
							.Visibility(this, &SFastNoiseEditorViewport::HandleExposureBiasWidgetVisibility)

						+ SHorizontalBox::Slot()
							.AutoWidth()
							.VAlign(VAlign_Center)
							[
								SNew(STextBlock)
									.Text(LOCTEXT("ExposureBiasLabel", "Exposure Bias:"))
							]

						+ SHorizontalBox::Slot()
							.AutoWidth()
							.Padding(4.0f, 0.0f)
							[
								SNew(SNumericEntryBox<int32>)
									.AllowSpin(true)
									.MinSliderValue(MinExposure)
									.MaxSliderValue(MaxExposure)
									.Value(this, &SFastNoiseEditorViewport::HandleExposureBiasBoxValue)
									.OnValueChanged(this, &SFastNoiseEditorViewport::HandleExposureBiasBoxValueChanged)
							]
					]

				// separator
				+ SHorizontalBox::Slot()
					.FillWidth(0.3f)
					[
						SNullWidget::NullWidget
					]

				// zoom slider
				+ SHorizontalBox::Slot()
					.FillWidth(0.3f)
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
							.AutoWidth()
							.VAlign(VAlign_Center)
							[
								SNew(STextBlock)
									.Text(LOCTEXT("ZoomLabel", "Zoom:"))
							]

						+ SHorizontalBox::Slot()
							.FillWidth(1.0f)
							.Padding(4.0f, 0.0f)
							.VAlign(VAlign_Center)
							[
								SNew(SSlider)
									.OnValueChanged(this, &SFastNoiseEditorViewport::HandleZoomSliderChanged)
									.Value(this, &SFastNoiseEditorViewport::HandleZoomSliderValue)
							]

						+ SHorizontalBox::Slot()
							.AutoWidth()
							.VAlign(VAlign_Center)
							[
								SNew(STextBlock)
									.Text(this, &SFastNoiseEditorViewport::HandleZoomPercentageText)
							]

						+ SHorizontalBox::Slot()
							.AutoWidth()
							.Padding(2.0f, 0.0f, 0.0f, 0.0f)
							.VAlign(VAlign_Center)
							[
								SNew(SComboButton)
									.ContentPadding(FMargin(0.0))
									.MenuContent()
									[
										ZoomMenuBuilder.MakeWidget()
									]
							]
					]
			]
	];

	ViewportClient = MakeShareable(new FFastNoiseEditorViewportClient(ToolkitPtr, SharedThis(this)));

	Viewport = MakeShareable(new FSceneViewport(ViewportClient.Get(), ViewportWidget));

	// The viewport widget needs an interface so it knows what should render
	ViewportWidget->SetViewportInterface( Viewport.ToSharedRef() );
}


void SFastNoiseEditorViewport::ModifyCheckerboardTextureColors( )
{
	if (ViewportClient.IsValid())
	{
		ViewportClient->ModifyCheckerboardTextureColors();
	}
}

void SFastNoiseEditorViewport::EnableRendering()
{
	bIsRenderingEnabled = true;
}

void SFastNoiseEditorViewport::DisableRendering()
{
	bIsRenderingEnabled = false;
}

TSharedPtr<FSceneViewport> SFastNoiseEditorViewport::GetViewport( ) const
{
	return Viewport;
}

TSharedPtr<SViewport> SFastNoiseEditorViewport::GetViewportWidget( ) const
{
	return ViewportWidget;
}

TSharedPtr<SScrollBar> SFastNoiseEditorViewport::GetVerticalScrollBar( ) const
{
	return TextureViewportVerticalScrollBar;
}

TSharedPtr<SScrollBar> SFastNoiseEditorViewport::GetHorizontalScrollBar( ) const
{
	return TextureViewportHorizontalScrollBar;
}


/* SWidget overrides
 *****************************************************************************/

void SFastNoiseEditorViewport::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
	if (bIsRenderingEnabled)
	{
		Viewport->Invalidate();
		Viewport->InvalidateDisplay();
	}
}


/* SFastNoiseEditorViewport implementation
 *****************************************************************************/

FText SFastNoiseEditorViewport::GetDisplayedResolution( ) const
{
	return ViewportClient->GetDisplayedResolution();
}


/* SFastNoiseEditorViewport event handlers
 *****************************************************************************/

EVisibility SFastNoiseEditorViewport::HandleExposureBiasWidgetVisibility( ) const
{
	UTexture2D* Texture = ToolkitPtr.Pin()->GetFastNoiseTexture();

	if ((Texture != NULL) && (Texture->CompressionSettings == TC_HDR))
	{
		return EVisibility::Visible;
	}

	return EVisibility::Hidden;
}


TOptional<int32> SFastNoiseEditorViewport::HandleExposureBiasBoxValue( ) const
{
	return ExposureBias;
}


void SFastNoiseEditorViewport::HandleExposureBiasBoxValueChanged( int32 NewExposure )
{
	ExposureBias = NewExposure;
}


void SFastNoiseEditorViewport::HandleHorizontalScrollBarScrolled( float InScrollOffsetFraction )
{
	float Ratio = ViewportClient->GetViewportHorizontalScrollBarRatio();
	float MaxOffset = (Ratio < 1.0f) ? 1.0f - Ratio : 0.0f;
	InScrollOffsetFraction = FMath::Clamp(InScrollOffsetFraction, 0.0f, MaxOffset);

	TextureViewportHorizontalScrollBar->SetState(InScrollOffsetFraction, Ratio);
}


EVisibility SFastNoiseEditorViewport::HandleHorizontalScrollBarVisibility( ) const
{
	if (ViewportClient->GetViewportHorizontalScrollBarRatio() < 1.0f)
	{
		return EVisibility::Visible;
	}
	
	return EVisibility::Collapsed;
}


void SFastNoiseEditorViewport::HandleVerticalScrollBarScrolled( float InScrollOffsetFraction )
{
	float Ratio = ViewportClient->GetViewportVerticalScrollBarRatio();
	float MaxOffset = (Ratio < 1.0f) ? 1.0f - Ratio : 0.0f;
	InScrollOffsetFraction = FMath::Clamp(InScrollOffsetFraction, 0.0f, MaxOffset);

	TextureViewportVerticalScrollBar->SetState(InScrollOffsetFraction, Ratio);
}


EVisibility SFastNoiseEditorViewport::HandleVerticalScrollBarVisibility( ) const
{
	if (ViewportClient->GetViewportVerticalScrollBarRatio() < 1.0f)
	{
		return EVisibility::Visible;
	}
	
	return EVisibility::Collapsed;
}


void SFastNoiseEditorViewport::HandleZoomMenuEntryClicked( double ZoomValue )
{
	ToolkitPtr.Pin()->SetZoom(ZoomValue);
}


void SFastNoiseEditorViewport::HandleZoomMenuFitClicked()
{
	ToolkitPtr.Pin()->ToggleFitToViewport();
}


bool SFastNoiseEditorViewport::IsZoomMenuFitChecked() const
{
	return ToolkitPtr.Pin()->GetFitToViewport();
}

bool SFastNoiseEditorViewport::HasValidTextureResource() const
{
	return ToolkitPtr.Pin()->HasValidTextureResource();
}

FText SFastNoiseEditorViewport::HandleZoomPercentageText( ) const
{
	const bool bFitToViewport = ToolkitPtr.Pin()->GetFitToViewport();
	if(bFitToViewport)
	{
		return LOCTEXT("ZoomFitText", "Fit");
	}

	return FText::AsPercent(ToolkitPtr.Pin()->GetZoom());
}


void SFastNoiseEditorViewport::HandleZoomSliderChanged( float NewValue )
{
	ToolkitPtr.Pin()->SetZoom(NewValue * MaxZoom);
}


float SFastNoiseEditorViewport::HandleZoomSliderValue( ) const
{
	return (ToolkitPtr.Pin()->GetZoom() / MaxZoom);
}


#undef LOCTEXT_NAMESPACE
