/******************************************************************************
 * Spine Runtimes License Agreement
 * Last updated January 1, 2020. Replaces all prior versions.
 *
 * Copyright (c) 2013-2020, Esoteric Software LLC
 *
 * Integration of the Spine Runtimes into software or otherwise creating
 * derivative works of the Spine Runtimes is permitted under the terms and
 * conditions of Section 2 of the Spine Editor License Agreement:
 * http://esotericsoftware.com/spine-editor-license
 *
 * Otherwise, it is permitted to integrate the Spine Runtimes into software
 * or otherwise create derivative works of the Spine Runtimes (collectively,
 * "Products"), provided that each user of the Products must obtain their own
 * Spine Editor license and redistribution of the Products in any form must
 * include this license and copyright notice.
 *
 * THE SPINE RUNTIMES ARE PROVIDED BY ESOTERIC SOFTWARE LLC "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ESOTERIC SOFTWARE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES,
 * BUSINESS INTERRUPTION, OR LOSS OF USE, DATA, OR PROFITS) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THE SPINE RUNTIMES, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************/

#include "AssetToolsModule.h"
#include "SpineEditorPluginPrivatePCH.h"
#include "SpineSkeletonAssetActions.h"
#include "spine/spine.h"


class FSpineEditorPlugin : public ISpineEditorPlugin {
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;
};

IMPLEMENT_MODULE(FSpineEditorPlugin, SpineEditorPlugin)

void FSpineEditorPlugin::StartupModule() {
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	// register custom category
	const EAssetTypeCategories::Type SpineSkeletonDataCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("SpineSkeletonData")), FText::FromString("SpineSkeletonData"));
	// register custom actions
	const TSharedPtr<IAssetTypeActions> Action = MakeShareable(new FSpineSkeletonAssetActions(SpineSkeletonDataCategory));
	AssetTools.RegisterAssetTypeActions(Action.ToSharedRef());
	// saved it here for unregister later
	CreatedAssetTypeActions.Add(Action);
}

void FSpineEditorPlugin::ShutdownModule() {
	// Unregister all the asset types that we registered
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		for (const auto& CreatedAssetTypeAction : CreatedAssetTypeActions)
		{
			AssetTools.UnregisterAssetTypeActions(CreatedAssetTypeAction.ToSharedRef());
		}
		CreatedAssetTypeActions.Empty();
	}
}
