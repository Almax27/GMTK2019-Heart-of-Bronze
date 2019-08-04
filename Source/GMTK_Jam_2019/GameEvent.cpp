// Fill out your copyright notice in the Description page of Project Settings.


#include "GameEvent.h"

DEFINE_LOG_CATEGORY_STATIC(LogGameEvent, Log, All);

UGameEvent::UGameEvent()
{

}

UWorld* UGameEvent::GetWorld() const
{
    return WorldPtr.Get();
}

bool UGameEvent::CanBegin_Implementation() const
{
    return true;
}

bool UGameEvent::IsActive() const
{
    return bIsActive;
}

void UGameEvent::Begin(UWorld* World)
{
    if(ensure(!bIsActive))
    {
        UE_LOG(LogGameEvent, Log, TEXT("Begin: %s"), *GetName());
        WorldPtr = World;
        bIsActive = true;
        OnBegin();
    }
}

void UGameEvent::Tick(float DeltaTime)
{
    if(ensure(bIsActive))
    {
        OnTick(DeltaTime);
    }
}

void UGameEvent::End()
{
    if(ensure(bIsActive))
    {
        UE_LOG(LogGameEvent, Log, TEXT("End: %s"), *GetName());
        bIsActive = false;
        OnEnd();
    }
}

void UGameEvent::OnBegin_Implementation()
{
}

void UGameEvent::OnTick_Implementation(float DeltaTime)
{
}

void UGameEvent::OnEnd_Implementation()
{
}