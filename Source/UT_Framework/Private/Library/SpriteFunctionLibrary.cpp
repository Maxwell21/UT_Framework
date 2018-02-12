/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "SpriteFunctionLibrary.h"
#include "Engine/LatentActionManager.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "PaperFlipbookComponent.h"
#include "PaperFlipbook.h"

void USpriteFunctionLibrary::PlayFlipbookMontage(UObject* WorldContextObject, struct FLatentActionInfo LatentInfo, UPaperFlipbookComponent* Target, UPaperFlipbook* Flipbook, float Rate/* = 1.f*/)
{
	if (!Target || !Flipbook)
		return;

	// Prepare latent action
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
	{
		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
		if (LatentActionManager.FindExistingAction<FFlipbookMontageAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == NULL)
		{
			float Duration = Flipbook->GetTotalDuration() / Rate;
			Target->SetFlipbook(Flipbook);
			Target->SetPlayRate(Rate);
			Target->PlayFromStart();
			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, new FFlipbookMontageAction(Duration, LatentInfo));
		}
	}
}
