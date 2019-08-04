// Fill out your copyright notice in the Description page of Project Settings.


#include "GMTK_Jam_2019GameModeBase.h"
#include "Engine/World.h"
#include "GameEvent.h"

DEFINE_LOG_CATEGORY_STATIC(LogGMTKGameMode, Log, All);

static FAutoConsoleCommandWithWorldAndArgs CMD_SubmitEvent(TEXT("SubmitGameEvent"), TEXT(""),
                                                          FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray< FString >& Args, UWorld* World)
{
    if(auto GM = World ? Cast<AGMTK_Jam_2019GameModeBase>(World->GetAuthGameMode()) : nullptr)
    {
        FName Name = NAME_None;
        if(Args.Num() > 0)
        {
            Name = *Args[0];
        }
        GM->SubmitGameEventByName(Name);
    }
}), ECVF_Cheat);

AGMTK_Jam_2019GameModeBase::AGMTK_Jam_2019GameModeBase()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
}

void AGMTK_Jam_2019GameModeBase::BeginPlay()
{
    Super::BeginPlay();
}

void AGMTK_Jam_2019GameModeBase::TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& TickFunction)
{
    Super::TickActor(DeltaTime, TickType, TickFunction);

    TickGameEventQueue(DeltaTime);
    TickActiveGameEvents(DeltaTime);
}

void AGMTK_Jam_2019GameModeBase::TickGameEventQueue(float DeltaTime)
{
    if(GameEventQueue.Num() > 0)
    {
        //Remove invalid entries
        GameEventQueue.Remove(nullptr);

        //Sort in decending priority
        GameEventQueue.StableSort([](UGameEvent& A, UGameEvent& B)
        {
            return A.QueuePriority > B.QueuePriority;
        });

        if(!QueuedActiveGameEvent
           || GameEventQueue[0]->bCanInterupt)
        {
            if(QueuedActiveGameEvent)
            {
                QueuedActiveGameEvent->End();
            }

            QueuedActiveGameEvent = GameEventQueue[0];
            GameEventQueue.RemoveAt(0, 1, false);

            if(QueuedActiveGameEvent->IsValidLowLevel() && !QueuedActiveGameEvent->IsActive())
            {
                QueuedActiveGameEvent->Begin(GetWorld());
                ActiveGameEvents.AddUnique(QueuedActiveGameEvent);
                OnGameEventBegan(QueuedActiveGameEvent);
            }

#if !UE_BUILD_SHIPPING
            FString DebugString = GameEventQueue.Num() > 0 ? TEXT("") : TEXT(" NONE");
            for(auto& QueuedEvent : GameEventQueue)
            {
                DebugString += TEXT("\n - ") + QueuedEvent->GetName();
            }
            UE_LOG(LogGMTKGameMode, Log, TEXT("Queued Game Events:%s"), *DebugString);
#endif

        }
    }
}

void AGMTK_Jam_2019GameModeBase::TickActiveGameEvents(float DeltaTime)
{
    TArray<UGameEvent*> InactiveGameEvents;
    for(auto& GameEvent : ActiveGameEvents)
    {
        if(GameEvent->IsActive())
        {
            GameEvent->Tick(DeltaTime);
        }
        else
        {
            InactiveGameEvents.Add(GameEvent);
        }
    }
    if(InactiveGameEvents.Contains(QueuedActiveGameEvent))
    {
        QueuedActiveGameEvent = nullptr;
    }
    for(auto& GameEvent : InactiveGameEvents)
    {
        ActiveGameEvents.Remove(GameEvent);
    }
}

void AGMTK_Jam_2019GameModeBase::SubmitGameEvent_Implementation(UGameEvent* Event)
{
    if(Event->IsValidLowLevel())
    {
        if(Event->bIsInstant)
        {
            if(!Event->IsActive())
            {
                Event->Begin(GetWorld());
                ActiveGameEvents.AddUnique(Event);
                OnGameEventBegan(Event);
            }
            else
            {
                UE_LOG(LogGMTKGameMode, Error, TEXT("SubmitGameEvent %s: Failed. Already Active"), *Event->GetName());
            }
        }
        else
        {
            UE_LOG(LogGMTKGameMode, Log, TEXT("SubmitGameEvent %s: Added to queue"), *Event->GetName());
            GameEventQueue.Add(Event);
        }
    }
}

void AGMTK_Jam_2019GameModeBase::SubmitGameEventByName_Implementation(FName Name)
{
    if(UGameEvent** EventPtr = NamedGameEvents.Find(Name))
    {
        if(UGameEvent* Event = *EventPtr)
        {
            SubmitGameEvent(Event);
        }
    }
}

void AGMTK_Jam_2019GameModeBase::OnGameEventBegan_Implementation(UGameEvent* Event)
{

}