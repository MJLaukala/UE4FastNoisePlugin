// Fill out your copyright notice in the Description page of Project Settings.

#include "FastNoiseFactory.h"
#include "AssetTypeCategories.h"
#include "FastNoise.h"

UFastNoiseFactory::UFastNoiseFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UFastNoise::StaticClass();
}

uint32 UFastNoiseFactory::GetMenuCategories() const
{
	//Let's place this asset in the Blueprints category in the Editor
	return EAssetTypeCategories::Blueprint;
}

UObject* UFastNoiseFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UFastNoise>(InParent, Class, Name, Flags | RF_Transactional);
}

