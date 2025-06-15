/************************************************************************************
 *																					*
 * Copyright (C) 2025 Levi Spevakow.												*
 * Website:	https://github.com/levispev												*
 * Licensed under the MIT License. See 'LICENSE' file for full license information. *
 *																					*
 ************************************************************************************/


#include "CustomPlaceActorsSettings.h"
#include "CustomPlaceActors.h"

void UCustomPlaceActorsSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	if (OnCategoriesUpdated.IsBound())
	{
		OnCategoriesUpdated.Execute();
	}
	
	UE_LOG(LogCustomPlaceActors, Log, TEXT("Update Placement Categories!"));
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
