// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "FastNoiseFactory.generated.h"

/**
 * 
 */
UCLASS()
class FASTNOISEPLUGINEDITOR_API UFastNoiseFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Preview"))
	int32 Size;
	
	/* New assets that don't override this function are automatically placed into the "Miscellaneous" category in the editor */
	virtual uint32 GetMenuCategories() const override;


	/* Creates the asset inside the UE4 Editor */
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

	
	
};
