// Copyright Epic Games, Inc. All Rights Reserved.


#include "ElementalGameModeBase.h"


AManager* AElementalGameModeBase::GetManagerByClass(TSubclassOf<AManager> ManagerClassType)
{
	for(int i = 0; i < Managers.Num(); i++)
	{
		if(Managers[i]->GetClass() == ManagerClassType)
		{
			return Managers[i];
		}
	}
	return nullptr;
}
