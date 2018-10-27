// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FastNoiseEditorViewportClient.h"
#include "Widgets/Layout/SScrollBar.h"
#include "CanvasItem.h"
#include "Editor/UnrealEdEngine.h"
#include "Engine/Texture2D.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "Engine/TextureCube.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/TextureRenderTargetCube.h"
#include "UnrealEdGlobals.h"
#include "CubemapUnwrapUtils.h"
#include "Slate/SceneViewport.h"
#include "Texture2DPreview.h"
#include "FastNoiseEditorSettings.h"
#include "CanvasTypes.h"
#include "ImageUtils.h"


/* FFastNoiseEditorViewportClient structors
 *****************************************************************************/

FFastNoiseEditorViewportClient::FFastNoiseEditorViewportClient( TWeakPtr<IFastNoiseEditor> InFastNoiseEditor, TWeakPtr<SFastNoiseEditorViewport> InFastNoiseEditorViewport )
	: FastNoiseEditorPtr(InFastNoiseEditor)
	, FastNoiseEditorViewportPtr(InFastNoiseEditorViewport)
	, CheckerboardTexture(NULL)
{
	check(FastNoiseEditorPtr.IsValid() && FastNoiseEditorViewportPtr.IsValid());

	ModifyCheckerboardTextureColors();
}


FFastNoiseEditorViewportClient::~FFastNoiseEditorViewportClient( )
{
	DestroyCheckerboardTexture();
}


/* FViewportClient interface
 *****************************************************************************/

void FFastNoiseEditorViewportClient::Draw(FViewport* Viewport, FCanvas* Canvas)
{
	if (!FastNoiseEditorPtr.IsValid())
	{
		return;
	}
	
	UTexture2D* Texture = FastNoiseEditorPtr.Pin()->GetFastNoiseTexture();
	FVector2D Ratio = FVector2D(GetViewportHorizontalScrollBarRatio(), GetViewportVerticalScrollBarRatio());
	FVector2D ViewportSize = FVector2D(FastNoiseEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().X, FastNoiseEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().Y);
	FVector2D ScrollBarPos = GetViewportScrollBarPositions();
	int32 YOffset = (Ratio.Y > 1.0f)? ((ViewportSize.Y - (ViewportSize.Y / Ratio.Y)) * 0.5f): 0;
	int32 YPos = YOffset - ScrollBarPos.Y;
	int32 XOffset = (Ratio.X > 1.0f)? ((ViewportSize.X - (ViewportSize.X / Ratio.X)) * 0.5f): 0;
	int32 XPos = XOffset - ScrollBarPos.X;
	
	UpdateScrollBars();

	const UFastNoiseEditorSettings& Settings = *GetDefault<UFastNoiseEditorSettings>();

	Canvas->Clear( Settings.BackgroundColor );

	FastNoiseEditorPtr.Pin()->PopulateQuickInfo();
	
	// Fully stream in the texture before drawing it.
	if (Texture)
	{
		Texture->SetForceMipLevelsToBeResident(30.0f);
		Texture->WaitForStreaming();
	}

	// Figure out the size we need
	uint32 Width, Height;
	FastNoiseEditorPtr.Pin()->CalculateTextureDimensions(Width, Height);

	TRefCountPtr<FBatchedElementParameters> BatchedElementParameters;

	if (GMaxRHIFeatureLevel >= ERHIFeatureLevel::SM4)
	{
		float MipLevel = 0;
		bool bIsNormalMap = Texture->IsNormalMap();
		bool bIsSingleChannel = Texture->CompressionSettings == TC_Grayscale || Texture->CompressionSettings == TC_Alpha;
		BatchedElementParameters = new FBatchedElementTexture2DPreviewParameters(MipLevel, bIsNormalMap, bIsSingleChannel);
	}

	// Draw the background checkerboard pattern in the same size/position as the render texture so it will show up anywhere
	// the texture has transparency
	if (Settings.Background == FastNoiseEditorBackground_CheckeredFill)
	{
		Canvas->DrawTile( 0.0f, 0.0f, Viewport->GetSizeXY().X, Viewport->GetSizeXY().Y, 0.0f, 0.0f, (Viewport->GetSizeXY().X / CheckerboardTexture->GetSizeX()), (Viewport->GetSizeXY().Y / CheckerboardTexture->GetSizeY()), FLinearColor::White, CheckerboardTexture->Resource);
	}
	else if (Settings.Background == FastNoiseEditorBackground_Checkered)
	{
		Canvas->DrawTile( XPos, YPos, Width, Height, 0.0f, 0.0f, (Width / CheckerboardTexture->GetSizeX()), (Height / CheckerboardTexture->GetSizeY()), FLinearColor::White, CheckerboardTexture->Resource);
	}

	float Exposure = FMath::Pow(2.0f, (float)FastNoiseEditorViewportPtr.Pin()->GetExposureBias());

	if ( Texture->Resource != nullptr )
	{
		FCanvasTileItem TileItem( FVector2D( XPos, YPos ), Texture->Resource, FVector2D( Width, Height ), FLinearColor(Exposure, Exposure, Exposure) );
		TileItem.BlendMode = FastNoiseEditorPtr.Pin()->GetColourChannelBlendMode();
		TileItem.BatchedElementParameters = BatchedElementParameters;
		Canvas->DrawItem( TileItem );

		// Draw a white border around the texture to show its extents
		if (Settings.TextureBorderEnabled)
		{
			FCanvasBoxItem BoxItem( FVector2D(XPos, YPos), FVector2D(Width , Height ) );
			BoxItem.SetColor( Settings.TextureBorderColor );
			Canvas->DrawItem( BoxItem );
		}
	}
}


