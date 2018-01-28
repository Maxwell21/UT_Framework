/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "AssetTypeActions_NpcBehavior.h"
#include "UT_FrameworkEditor.h"
#include "Engine/Blueprint.h"
#include "NpcBehaviorBlueprint.h"
#include "AIModule.h"
#include "MessageDialog.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_NpcBehavior::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_NpcBehavior", "Npc Behavior");
}

FColor FAssetTypeActions_NpcBehavior::GetTypeColor() const
{
	return FColor(244, 66, 66);
}

UClass* FAssetTypeActions_NpcBehavior::GetSupportedClass() const
{
	return UNpcBehaviorBlueprint::StaticClass();
}

void FAssetTypeActions_NpcBehavior::OpenAssetEditor( const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor )
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto Blueprint = Cast<UBlueprint>(*ObjIt);
		if (Blueprint && Blueprint->SkeletonGeneratedClass && Blueprint->GeneratedClass && Blueprint->IsA<UNpcBehaviorBlueprint>())
		{
			FUT_FrameworkEditorModule* FrameworkEditorModule = &FUT_FrameworkEditorModule::Get();
			if (FrameworkEditorModule)
				FrameworkEditorModule->CreateNpcEditor(Mode, EditWithinLevelEditor, Cast<UNpcBehaviorBlueprint>(Blueprint));
		}
		else
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("FailedToLoadNpcBehaviorBlueprint", "Npc Behavior Blueprint could not be loaded because it derives from an invalid class.\nCheck to make sure the parent class for this blueprint hasn't been removed!"));
	}
}

uint32 FAssetTypeActions_NpcBehavior::GetCategories()
{
	IAIModule& AIModule = FModuleManager::GetModuleChecked<IAIModule>("AIModule").Get();
	return AIModule.GetAIAssetCategoryBit();
}

#undef LOCTEXT_NAMESPACE
