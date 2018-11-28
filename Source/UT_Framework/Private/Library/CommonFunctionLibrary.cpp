/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "CommonFunctionLibrary.h"
#include "Runtime/CoreUObject/Public/UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"

TMap<FName, FTimerMapRangeData> UCommonFunctionLibrary::TimeMapRangeData = TMap<FName, FTimerMapRangeData>();
TMap<FName, FActionKeysCache> UCommonFunctionLibrary::CachedActions = TMap<FName, FActionKeysCache>();
TMap<FName, FActionKeysCache> UCommonFunctionLibrary::CachedAxis = TMap<FName, FActionKeysCache>();

FName UCommonFunctionLibrary::SetMapRangeTimerByEvent(const UObject* WorldContextObject, FDelegateMapRangeTimer Event, float Time, float Value, float InA, float InB, float OutA, float OutB, bool Reverse, bool Looping)
{
	FTimerHandle Handler;
	FName Key;
	if (UWorld* World = WorldContextObject->GetWorld())
	{
		if (Event.IsBound())
		{
			UCommonFunctionLibrary::RefreshAndCleanMapRangeTimer(WorldContextObject);
			Key = UCommonFunctionLibrary::GenerateTimerUniqueKey();

			FTimerDelegate Delegate;
			FTimerMapRangeData Data = FTimerMapRangeData();
			if (Reverse)
				Data.TimerValue = InB;

			UCommonFunctionLibrary::TimeMapRangeData.Add(Key, Data);
			
			Delegate.BindLambda([] (FDelegateMapRangeTimer Event, float Time, float Value, float InA, float InB, float OutA, float OutB, bool Reverse, bool Looping, FName Key)
			{
				if (FTimerMapRangeData* Data = UCommonFunctionLibrary::TimeMapRangeData.Find(Key))
				{
					Data->TimeElapsed += Time;
					Data->TimerValue  = (Reverse) ? FMath::Clamp<float>(Data->TimerValue - Value, InA, InB) : FMath::Clamp<float>(Data->TimerValue + Value, InA, InB);

					if (!Looping)
						Event.Execute(Data->TimeElapsed, InB, OutB);
					else
					{
						float ResultValue = UKismetMathLibrary::MapRangeClamped(Data->TimerValue, InA, InB, OutA, OutB);
						Event.Execute(Data->TimeElapsed, Data->TimerValue, ResultValue);
					}
				}
			}, Event, Time, Value, InA, InB, OutA, OutB, Reverse, Looping, Key);
			World->GetTimerManager().SetTimer(Handler, Delegate, Time, Looping);

			// Update Handler
			UCommonFunctionLibrary::TimeMapRangeData.Find(Key)->TimerHandler = Handler;
		}
	}

	return Key;
}

void UCommonFunctionLibrary::PauseMapRangeTimer(const UObject* WorldContextObject, FName TimerKey)
{
	if (UWorld* World = WorldContextObject->GetWorld())
	{
		if (FTimerMapRangeData* Data = UCommonFunctionLibrary::TimeMapRangeData.Find(TimerKey))
			World->GetTimerManager().PauseTimer(Data->TimerHandler);
	}
}

void UCommonFunctionLibrary::UnPauseMapRangeTimer(const UObject* WorldContextObject, FName TimerKey)
{
	if (UWorld* World = WorldContextObject->GetWorld())
	{
		if (FTimerMapRangeData* Data = UCommonFunctionLibrary::TimeMapRangeData.Find(TimerKey))
			World->GetTimerManager().UnPauseTimer(Data->TimerHandler);
	}
}

void UCommonFunctionLibrary::CleanMapRangeTimer(const UObject* WorldContextObject, FName TimerKey)
{
	if (UWorld* World = WorldContextObject->GetWorld())
	{
		if (FTimerMapRangeData* Data = UCommonFunctionLibrary::TimeMapRangeData.Find(TimerKey))
		{
			World->GetTimerManager().ClearTimer(Data->TimerHandler);
			UCommonFunctionLibrary::TimeMapRangeData.Remove(TimerKey);
		}
	}
}

TArray<FKey> UCommonFunctionLibrary::GetPlayerInputActionKeys(const UObject* WorldContextObject, bool &IsValid, FName ActionName, int32 PlayerIndex /*= 0*/)
{
	TArray<FKey> Keys = TArray<FKey>();

	if (WorldContextObject && ActionName.IsValid())
	{
		if (UCommonFunctionLibrary::CachedActions.Find(ActionName))
			Keys = UCommonFunctionLibrary::CachedActions[ActionName].Cachedkeys;
		else
		{
			if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex))
			{
				TArray<FInputActionKeyMapping> ActionMappings = PlayerController->PlayerInput->GetKeysForAction(ActionName);
				if (ActionMappings.Num() > 0)
				{
					for (auto const& ActionKey : ActionMappings)
					{
						Keys.Add(ActionKey.Key);
					}

					// Create cache
					FActionKeysCache Cache = FActionKeysCache();
					Cache.Cachedkeys = Keys;
					UCommonFunctionLibrary::CachedActions.Add(ActionName, Cache);
				}
			}
		}
	}

	IsValid = Keys.Num() > 0;
	return Keys;
}

