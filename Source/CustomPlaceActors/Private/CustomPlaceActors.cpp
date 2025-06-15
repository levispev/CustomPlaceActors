/************************************************************************************
 *																					*
 * Copyright (C) 2025 Levi Spevakow.												*
 * Website:	https://github.com/levispev												*
 * Licensed under the MIT License. See 'LICENSE' file for full license information. *
 *																					*
 ************************************************************************************/

#include "CustomPlaceActors.h"

#include "IPlacementModeModule.h"
#include "Styling/SlateStyleRegistry.h"

DEFINE_LOG_CATEGORY(LogCustomPlaceActors);

#define LOCTEXT_NAMESPACE "FCustomPlaceActorsModule"

void FCustomPlaceActorsModule::StartupModule()
{
#if UE_EDITOR
	UE_LOG(LogCustomPlaceActors, Log, TEXT("Custom Place Actors Module Startup"));
	if (IPlacementModeModule::IsAvailable())
	{
		Settings = GetMutableDefault<UCustomPlaceActorsSettings>();
		RegisterCategories();
		Settings->OnCategoriesUpdatedDelegate().BindRaw(this, &FCustomPlaceActorsModule::UpdateCategories);
	}
#endif
}

void FCustomPlaceActorsModule::ShutdownModule()
{
#if UE_EDITOR
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(StyleSet->GetStyleSetName());
		StyleSet.Reset();
	}
	
	UE_LOG(LogCustomPlaceActors, Log, TEXT("Custom Place Actors Module Shutdown"));
#endif
}

void FCustomPlaceActorsModule::RegisterCustomPlacementCategory(const FCustomPlacementCategoryData& CategoryData)
{
	// Display name
	FText displayName = FText::FromString(CategoryData.CategoryName);
	FSlateIcon icon = CreateSlateIcon(CategoryData);
	// Unique handle
	FName uniqueHandle = GenerateUniquePlacementHandle(CategoryData.CategoryName);
	// Tag
	FString tag = TEXT("CustomActors");
	
	// Constructing category data
	FPlacementCategoryInfo categoryInfo(
		displayName,
		icon,
		uniqueHandle,
		tag,
		CategoryData.SortOrder
		);
	
	// Registering category
	if (IPlacementModeModule::Get().RegisterPlacementCategory(categoryInfo))
	{
		UE_LOG(LogCustomPlaceActors, Log, TEXT("Registered placement category '%s' with handle '%s'"), *displayName.ToString(), *uniqueHandle.ToString());
		// Store unique handle in array
		UniqueHandles.Add(uniqueHandle);
		// Registering actors in category
		RegisterCustomPlacementActors(CategoryData, uniqueHandle);
	}
	else
	{
		UE_LOG(LogCustomPlaceActors, Error, TEXT("Failed to register placement category '%s'!"), *displayName.ToString());
	}
}

void FCustomPlaceActorsModule::RegisterCustomPlacementActors(const FCustomPlacementCategoryData& CategoryData, FName UniqueHandle)
{
#if UE_EDITOR
	for (TSoftClassPtr<AActor> actorClass : CategoryData.PlaceCategoryClasses)
	{
		// Attempt to synchronously load class from soft pointer
		UClass* loadedClass = actorClass.LoadSynchronous();
		if (loadedClass)
		{
			// Register actor as item
			TOptional<FPlacementModeID> id = IPlacementModeModule::Get().RegisterPlaceableItem(UniqueHandle, MakeShareable(new FPlaceableItem(nullptr, FAssetData(loadedClass),
				FName(), FName(), TOptional<FLinearColor>(), TOptional<int32>(), loadedClass->GetDisplayNameText())));

			if (id.IsSet())
			{
				UniqueActorIDs.Add(id.GetValue());
			}
			
			UE_LOG(LogCustomPlaceActors, Log, TEXT("Registered actor placement item '%s' for category '%s'"), *loadedClass->GetName(), *CategoryData.CategoryName);
		}
		else
		{
			UE_LOG(LogCustomPlaceActors, Warning, TEXT("Failed to load actor class for category '%s'!"), *CategoryData.CategoryName);
		}
	}
#endif
}

void FCustomPlaceActorsModule::UnregisterCategories()
{
	if (IPlacementModeModule::IsAvailable())
	{
		UnregisterCustomCategoryActors();
		for (FName handle : UniqueHandles)
		{
			IPlacementModeModule::Get().UnregisterPlacementCategory(handle);
			// Remove unique handle from array
			UE_LOG(LogCustomPlaceActors, Log, TEXT("Unregistered placement category with handle '%s'"), *handle.ToString());
		}
		UniqueHandles.Empty();
	}
}

void FCustomPlaceActorsModule::UnregisterCustomCategoryActors()
{
	// Unregister all actors from all categories
	for (FPlacementModeID id : UniqueActorIDs)
	{
		IPlacementModeModule::Get().UnregisterPlaceableItem(id);
	}
	UniqueActorIDs.Empty();
}

void FCustomPlaceActorsModule::UpdateCategories()
{
	// Unregister all categories
	UnregisterCategories();
	// Register all categories
	RegisterCategories();
}

void FCustomPlaceActorsModule::RegisterCategories()
{
	if (IsValid(Settings))
	{
		// Iterate over custom placement categories
		for (const FCustomPlacementCategoryData& data : Settings->PlaceActorsSettings.CustomPlacementCategories)
		{
			RegisterCustomPlacementCategory(data);
		}
	}
}

FSlateIcon FCustomPlaceActorsModule::CreateSlateIcon(const FCustomPlacementCategoryData& CategoryData)
{
	if (IsValid(CategoryData.CategoryIcon))
	{
		if (!StyleSet.IsValid())
		{
			StyleSet = MakeShared<FSlateStyleSet>("CustomPlaceActorsStyle");
			FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);
		}

		const FVector2D iconSize(20.0f, 20.0f);
		FName brushName = FName(*FString::Printf(TEXT("CustomPlaceActorsIcon.%s"), *CategoryData.CategoryIcon->GetName())); 

		FSlateImageBrush* customIconBrush = new FSlateImageBrush(CategoryData.CategoryIcon.Get(), iconSize);
		
		StyleSet->Set(brushName, customIconBrush);

		return FSlateIcon(StyleSet->GetStyleSetName(), brushName);
	}

	return FSlateIcon();
}

FName FCustomPlaceActorsModule::GenerateUniquePlacementHandle(const FString& CategoryName)
{
	// Generate a unique handle based on name and GUID
	FGuid guid = FGuid::NewGuid();
	FString uniqueString = FString::Printf(TEXT("%s_%s"), *CategoryName, *guid.ToString());

	return FName(*uniqueString);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FCustomPlaceActorsModule, CustomPlaceActors)