bool FFastNoiseEditorViewportClient::InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool Gamepad)
{
	if (Key == EKeys::MouseScrollUp)
	{
		FastNoiseEditorPtr.Pin()->ZoomIn();

		return true;
	}
	else if (Key == EKeys::MouseScrollDown)
	{
		FastNoiseEditorPtr.Pin()->ZoomOut();

		return true;
	}

	return false;
}


bool FFastNoiseEditorViewportClient::InputGesture(FViewport* Viewport, EGestureEvent GestureType, const FVector2D& GestureDelta, bool bIsDirectionInvertedFromDevice)
{
	const bool LeftMouseButtonDown = Viewport->KeyState(EKeys::LeftMouseButton);
	const bool RightMouseButtonDown = Viewport->KeyState(EKeys::RightMouseButton);

	if (GestureType == EGestureEvent::Scroll && !LeftMouseButtonDown && !RightMouseButtonDown)
	{
		double CurrentZoom = FastNoiseEditorPtr.Pin()->GetZoom();
		FastNoiseEditorPtr.Pin()->SetZoom(CurrentZoom + GestureDelta.Y * 0.01);
		return true;
	}

	return false;
}


void FFastNoiseEditorViewportClient::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(CheckerboardTexture);
}


void FFastNoiseEditorViewportClient::ModifyCheckerboardTextureColors()
{
	DestroyCheckerboardTexture();

	const UFastNoiseEditorSettings& Settings = *GetDefault<UFastNoiseEditorSettings>();
	CheckerboardTexture = FImageUtils::CreateCheckerboardTexture(Settings.CheckerColorOne, Settings.CheckerColorTwo, Settings.CheckerSize);
}


FText FFastNoiseEditorViewportClient::GetDisplayedResolution() const
{
	uint32 Height = 1;
	uint32 Width = 1;
	FastNoiseEditorPtr.Pin()->CalculateTextureDimensions(Width, Height);
	return FText::Format( NSLOCTEXT("FastNoisePluginEditor", "DisplayedResolution", "Displayed: {0}x{1}"), FText::AsNumber( FMath::Max((uint32)1, Width) ), FText::AsNumber( FMath::Max((uint32)1, Height)) );
}


float FFastNoiseEditorViewportClient::GetViewportVerticalScrollBarRatio() const
{
	uint32 Height = 1;
	uint32 Width = 1;
	float WidgetHeight = 1.0f;
	if (FastNoiseEditorViewportPtr.Pin()->GetVerticalScrollBar().IsValid())
	{
		FastNoiseEditorPtr.Pin()->CalculateTextureDimensions(Width, Height);

		WidgetHeight = FastNoiseEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().Y;
	}

	return WidgetHeight / Height;
}


