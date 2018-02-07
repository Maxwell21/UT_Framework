/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "PoolContainer.h"
#include "PoolComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

void UPoolContainer::Init()
{
	for (auto const& CurrentData : this->Data)
	{
		if (CurrentData.Actor && CurrentData.Key.IsValid() && CurrentData.Number > 0)
		{
			UPoolRuntimeData* RuntimeData = this->CreatePoolRuntimeData(CurrentData.Actor);
			int32 Number = CurrentData.Number - 1;
			this->AddNext(CurrentData.Actor, RuntimeData, Number);

			this->RuntimeData.Add(CurrentData.Key, RuntimeData);
		}
	}
}

UPoolRuntimeData* UPoolContainer::CreatePoolRuntimeData(TSubclassOf<AActor> ActorClass)
{
	UPoolRuntimeData* RuntimeData = NewObject<UPoolRuntimeData>();
	RuntimeData->Active = false;
	RuntimeData->Actor = GetWorld()->SpawnActor<AActor>(ActorClass);

	if (UPoolComponent* PoolComponent = Cast<UPoolComponent>(RuntimeData->Actor->GetComponentByClass(UPoolComponent::StaticClass())))
	{
		PoolComponent->PoolRuntimeData = RuntimeData;
		PoolComponent->PoolDeactivate();
	}
	else
	{
		PoolComponent = NewObject<UPoolComponent>(this, UPoolComponent::StaticClass());
		PoolComponent->PoolRuntimeData = RuntimeData;
		PoolComponent->OnComponentCreated();
		PoolComponent->RegisterComponent();
		PoolComponent->PoolDeactivate();

		RuntimeData->Actor->AddInstanceComponent(PoolComponent);
		RuntimeData->Actor->RerunConstructionScripts();
	}

	return RuntimeData;
}

void UPoolContainer::AddNext(TSubclassOf<AActor> ActorClass, UPoolRuntimeData* Parent, int32& Number)
{
	if (Parent && Number > 0)
	{
		UPoolRuntimeData* RuntimeData = this->CreatePoolRuntimeData(ActorClass);
		Parent->Next = RuntimeData;
		Number--;

		this->AddNext(ActorClass, RuntimeData, Number);
	}
}

AActor* UPoolContainer::GetByKey(FName Key)
{
	UPoolRuntimeData* RuntimeData = *this->RuntimeData.Find(Key);
	if (RuntimeData)
	{
		AActor* Actor =	this->GetValidRuntimePoolData(RuntimeData);
		if (Actor)
		{
			Cast<UPoolComponent>(Actor->GetComponentByClass(UPoolComponent::StaticClass()))->PoolActivate();
			return Actor;
		}
	}

	return nullptr;
}

AActor* UPoolContainer::GetValidRuntimePoolData(UPoolRuntimeData* CurrentRuntimeData)
{
	if (CurrentRuntimeData && CurrentRuntimeData->Active == false)
	{
		CurrentRuntimeData->Active = true;
		return CurrentRuntimeData->Actor;
	}
	else if (CurrentRuntimeData && CurrentRuntimeData->Active && CurrentRuntimeData->Next)
		return this->GetValidRuntimePoolData(CurrentRuntimeData->Next);
	else
		return nullptr;
}
