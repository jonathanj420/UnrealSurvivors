// Fill out your copyright notice in the Description page of Project Settings.


#include "DEAccInventoryWidget.h"
#include "DEAccSlotWidget.h"
#include "DEAccessoryComponent.h"
#include "DEAccessoryData.h"
#include "Components/PanelWidget.h"

void UDEAccInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UDEAccInventoryWidget::InitializeAccessoryWidget(APawn* NewPawn)
{
    if (!NewPawn) return;

    // 플레이어에게서 악세서리 컴포넌트를 찾아서 캐싱!
    AccessoryComp = NewPawn->FindComponentByClass<UDEAccessoryComponent>();

    if (AccessoryComp)
    {
        AccessoryComp->OnAccUpdated.AddUObject(this, &UDEAccInventoryWidget::OnAccessoryUpdated);

        // 1. 처음 열렸을 때 UI 한번 싹 그려주기
        Refresh();

        // (선택) 만약 AccessoryComp에 델리게이트(OnAccessoryChanged)가 있다면 여기서 Bind 하시면 아주 좋습니다!
    }
}

void UDEAccInventoryWidget::Refresh()
{
    UE_LOG(LogTemp, Warning, TEXT("Try Refresh"));
    //if (!AccessoryComp || !AccSlotPanel || !AccSlotWidgetClass) return;
    if (!AccessoryComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("NO ACcComp"));
        return;

    }
    if (!AccSlotPanel)
    {
        UE_LOG(LogTemp, Warning, TEXT("NO AccSlotPanel"));
        return;

    }
    if (!AccSlotWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("NO AccSlotWidgetClass"));
        return;

    }
    // 1. 기존에 그려져 있던 슬롯들을 싹 밀어버립니다. (중복 생성 방지)
    AccSlotPanel->ClearChildren();

    // 2. 장착된 악세서리 목록 가져오기 
    const TMap<const UDEAccessoryData*, int32>& EquippedMap = AccessoryComp->GetEquippedAccessories();
    for (const auto& Pair : EquippedMap)
    {
        UDEAccSlotWidget* NewSlot = CreateWidget<UDEAccSlotWidget>(GetWorld(), AccSlotWidgetClass);
        if (NewSlot)
        {
            // Pair.Key = UDEAccessoryData* (데이터)
            // Pair.Value = int32 (현재 레벨)
            NewSlot->SetAccessory(Pair.Key->Name, Pair.Value, Pair.Key->Icon);

            AccSlotPanel->AddChild(NewSlot);
        }
    }
}

void UDEAccInventoryWidget::OnAccessoryUpdated(const UDEAccessoryData* InAccData)
{
    // 악세서리를 새로 먹거나 레벨업 했을 때 호출되는 함수
    // 복잡하게 찾아서 수정할 필요 없이, 그냥 전체 인벤토리를 다시 그리는 게 제일 깔끔합니다.
    Refresh();
}
