// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GHInventory.h"
#include "Game/GHGameMode.h"

#include "Components/TextBlock.h"

UGHInventory::UGHInventory()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> InventorySlotsClassFinder(TEXT("/Game/UI/InventorySlots"));
	if (InventorySlotsClassFinder.Class != nullptr)
		InventorySlotsClass = InventorySlotsClassFinder.Class;
	
	static ConstructorHelpers::FClassFinder<UUserWidget> InventoryItemClassFinder(TEXT("/Game/UI/InventoryItem"));
	if (InventoryItemClassFinder.Class != nullptr)
		InventoryItemClass = InventoryItemClassFinder.Class;

	// static ConstructorHelpers::FClassFinder<UUserWidget> DialogChoicesButtonClassFinder(TEXT("/Game/UI/DialogChoicesButton"));
	// if (DialogChoicesButtonClassFinder.Class != nullptr)
	// 	DialogChoicesButtonClass = DialogChoicesButtonClassFinder.Class;
}

void UGHInventory::Initialization()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHInventory::Initialization");
	
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	
	if (InventorySlotsClass)
		InventorySlots = CreateWidget<UGHInventorySlots>(World, InventorySlotsClass);
	
	if (InventorySlots && InventorySlots->SlotsUniformGridPanel && InventoryItemClass)
	{
		for (int32 y = 0; y < Rows; ++y)
		{
			for (int32 x = 0; x < Columns; ++x)
			{
				auto InventoryItem = CreateWidget<UGHInventoryItem>(InventorySlots, InventoryItemClass);
				if (InventoryItem)
				{
					InventorySlots->SlotsUniformGridPanel->AddChildToUniformGrid(InventoryItem, y, x);
					InventoryItemArray.Add(InventoryItem);
				}
			}
		}
	}
}

void UGHInventory::ShowInventory(AGHNPC* Npc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHInventory::ShowInventory");

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));

	if (Npc->_table.Type != ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("_table.Type != ELuaValueType::Table"));
		return;
	}
	
	auto Items = Npc->_table.GetField("items");
	if (Items.Type != ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("Items.Type != ELuaValueType::Table"));
		return;
	}

	auto TotalItems = 0;
	auto TableValues = ULuaBlueprintFunctionLibrary::LuaTableGetValues(Items);
	for (FLuaValue TableValue : TableValues)
	{
		TotalItems++;
		if (TotalItems <= InventoryItemArray.Num())
		{
			auto LuaValueItemName = TableValue.GetField("name");
			auto TranslatedString = GameMode->GetTranslatedString(LuaValueItemName.ToString());
			InventoryItemArray[TotalItems - 1]->Name->SetText(FText::FromString(TranslatedString));

			auto LuaValueItemAmount = TableValue.GetField("amount");
			if (LuaValueItemAmount.Type == ELuaValueType::Integer || LuaValueItemAmount.Type == ELuaValueType::Number)
			{
				auto Amount = LuaValueItemAmount.ToInteger();
				if (Amount > 1)
				{
					InventoryItemArray[TotalItems - 1]->Count->SetVisibility(ESlateVisibility::Visible);
					InventoryItemArray[TotalItems - 1]->Count->SetText(FText::FromString(FString::FromInt(Amount)));
				}
				else
				{
					InventoryItemArray[TotalItems - 1]->Count->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		}
	}

	if (InventorySlots)
	{
		for (int i = TotalItems; i < InventoryItemArray.Num(); i++)
		{
			InventoryItemArray[i]->Name->SetText(FText::FromString("empty"));
			InventoryItemArray[i]->Count->SetVisibility(ESlateVisibility::Hidden);
		}
		InventorySlots->AddToViewport();
	}
}