float FFastNoiseEditorViewportClient::GetViewportHorizontalScrollBarRatio() const
{
	uint32 Width = 1;
	uint32 Height = 1;
	float WidgetWidth = 1.0f;
	if (FastNoiseEditorViewportPtr.Pin()->GetHorizontalScrollBar().IsValid())
	{
		FastNoiseEditorPtr.Pin()->CalculateTextureDimensions(Width, Height);

		WidgetWidth = FastNoiseEditorViewportPtr.Pin()->GetViewport()->GetSizeXY().X;
	}

	return WidgetWidth / Width;
}


void FFastNoiseEditorViewportClient::UpdateScrollBars()
{
	TSharedPtr<SFastNoiseEditorViewport> Viewport = FastNoiseEditorViewportPtr.Pin();

	if (!Viewport.IsValid() || !Viewport->GetVerticalScrollBar().IsValid() || !Viewport->GetHorizontalScrollBar().IsValid())
	{
		return;
	}

	float VRatio = GetViewportVerticalScrollBarRatio();
	float HRatio = GetViewportHorizontalScrollBarRatio();
	float VDistFromBottom = Viewport->GetVerticalScrollBar()->DistanceFromBottom();
	float HDistFromBottom = Viewport->GetHorizontalScrollBar()->DistanceFromBottom();

	if (VRatio < 1.0f)
	{
		if (VDistFromBottom < 1.0f)
		{
			Viewport->GetVerticalScrollBar()->SetState(FMath::Clamp(1.0f - VRatio - VDistFromBottom, 0.0f, 1.0f), VRatio);
		}
		else
		{
			Viewport->GetVerticalScrollBar()->SetState(0.0f, VRatio);
		}
	}

	if (HRatio < 1.0f)
	{
		if (HDistFromBottom < 1.0f)
		{
			Viewport->GetHorizontalScrollBar()->SetState(FMath::Clamp(1.0f - HRatio - HDistFromBottom, 0.0f, 1.0f), HRatio);
		}
		else
		{
			Viewport->GetHorizontalScrollBar()->SetState(0.0f, HRatio);
		}
	}
}


FVector2D FFastNoiseEditorViewportClient::GetViewportScrollBarPositions() const
{
	FVector2D Positions = FVector2D::ZeroVector;
	if (FastNoiseEditorViewportPtr.Pin()->GetVerticalScrollBar().IsValid() && FastNoiseEditorViewportPtr.Pin()->GetHorizontalScrollBar().IsValid())
	{
		uint32 Width, Height;
		UTexture2D* Texture = FastNoiseEditorPtr.Pin()->GetFastNoiseTexture();
		float VRatio = GetViewportVerticalScrollBarRatio();
		float HRatio = GetViewportHorizontalScrollBarRatio();
		float VDistFromBottom = FastNoiseEditorViewportPtr.Pin()->GetVerticalScrollBar()->DistanceFromBottom();
		float HDistFromBottom = FastNoiseEditorViewportPtr.Pin()->GetHorizontalScrollBar()->DistanceFromBottom();
	
		FastNoiseEditorPtr.Pin()->CalculateTextureDimensions(Width, Height);

		if ((FastNoiseEditorViewportPtr.Pin()->GetVerticalScrollBar()->GetVisibility() == EVisibility::Visible) && VDistFromBottom < 1.0f)
		{
			Positions.Y = FMath::Clamp(1.0f - VRatio - VDistFromBottom, 0.0f, 1.0f) * Height;
		}
		else
		{
			Positions.Y = 0.0f;
		}

		if ((FastNoiseEditorViewportPtr.Pin()->GetHorizontalScrollBar()->GetVisibility() == EVisibility::Visible) && HDistFromBottom < 1.0f)
		{
			Positions.X = FMath::Clamp(1.0f - HRatio - HDistFromBottom, 0.0f, 1.0f) * Width;
		}
		else
		{
			Positions.X = 0.0f;
		}
	}

	return Positions;
}

void FFastNoiseEditorViewportClient::DestroyCheckerboardTexture()
{
	if (CheckerboardTexture)
	{
		if (CheckerboardTexture->Resource)
		{
			CheckerboardTexture->ReleaseResource();
		}
		CheckerboardTexture->MarkPendingKill();
		CheckerboardTexture = NULL;
	}
}