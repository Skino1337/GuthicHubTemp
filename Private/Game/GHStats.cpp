// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GHStats.h"

#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

#include "Game/GHGameMode.h"

UGHStats::UGHStats()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> StatsPanelClassFinder(TEXT("/Game/UI/StatsPanel"));
	if (StatsPanelClassFinder.Class != nullptr)
		StatsPanelClass = StatsPanelClassFinder.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> StatsLineClassFinder(TEXT("/Game/UI/StatsLine"));
	if (StatsLineClassFinder.Class != nullptr)
		StatsLineClass = StatsLineClassFinder.Class;
}

void UGHStats::Initialization()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHStats::Initialization");
	
	auto World = GEngine->GetCurrentPlayWorld(nullptr);
}

void UGHStats::Show(AGHNPC* Npc)
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "UGHStats::Show");

	auto World = GEngine->GetCurrentPlayWorld(nullptr);
	auto GameMode = Cast<AGHGameMode>(UGameplayStatics::GetGameMode(World));
	
	if (Npc->_table.Type != ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("_table.Type != ELuaValueType::Table"));
		return;
	}

	auto Attributes = Npc->_table.GetField("attributes");
	if (Attributes.Type != ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attributes.Type != ELuaValueType::Table"));
		return;
	}
	
	auto Talents = Npc->_table.GetField("talents");
	if (Talents.Type != ELuaValueType::Table)
	{
		UE_LOG(LogTemp, Warning, TEXT("Talents.Type != ELuaValueType::Table"));
		return;
	}

	if (StatsPanelClass)
	{
		auto StatsPanel = CreateWidget<UGHStatsPanel>(World, StatsPanelClass);
		if (StatsPanel && StatsLineClass)
		{
			auto TableValues = ULuaBlueprintFunctionLibrary::LuaTableGetValues(Attributes);
			for (FLuaValue TableValue : TableValues)
			{
				auto LuaValueAttributeName = TableValue.GetField("name");
				auto LuaValueAttributeValue = TableValue.GetField("value");
				auto TranslatedString = GameMode->GetTranslatedString(LuaValueAttributeName.ToString());
				
				auto TextWidget = CreateWidget<UGHStatsLine>(StatsPanel, StatsLineClass);
				if (TextWidget)
				{
					TextWidget->StatsLineTextLeft->SetText(FText::FromString(TranslatedString));
					TextWidget->StatsLineTextRight->SetText(FText::FromString(LuaValueAttributeValue.ToString()));
					StatsPanel->AttributesBox->AddChildToVerticalBox(TextWidget);
				}
			}

			TableValues = ULuaBlueprintFunctionLibrary::LuaTableGetValues(Talents);
			for (FLuaValue TableValue : TableValues)
			{
				auto LuaValueAttributeName = TableValue.GetField("name");
				auto LuaValueAttributeSkill = TableValue.GetField("skill");
				auto TranslatedString = GameMode->GetTranslatedString(LuaValueAttributeName.ToString());
				
				auto TextWidget = CreateWidget<UGHStatsLine>(StatsPanel, StatsLineClass);
				if (TextWidget)
				{
					TextWidget->StatsLineTextLeft->SetText(FText::FromString(TranslatedString));
					if (LuaValueAttributeSkill.Type == ELuaValueType::Integer || LuaValueAttributeSkill.Type == ELuaValueType::Number)
					{
						auto Skill = LuaValueAttributeSkill.ToInteger();
						if (Skill > 0)
						{
							TextWidget->StatsLineTextRight->SetVisibility(ESlateVisibility::Visible);
							TextWidget->StatsLineTextRight->SetText(FText::FromString(LuaValueAttributeSkill.ToString()));
						}
						else
						{
							TextWidget->StatsLineTextRight->SetVisibility(ESlateVisibility::Hidden);
						}
					}
					StatsPanel->SkillsBox->AddChildToVerticalBox(TextWidget);
				}
			}
			
			StatsPanel->AddToViewport();
		}
	}
}