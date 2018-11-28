/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "LoadSystem.h"
#include "PackageName.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreamingDynamic.h"
#include "LoadSystemLevelScriptActor.h"

TMap<TSoftObjectPtr<UWorld>, ULoadSystem*> ULoadSystem::LoadedLevels = TMap<TSoftObjectPtr<UWorld>, ULoadSystem*>();

ULoadSystem* ULoadSystem::AsyncLoadLevel(UObject* WorldContextObject, TSoftObjectPtr<UWorld> Level, bool VisibleOnLoad)
{
	// Check if the LoadStystem existing
	if (ULoadSystem::LoadedLevels.Contains(Level))
	{
		ULoadSystem* LoadSystemInstance = *ULoadSystem::LoadedLevels.Find(Level);

		return LoadSystemInstance;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	ULoadSystem* LoadSystemInstance = NewObject<ULoadSystem>(World, ULoadSystem::StaticClass(), NAME_None, RF_Transient);
	LoadSystemInstance->SetFlags(RF_StrongRefOnFrame);

	if (World || !Level.IsNull())
	{
		// Setup streaming level object that will load specified map
		LoadSystemInstance->LevelStreaming = NewObject<ULevelStreamingDynamic>(World, ULevelStreamingDynamic::StaticClass(), NAME_None, RF_Transient);
		if (LoadSystemInstance->LevelStreaming)
		{
			LoadSystemInstance->VisibiltyOnLoad = VisibleOnLoad;
			LoadSystemInstance->CurrentLevel    = Level;

			LoadSystemInstance->HandleLoading();
		}
		
		return LoadSystemInstance;
	}
	else
		LoadSystemInstance->OnFailedLoad.Broadcast(nullptr);

	return nullptr;
}

bool ULoadSystem::UnloadLoadLevel(ULoadSystem* LoadSystemInstance)
{
	if (LoadSystemInstance)
	{
		if (LoadSystemInstance->HandleUnloading())
		{
			bool Result = LoadSystemInstance->ConditionalBeginDestroy();

			return Result;
		}
	}

	return false;
}

bool ULoadSystem::UnloadLoadLevelByAsset(TSoftObjectPtr<UWorld> Level)
{
	// Check if the LoadStystem existing
	if (ULoadSystem::LoadedLevels.Contains(Level))
	{
		ULoadSystem* LoadSystemInstance = *ULoadSystem::LoadedLevels.Find(Level);

		return UnloadLoadLevel(LoadSystemInstance);
	}

	return false;
}

void ULoadSystem::ShowLevel()
{
	if (!this->VisibiltyOnLoad)
	{
		this->VisibiltyOnLoad = true;
		this->LevelStreaming->SetShouldBeVisible(VisibiltyOnLoad);
	}
}

void ULoadSystem::HandleLoading()
{
	this->OnBeginLoad.Broadcast(nullptr);

	// Create Unique Name for sub-level package
	const FString LongPackageName = this->CurrentLevel.GetLongPackageName();
	const FString ShortPackageName = FPackageName::GetShortName(LongPackageName);
	const FString PackagePath = FPackageName::GetLongPackagePath(LongPackageName);
	FString UniqueLevelPackageName = PackagePath + TEXT("/") + GetWorld()->StreamingLevelsPrefix + ShortPackageName;
	UniqueLevelPackageName += TEXT("_LevelInstance");

	this->LevelStreaming->SetWorldAssetByPackageName(FName(*UniqueLevelPackageName));
	this->LevelStreaming->LevelColor = FColor::MakeRandomColor();
	this->LevelStreaming->SetShouldBeLoaded(true);
	this->LevelStreaming->SetShouldBeVisible(this->VisibiltyOnLoad);
	this->LevelStreaming->bShouldBlockOnLoad = false;
	this->LevelStreaming->bInitiallyLoaded = true;
	this->LevelStreaming->bInitiallyVisible = this->VisibiltyOnLoad;

	// Transform
	this->LevelStreaming->LevelTransform = FTransform(FRotator(0,0,0), FVector(0,0,0));
	// Map to Load
	this->LevelStreaming->PackageNameToLoad = FName(*LongPackageName);

	// Binding
	this->LevelStreaming->OnLevelShown.AddDynamic(this, &ULoadSystem::OnLevelStreamingShown);
	this->LevelStreaming->OnLevelLoaded.AddDynamic(this, &ULoadSystem::OnLevelStreamingLoaded);

	// Add the new level to world.
	GetWorld()->AddStreamingLevel(this->LevelStreaming);

	ULoadSystem::LoadedLevels.Add(this->CurrentLevel, this);
}

void ULoadSystem::BeginDestroy()
{
	TArray<TSoftObjectPtr<UWorld>> Keys;
	ULoadSystem::LoadedLevels.GetKeys(Keys);

	for (int32 Idx = Keys.Num() - 1; Idx >= 0; --Idx)
	{
		if (ULoadSystem* LoadSystem = *ULoadSystem::LoadedLevels.Find(Keys[Idx]))
		{
			if (LoadSystem == this)
				ULoadSystem::LoadedLevels.Remove(Keys[Idx]);
		}
	}

	Super::BeginDestroy();
}

bool ULoadSystem::HandleUnloading()
{
	if (UWorld* World = this->GetWorld())
	{
		if (ULevel* LoadedLevel = this->LevelStreaming->GetLoadedLevel())
		{
			World->RemoveFromWorld(LoadedLevel);
			bool RegisteredLevel = ULoadSystem::LoadedLevels.Contains(this->CurrentLevel);
			bool Result = World->RemoveStreamingLevel(this->LevelStreaming);

			if (Result && RegisteredLevel)
				ULoadSystem::LoadedLevels.Remove(this->CurrentLevel);

			return Result;
		}
	}

	return true;
}

void ULoadSystem::OnLevelStreamingShown()
{
	this->OnShow.Broadcast(this);
	
	ILoadSystemInterface* LoadSystemInterface = nullptr;
	if (this->GetLevelScriptActor())
	{
		LoadSystemInterface = Cast<ILoadSystemInterface>(this->GetLevelScriptActor());
		if (LoadSystemInterface)
			LoadSystemInterface->Execute_OnLevelShown(GetLevelScriptActor());
	}

	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		AActor* Actor = *ActorItr;
		if (!ActorItr->IsValidLowLevel() || !Actor)
			continue;
		
		if (Actor->GetClass()->ImplementsInterface(ULoadSystemInterface::StaticClass()))
		{
			ILoadSystemInterface::Execute_OnLevelLoaded(Actor);
			ILoadSystemInterface::Execute_OnLevelShown(Actor);
		}
	}
}

void ULoadSystem::OnLevelStreamingLoaded()
{	
	this->OnComplete.Broadcast(this);

	ILoadSystemInterface* LoadSystemInterface = nullptr;
	if (this->GetLevelScriptActor())
	{
		LoadSystemInterface = Cast<ILoadSystemInterface>(this->GetLevelScriptActor());
		if (LoadSystemInterface)
			LoadSystemInterface->Execute_OnLevelLoaded(GetLevelScriptActor());
	}
}

ALoadSystemLevelScriptActor* ULoadSystem::GetLevelScriptActor()
{
	return Cast<ALoadSystemLevelScriptActor>(this->LevelStreaming->GetLoadedLevel()->GetLevelScriptActor());
}
