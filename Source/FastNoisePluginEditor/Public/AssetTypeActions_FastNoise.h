#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture.h"
#include "Toolkits/IToolkitHost.h"
#include "AssetTypeActions_Base.h"
#include "FastNoise.h"


class FAssetTypeActions_FastNoise : public FAssetTypeActions_Base
{
public:
	FAssetTypeActions_FastNoise(EAssetTypeCategories::Type InAssetCategory);

	// IAssetTypeActions Implementation
	virtual FText GetName() const override { return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_FastNoise", "FastNoise"); }
	virtual UClass* GetSupportedClass() const override { return UFastNoise::StaticClass(); }
	virtual FColor GetTypeColor() const override { return FColor(192, 64, 64); }
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual uint32 GetCategories() override;
	virtual void GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const override { };

private:
	EAssetTypeCategories::Type MyAssetCategory;

};
