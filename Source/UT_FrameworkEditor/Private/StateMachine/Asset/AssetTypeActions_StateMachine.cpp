/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "AssetTypeActions_StateMachine.h"
#include "UT_FrameworkEditor.h"
#include "Engine/Blueprint.h"
#include "StateMachineBlueprint.h"
#include "MessageDialog.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_StateMachine::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_StateMachine", "State Machine");
}

FColor FAssetTypeActions_StateMachine::GetTypeColor() const
{
	return FColor(0.301921f, 0.827451f, 0.396078f);
}

UClass* FAssetTypeActions_StateMachine::GetSupportedClass() const
{
	return UStateMachineBlueprint::StaticClass();
}

void FAssetTypeActions_StateMachine::OpenAssetEditor( const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor )
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		auto Blueprint = Cast<UBlueprint>(*ObjIt);
		if (Blueprint && Blueprint->SkeletonGeneratedClass && Blueprint->GeneratedClass && Blueprint->IsA<UStateMachineBlueprint>())
		{
			FUT_FrameworkEditorModule* FrameworkEditorModule = &FUT_FrameworkEditorModule::Get();
			if (FrameworkEditorModule)
				FrameworkEditorModule->CreateStateMachineEditor(Mode, EditWithinLevelEditor, Cast<UStateMachineBlueprint>(Blueprint));
		}
		else
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("FailedToLoadNpcBehaviorBlueprint", "State machine Blueprint could not be loaded because it derives from an invalid class.\nCheck to make sure the parent class for this blueprint hasn't been removed!"));
	}
}

uint32 FAssetTypeActions_StateMachine::GetCategories()
{
	return EAssetTypeCategories::Gameplay;
}

#undef LOCTEXT_NAMESPACE
