// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Models/FastNoiseEditorCommands.h"

#define LOCTEXT_NAMESPACE "TextureEditorViewOptionsMenu"

/**
 * Static helper class for populating the "View Options" menu in the texture editor's view port.
 */
class FFastNoiseEditorViewOptionsMenu
{
public:

	/**
	 * Creates the menu.
	 *
	 * @param MenuBuilder The builder for the menu that owns this menu.
	 */
	static void MakeMenu( FMenuBuilder& MenuBuilder )
	{
		// color channel options
		MenuBuilder.BeginSection("ChannelSection", LOCTEXT("ChannelsSectionHeader", "Color Channels"));
		{
			MenuBuilder.AddMenuEntry(FFastNoiseEditorCommands::Get().RedChannel);
			MenuBuilder.AddMenuEntry(FFastNoiseEditorCommands::Get().GreenChannel);
			MenuBuilder.AddMenuEntry(FFastNoiseEditorCommands::Get().BlueChannel);
			MenuBuilder.AddMenuEntry(FFastNoiseEditorCommands::Get().AlphaChannel);
		}
		MenuBuilder.EndSection();

		// view port options
		MenuBuilder.BeginSection("ViewportSection", LOCTEXT("ViewportSectionHeader", "Viewport Options"));
		{
			MenuBuilder.AddSubMenu(
				LOCTEXT("Background", "Background"),
				LOCTEXT("BackgroundTooltip", "Set the viewport's background"),
				FNewMenuDelegate::CreateStatic(&FFastNoiseEditorViewOptionsMenu::GenerateBackgroundMenuContent)
			);

			MenuBuilder.AddMenuEntry(FFastNoiseEditorCommands::Get().TextureBorder);
			MenuBuilder.AddMenuEntry(FFastNoiseEditorCommands::Get().FitToViewport);
		}
		MenuBuilder.EndSection();

		MenuBuilder.AddMenuSeparator();
		MenuBuilder.AddMenuEntry(FFastNoiseEditorCommands::Get().Settings);
	}

protected:

	/**
	 * Creates the 'Background' sub-menu.
	 *
	 * @param MenuBuilder The builder for the menu that owns this menu.
	 */
	static void GenerateBackgroundMenuContent( FMenuBuilder& MenuBuilder )
	{
		MenuBuilder.AddMenuEntry(FFastNoiseEditorCommands::Get().CheckeredBackground);
		MenuBuilder.AddMenuEntry(FFastNoiseEditorCommands::Get().CheckeredBackgroundFill);
		MenuBuilder.AddMenuEntry(FFastNoiseEditorCommands::Get().SolidBackground);
	}
};


#undef LOCTEXT_NAMESPACE
