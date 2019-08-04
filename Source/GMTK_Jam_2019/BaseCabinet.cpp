// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCabinet.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"

namespace CabinetState
{
	const FName Active("Active");
	const FName Inactive("Inactive");
	const FName Fail("Fail");

	const TArray<FName> States = { Active, Inactive, Fail };

}

namespace CabinetResult
{
	const FName Success("Success");
	const FName Fail("Fail");

	const TArray<FName> Results = { Success, Fail };
}

// Sets default values
ABaseCabinet::ABaseCabinet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	m_currentState = CabinetState::Inactive;
	m_timeInCurrentState = 0.f;
}

// Called when the game starts or when spawned
void ABaseCabinet::BeginPlay()
{
	Super::BeginPlay();
	
	if (StartActivated)
	{
		ActivateCabinet();
	}
}

void ABaseCabinet::RefreshScreenWidget()
{
    if(!AutoRefreshWidget)
    {
        return;
    }

	UWidgetComponent* widgetComponent = GetWidgetComponent();
	if (widgetComponent == nullptr) return;

	if (m_currentState.IsEqual(CabinetState::Active))
	{
		// show the active screen
		widgetComponent->SetWidgetClass(ActiveScreen);
	}
	if (m_currentState.IsEqual(CabinetState::Inactive))
	{
		// show the inactive screen
		widgetComponent->SetWidgetClass(InactiveScreen);
	}
	if (m_currentState.IsEqual(CabinetState::Fail))
	{
		// show the fail screen
		widgetComponent->SetWidgetClass(FailScreen);
	}
}

// Called every frame
void ABaseCabinet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateState(DeltaTime);

}

/* The widget is not instantiated in blueprints because... I can't get it to work properly :(
* The blueprint implementation of this returns the component - don't call the parent function as it returns null
*/
UWidgetComponent* ABaseCabinet::GetWidgetComponent_Implementation()
{
	return nullptr;
}

void ABaseCabinet::UpdateState_Implementation(float dt)
{
	FName PendingState = FName("None");
	GetPendingState(PendingState);

	if (PendingState != m_currentState)
	{
		ChangeState(PendingState);
	}

	m_timeInCurrentState += dt;
}

void ABaseCabinet::ChangeState(FName newState)
{
	if (IsValidState(newState) && !newState.IsEqual(m_currentState))
	{
		m_currentState = newState;
		m_timeInCurrentState = 0.f;
		RefreshScreenWidget();
		OnCabinetStateChanged.Broadcast(this);
	}
}

void ABaseCabinet::BroadcastResult(FCabinetResult result)
{
	if (IsValidResult(result.result))
	{
        result.Cabinet = this;
		OnCabinetResult.Broadcast(result);
	}
}

/* base implementation just sets outgoing state to current state 
* derived classes can add additional logic
*/ 
void ABaseCabinet::GetPendingState_Implementation(FName& outPendingState)
{
	outPendingState = m_currentState;
}

bool ABaseCabinet::IsValidState(FName state)
{
	return CabinetState::States.Contains(state);
}

bool ABaseCabinet::IsValidResult(FName result)
{
	return CabinetResult::Results.Contains(result);
}

void ABaseCabinet::ActivateCabinet_Implementation()
{
	ChangeState(CabinetState::Active);
}

void ABaseCabinet::CheckForResult_Implementation()
{
	// how am I supposed to know what's going on? I'm just a cabinet
}