float UCommonFunctionLibrary::GetPlayerInputActionTimeDown(const UObject* WorldContextObject, FName ActionName, int32 PlayerIndex /*= 0*/)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex);
	if (!WorldContextObject || !ActionName.IsValid() || !PlayerController)
		return 0.f;

	bool ValidAction = false;
	TArray<FKey> Keys = GetPlayerInputActionKeys(WorldContextObject, ValidAction, ActionName, PlayerIndex);
	if (ValidAction)
	{
		for (auto const& Key : Keys)
		{
			float Time = PlayerController->GetInputKeyTimeDown(Key);
			if (Time > 0.f)
				return Time;
		}
	}

	return 0.f;
}

TArray<FKey> UCommonFunctionLibrary::GetPlayerInputAxisKeys(const UObject* WorldContextObject, bool &IsValid, FName AxisName, int32 PlayerIndex /*= 0*/)
{
	TArray<FKey> Keys = TArray<FKey>();
	if (WorldContextObject && AxisName.IsValid())
	{
		if (UCommonFunctionLibrary::CachedAxis.Find(AxisName))
			Keys = UCommonFunctionLibrary::CachedAxis[AxisName].Cachedkeys;
		else 
		{
			if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex))
			{
				TArray<FInputAxisKeyMapping> AxisMappings = PlayerController->PlayerInput->GetKeysForAxis(AxisName);
				if (AxisMappings.Num() > 0)
				{
					for (auto const& AxisKey : AxisMappings)
					{
						Keys.Add(AxisKey.Key);
					}

					// Create cache
					FActionKeysCache Cache = FActionKeysCache();
					Cache.Cachedkeys = Keys;
					UCommonFunctionLibrary::CachedAxis.Add(AxisName, Cache);
				}
			}
		}
	}

	IsValid = Keys.Num() > 0;
	return Keys;
}

bool UCommonFunctionLibrary::WasPlayerInputActionJustPressed(const UObject* WorldContextObject, FName ActionName, int32 PlayerIndex /*= 0*/)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex);
	if (!WorldContextObject || !ActionName.IsValid() || !PlayerController)
		return false;

	bool ValidAction = false;
	TArray<FKey> Keys =	GetPlayerInputActionKeys(WorldContextObject, ValidAction, ActionName, PlayerIndex);
	if (ValidAction)
	{
		for (auto const& Key : Keys)
		{
			if (PlayerController->WasInputKeyJustPressed(Key))
				return true;
		}
	}

	return false;
}

bool UCommonFunctionLibrary::WasPlayerInputActionJustReleased(const UObject* WorldContextObject, FName ActionName, int32 PlayerIndex /*= 0*/)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(WorldContextObject, PlayerIndex);
	if (!WorldContextObject || !ActionName.IsValid() || !PlayerController)
		return false;

	bool ValidAction = false;
	TArray<FKey> Keys = GetPlayerInputActionKeys(WorldContextObject, ValidAction, ActionName, PlayerIndex);
	if (ValidAction)
	{
		for (auto const& Key : Keys)
		{
			if (PlayerController->WasInputKeyJustReleased(Key))
				return true;
		}
	}

	return false;
}

void UCommonFunctionLibrary::RefreshAndCleanMapRangeTimer(const UObject* WorldContextObject)
{
	if (UWorld* World = WorldContextObject->GetWorld())
	{
		TArray<FName> Keys;
		UCommonFunctionLibrary::TimeMapRangeData.GetKeys(Keys);

		for (auto const& Key : Keys)
		{
			if (FTimerMapRangeData* Data = UCommonFunctionLibrary::TimeMapRangeData.Find(Key))
			{
				FTimerManager& TimerManager = World->GetTimerManager();
				if (Data->TimerHandler.IsValid() == false || (!TimerManager.IsTimerActive(Data->TimerHandler) && !TimerManager.IsTimerPaused(Data->TimerHandler)))
					UCommonFunctionLibrary::TimeMapRangeData.Remove(Key);
			}
		}
	}
}

FName UCommonFunctionLibrary::GenerateTimerUniqueKey()
{
	FString Val = FString::FromInt(TimeMapRangeData.Num());
	FString Concat = TEXT("TIMER_MAP_RANGE_") + Val;
	
	return *Concat;
}
