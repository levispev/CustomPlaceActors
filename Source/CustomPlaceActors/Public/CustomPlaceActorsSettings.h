/************************************************************************************
 *																					*
 * Copyright (C) 2025 Levi Spevakow.												*
 * Website:	https://github.com/levispev												*
 * Licensed under the MIT License. See 'LICENSE' file for full license information. *
 *																					*
 ************************************************************************************/

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CustomPlaceActorsSettings.generated.h"

DECLARE_DELEGATE(FOnCategoriesUpdated);

/** Placement Category Data */
USTRUCT(BlueprintType)
struct FCustomPlacementCategoryData
{
	GENERATED_BODY()

	// Displayed name for the category.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Place Actors Category")
	FText CategoryName;
	// Optional shorthand for the category name. (will be used in the "Place Actors" panel (UE5.5+))
	UPROPERTY(EditAnywhere, BlueprintReadOnly, DisplayName = "Short Category Name (optional)", Category = "Place Actors Category")
	FText ShortCategoryName;
	// Displayed icon for the category.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Place Actors Category")
	TObjectPtr<UTexture2D> CategoryIcon = nullptr;
	// Sorting order for the category when displayed.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Place Actors Category")
	int SortOrder = 0;
    // Soft reference array of classes to display in the category.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Place Actors Category")
	TArray<TSoftClassPtr<AActor>> PlaceCategoryClasses;
};

/** Custom Settings Struct */
USTRUCT(BlueprintType)
struct FCustomPlaceActorSettings
{
	GENERATED_BODY()
    
	UPROPERTY(Config, EditAnywhere, Category = "Place Actors Panel", meta = (NoElementDuplicate))
	TArray<FCustomPlacementCategoryData> CustomPlacementCategories;
    
};

/**
 * Custom Place Actors Settings
 */
UCLASS(Config = "Game", DefaultConfig, meta = (DisplayName = "Custom Place Actors Settings"))
class CUSTOMPLACEACTORS_API UCustomPlaceActorsSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(Config, BlueprintReadOnly, EditAnywhere, Category = "Categories")
	FCustomPlaceActorSettings PlaceActorsSettings;
	
	FOnCategoriesUpdated& OnCategoriesUpdatedDelegate() { return OnCategoriesUpdated; }
	
protected:
	
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
private:

	FOnCategoriesUpdated OnCategoriesUpdated;
	
};