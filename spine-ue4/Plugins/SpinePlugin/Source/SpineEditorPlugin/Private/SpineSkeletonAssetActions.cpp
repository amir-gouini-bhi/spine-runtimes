#include "SpineSkeletonAssetActions.h"

#define LOCTEXT_NAMESPACE "Spine"

namespace Constants
{
	FColor SpineSkeletonDataColor = FColor(255, 50, 20);
}

FSpineSkeletonAssetActions::FSpineSkeletonAssetActions(EAssetTypeCategories::Type InAssetCategory)
{
	MyAssetCategory = InAssetCategory;
}

FText FSpineSkeletonAssetActions::GetName() const
{
	return LOCTEXT("FSpineSkeletonAssetActionsGetName", "Spine Skeleton Asset");
}

FColor FSpineSkeletonAssetActions::GetTypeColor() const
{
	return Constants::SpineSkeletonDataColor;
}

uint32 FSpineSkeletonAssetActions::GetCategories()
{
	return MyAssetCategory;
}

UClass* FSpineSkeletonAssetActions::GetSupportedClass() const
{
	return USpineSkeletonDataAsset::StaticClass();
}

bool FSpineSkeletonAssetActions::IsImportedAsset() const
{
	return true;
}

bool FSpineSkeletonAssetActions::HasActions(const TArray<UObject*>& InObjects) const
{
	return false;
}

void FSpineSkeletonAssetActions::GetResolvedSourceFilePaths(const TArray<UObject*>& TypeAssets, TArray<FString>& OutSourceFilePaths) const
{
	for (const auto& TypeAsset : TypeAssets)
	{
		USpineSkeletonDataAsset* Asset = Cast<USpineSkeletonDataAsset>(TypeAsset);
		if (Asset != nullptr)
		{
			const FName FilePath = Asset->GetSkeletonDataFileName();
			OutSourceFilePaths.Add(FilePath.ToString());
		}
		else
		{
			OutSourceFilePaths.Add("");
		}
	}
}

#undef LOCTEXT_NAMESPACE
