// Fill out your copyright notice in the Description page of Project Settings.

#include "FastNoiseThumbnailRenderer.h"
#include "FastNoise.h"
#include "CanvasItem.h"
#include "NumericLimits.h"


UFastNoiseThumbnailRenderer::UFastNoiseThumbnailRenderer(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UFastNoiseThumbnailRenderer::BeginDestroy()
{
	Super::BeginDestroy();
}

void UFastNoiseThumbnailRenderer::Draw(UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas)
{
	UFastNoise* FastNoise = Cast<UFastNoise>(Object);
	if (FastNoise != nullptr)
	{
		if (FastNoise->ThumbnailTexture == nullptr)
		{
			FastNoise->ThumbnailTexture = UTexture2D::CreateTransient(Width, Height);
		}

		if (!FastNoise->ThumbnailTexture->IsValidLowLevel())
		{
			return;
		}

		int32 dataSize = Width * Height;
		TArray<FColor> DataArray;
		DataArray.SetNum(dataSize);
		for (uint32 y = 0; y < Height; y++)
		{
			for (uint32 x = 0; x < Width; x++)
			{
				int idx = (Width * y) + x;

				float xVal = (x + FastNoise->OffsetX) * FastNoise->ThumbnailScale;
				float yVal = (y + FastNoise->OffsetY) * FastNoise->ThumbnailScale;

				float noiseVal = FastNoise->GetValue2D(xVal, yVal);
				noiseVal = (noiseVal + 1.f) / 2.f;

				uint8 val = TNumericLimits<uint8>::Max() * noiseVal;
				DataArray[idx] = FColor(val, val, val);
			}
		}

		FTexture2DMipMap& Mip = FastNoise->ThumbnailTexture->PlatformData->Mips[0];
		void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(Data, DataArray.GetData(), dataSize * sizeof(FColor));
		Mip.BulkData.Unlock();
		FastNoise->ThumbnailTexture->UpdateResource();

		FCanvasTileItem CanvasTile(FVector2D(X, Y), FastNoise->ThumbnailTexture->Resource, FVector2D(Width, Height), FLinearColor::White);
		CanvasTile.BlendMode = SE_BLEND_Translucent;
		CanvasTile.Draw(Canvas);
	}
}

bool UFastNoiseThumbnailRenderer::AllowsRealtimeThumbnails(UObject* Object) const
{
	return true;
}


