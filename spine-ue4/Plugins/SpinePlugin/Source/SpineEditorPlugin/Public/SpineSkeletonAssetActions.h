#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"
#include "SpineSkeletonDataAsset.h"

class FSpineSkeletonAssetActions : public FAssetTypeActions_Base
{
public:

	FSpineSkeletonAssetActions(EAssetTypeCategories::Type InAssetCategory);

	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;

	virtual UClass* GetSupportedClass() const override;
	virtual bool IsImportedAsset() const override;

	virtual bool HasActions(const TArray<UObject*>& InObjects) const override;
	virtual void GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const override;

private:

	EAssetTypeCategories::Type MyAssetCategory;
};
