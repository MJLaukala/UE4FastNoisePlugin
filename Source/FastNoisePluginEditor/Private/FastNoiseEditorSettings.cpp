// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FastNoiseEditorSettings.h"
#include "CoreMinimal.h"


UFastNoiseEditorSettings::UFastNoiseEditorSettings( const FObjectInitializer& ObjectInitializer )
	: Super(ObjectInitializer)
	, Background(FastNoiseEditorBackground_Checkered)
	, BackgroundColor(FColor::Black)
	, CheckerColorOne(FColor(128, 128, 128))
	, CheckerColorTwo(FColor(64, 64, 64))
	, CheckerSize(32)
	, FitToViewport(true)
	, TextureBorderColor(FColor::White)
	, TextureBorderEnabled(true)
{ }
