/************************************************************************************
 *																					*
 * Copyright (C) 2025 Levi Spevakow.												*
 * Website:	https://github.com/levispev												*
 * Licensed under the MIT License. See 'LICENSE' file for full license information. *
 *																					*
 ************************************************************************************/

#pragma once

#include "CustomPlaceActorsSettings.h"
#include "Modules/ModuleManager.h"

class FPlacementModeID;

DECLARE_LOG_CATEGORY_EXTERN(LogCustomPlaceActors, Log, All);

class FCustomPlaceActorsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	void RegisterCategories();
	void RegisterCustomPlacementCategory(const FCustomPlacementCategoryData& CategoryData);
	void RegisterCustomPlacementActors(const FCustomPlacementCategoryData& CategoryData, FName UniqueHandle);

	void UnregisterCategories();
	void UnregisterCustomCategoryActors();

	void UpdateCategories();
	
	FSlateIcon CreateSlateIcon(const FCustomPlacementCategoryData& CategoryData);
	static FName GenerateUniquePlacementHandle(const FText& CategoryName);

	UCustomPlaceActorsSettings* Settings = nullptr;

	// Custom styleset for category icons
	TSharedPtr<FSlateStyleSet> StyleSet;
	// Unique handles for each registered category
	TArray<FName> UniqueHandles;
	// Unique ID's for all actor entries
	TArray<FPlacementModeID> UniqueActorIDs;
	
};