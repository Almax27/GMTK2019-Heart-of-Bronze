// Fill out your copyright notice in the Description page of Project Settings.


#include "NavigationComponent.h"

#pragma optimize("", off)
// Sets default values for this component's properties
UNavigationComponent::UNavigationComponent()
{
	m_defaultMissileData.Speed = 2.f;
	m_defaultMissileData.Damage = 1.f;
	m_currentLane = 1;
	m_isThreatened = false;
	m_missileSalvoSize = 1;
}


// Called when the game starts
void UNavigationComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}

void UNavigationComponent::ActivateNavigation(float missileSpeed, float missileSalvoSize)
{
	m_defaultMissileData.Speed = missileSpeed;
	m_missileSalvoSize = (int)missileSalvoSize;
	SpawnMissileSalvo();
}



void UNavigationComponent::ChangeLane(bool moveRight)
{
	if (moveRight && CanMoveRight())
	{
		m_currentLane++;
	}
	
	if (!moveRight && CanMoveLeft())
	{
		m_currentLane--;
	}
}


bool UNavigationComponent::CanMoveLeft()
{
	if (m_currentLane > 0) return true;
	return false;
}

bool UNavigationComponent::CanMoveRight()
{
	if (m_currentLane < m_numberOfLanes - 1) return true;
	return false;
}

bool UNavigationComponent::IsValidLane(int number)
{
	if (number >= 0 && number < m_numberOfLanes) return true;
	return false;
}

bool UNavigationComponent::SpawnMissileSalvo()
{
	return SpawnMissileSalvo(m_missileSalvoSize);
}

bool UNavigationComponent::SpawnMissileSalvo(int salvoSize)
{
	if (salvoSize <= 0) return false;

	// track how many missiles we actually spawn
	int spawnedMissiles = 0;

	// get TArray of lane indices (lane pool)
	TArray<int> lanePool = GenerateLanePool();

	// calculate the spacing
	float missileSpacing = m_minMissileStartProgress / salvoSize;
	float missileStartProgress = 0.f;

	// for each missile in salvo size
	for (int i = 0; i < salvoSize; ++i)
	{
		// if there are no lanes in lane pool, regenerate lane pool
		if (lanePool.Num() == 0)
		{
			lanePool = GenerateLanePool();
		}		
	
		// spawn missile in first lane in pool (remove lane from pool)
		if (SpawnMissile(lanePool[0], m_defaultMissileData.Damage, m_defaultMissileData.Speed, missileStartProgress))
		{
			spawnedMissiles++;
			lanePool.RemoveAt(0);

			// adjust starting progress for next missile to spawn
			missileStartProgress -= missileSpacing;

			// add or deduct this from starting progress
			if ((rand() % 2) != 0)
			{
				if ((rand() % 2) != 0)
				{
					missileStartProgress += m_missileSpacingVariance;
				}
				else
				{
					missileStartProgress -= m_missileSpacingVariance;
				}
			}			
		}
	}
	
	// return true if all missiles were spawned successfully
	return (spawnedMissiles == salvoSize);
}

// returns a pool of lane indices which always has the current lane at the front
TArray<int> UNavigationComponent::GenerateLanePool()
{
	TArray<int> result;

	if ((rand() % 2) != 0)
	{
		// fill forwards
		for (int i = 0; i < m_numberOfLanes; ++i)
		{
			result.Add(i);
		}
	}
	else
	{
		// fill backwards
		for (int i = m_numberOfLanes -1; i >= 0; --i)
		{
			result.Add(i);
		}
	}
	
	// move current lane to front
	result.Swap(m_currentLane, 0);

	return result;
}

bool UNavigationComponent::IsLaneEmpty(int lane)
{	
	if (IsValidLane(lane))
	{
		for (int j = 0; j < m_incomingMissiles.Num(); ++j)
		{
			// if there is a missile in this lane
			if (m_incomingMissiles[j].MissileData.Lane == lane)
			{
				return false;
			}
		}
		return true;
	}

	// return false because this is not a valid lane
	return false;
}

bool UNavigationComponent::SpawnMissile(int atLane)
{
	return SpawnMissile(atLane, m_defaultMissileData.Speed, m_defaultMissileData.Damage);
}

bool UNavigationComponent::SpawnMissile(int atLane, float damage, float speed, float startingProgress/* = 0.f*/)
{
	return SpawnMissile(FMissileData(atLane, damage, speed), startingProgress);
}

bool UNavigationComponent::SpawnMissile(FMissileData fromData, float startingProgress)
{
	if (IsValidLane(fromData.Lane))
	{
		FMissile missile = FMissile(fromData, startingProgress);
		m_incomingMissiles.Add(missile);
		return true;
	}
	return false;
}

void UNavigationComponent::UpdateNavigationComponent(float dt)
{
	// update is threatened
	bool previousThreatenedState = m_isThreatened;
	m_isThreatened = !IsLaneEmpty(m_currentLane);

	if (m_isThreatened != previousThreatenedState)
	{
		ThreatenedStateChangedEvent.Broadcast(m_isThreatened);
	}

	// update incoming missiles
	UpdateIncomingMissiles(dt);

	// check for missile impact
	CheckForMissileImpact();
}


void UNavigationComponent::UpdateIncomingMissiles(float dt)
{
	for (int i = 0; i < m_incomingMissiles.Num(); ++i)
	{
		float previousProgress = m_incomingMissiles[i].Progress;
		m_incomingMissiles[i].Progress += m_incomingMissiles[i].MissileData.Speed * dt;

		// if missile was at or below 0 progress and is now above that, that's an event, hoo boy
		if (previousProgress <= 0.f && m_incomingMissiles[i].Progress > 0.f)
		{
			MissileOnScreenEvent.Broadcast(m_incomingMissiles[i]);
		}

		// if missile was at or below half progress and is now above that, that's also an event, you betcha
		if (previousProgress <= 0.5f && m_incomingMissiles[i].Progress > 0.5f)
		{
			MissileHalfwayEvent.Broadcast(m_incomingMissiles[i]);
		}
	}
}

void UNavigationComponent::CheckForMissileImpact() 
{
	for (int i = 0; i < m_incomingMissiles.Num(); ++i)
	{
		if (m_incomingMissiles[i].Progress >= 1.f)
		{
			ProcessMissileImpact(i);
			break; 
		}
	}
}


void UNavigationComponent::ProcessMissileImpact(int missileIndex)
{
	// broadcast detonation
	MissileImpactEvent.Broadcast(m_incomingMissiles[missileIndex]);

	// remove from incoming missiles
	m_incomingMissiles.RemoveAt(missileIndex);
}

TArray<FMissile> UNavigationComponent::GetAllMissilesInLane(int laneIndex) const
{
	TArray<FMissile> result;

	for (int i = 0; i < m_incomingMissiles.Num(); ++i)
	{
		if (m_incomingMissiles[i].MissileData.Lane == laneIndex)
		{
			result.Add(m_incomingMissiles[i]);
		}
	}

	return result;
}

TArray<FMissile> UNavigationComponent::GetVisibleMissilesInLane(int laneIndex) const
{
	TArray<FMissile> result;

	for (int i = 0; i < m_incomingMissiles.Num(); ++i)
	{
		if (m_incomingMissiles[i].MissileData.Lane == laneIndex)
		{
			if (m_incomingMissiles[i].Progress >= 0)
			{
				result.Add(m_incomingMissiles[i]);
			}
			
		}
	}

	return result;
}

#pragma optimize("", on)
