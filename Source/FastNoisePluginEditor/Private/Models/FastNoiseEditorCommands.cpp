// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FastNoiseEditorCommands.h"

#define LOCTEXT_NAMESPACE "FastNoiseEditorCommands"

void FFastNoiseEditorCommands::RegisterCommands()
{
	UI_COMMAND(RedChannel, "Red", "Toggles the red channel", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(GreenChannel, "Green", "Toggles the green channel", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(BlueChannel, "Blue", "Toggles the blue channel", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(AlphaChannel, "Alpha", "Toggles the alpha channel", EUserInterfaceActionType::ToggleButton, FInputChord());

	UI_COMMAND(CheckeredBackground, "Checkered", "Checkered background pattern behind the texture", EUserInterfaceActionType::RadioButton, FInputChord());
	UI_COMMAND(CheckeredBackgroundFill, "Checkered (Fill)", "Checkered background pattern behind the entire viewport", EUserInterfaceActionType::RadioButton, FInputChord());
	UI_COMMAND(FitToViewport, "Scale To Fit", "If enabled, the texture will be scaled to fit the viewport", EUserInterfaceActionType::ToggleButton, FInputChord());
	UI_COMMAND(SolidBackground, "Solid Color", "Solid color background", EUserInterfaceActionType::RadioButton, FInputChord());
	UI_COMMAND(TextureBorder, "Draw Border", "If enabled, a border is drawn around the texture", EUserInterfaceActionType::ToggleButton, FInputChord());

	UI_COMMAND(Settings, "Settings...", "Opens the settings for the texture editor", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
