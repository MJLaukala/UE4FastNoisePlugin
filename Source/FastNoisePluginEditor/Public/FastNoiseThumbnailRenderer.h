// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThumbnailRendering/DefaultSizedThumbnailRenderer.h"
#include "Engine/Texture2D.h"
#include "FastNoiseThumbnailRenderer.generated.h"

/**
 * 
 */
UCLASS()
class FASTNOISEPLUGINEDITOR_API UFastNoiseThumbnailRenderer : public UDefaultSizedThumbnailRenderer
{
	GENERATED_UCLASS_BODY()

		virtual void BeginDestroy() override;
	
	// Begin UThumbnailRenderer Object
	virtual void Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas) override;
	virtual bool AllowsRealtimeThumbnails(UObject* Object) const override;
	// End UThumbnailRenderer Object
	
};
