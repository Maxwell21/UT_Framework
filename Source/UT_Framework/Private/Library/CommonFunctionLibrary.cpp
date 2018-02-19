/************************************************************************/
/* UMBRA TOOLS                                                          */
/* Maxwell - Axel Clerget   											*/
/************************************************************************/

#include "CommonFunctionLibrary.h"
#include "Runtime/CoreUObject/Public/UObject/NoExportTypes.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "TimerManager.h"
#include "Kismet/KismetMathLibrary.h"

TMap<FName, FTimerMapRangeData> UCommonFunctionLibrary::TimeMapRangeData = TMap<FName, FTimerMapRangeData>();

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
