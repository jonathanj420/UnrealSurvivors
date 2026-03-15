// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DEChestWidget.generated.h"

class UDELevelUpChoiceBase;
class UDEChestRewardSlotWidget;
class UHorizontalBox;
class UButton;
class ADEPlayerController;

UENUM(BlueprintType)
enum class EChestWidgetState : uint8
{
    Appearing,   // 바닥에서 튀어오르는 중
    WaitClick,   // 가만히 유저 클릭 대기 중
    Shaking,     // 흔들리는 중
    RewardsShown // 보상이 화면에 뜬 상태
};


/**
 * 
 */
UCLASS()
class DARKEDENSURVIVORS_API UDEChestWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // 컨트롤러나 매니저가 이 위젯을 화면에 띄우면서 보상 배열을 꽂아줄 핵심 함수!
    UFUNCTION(BlueprintCallable, Category = "Chest UI")
    void SetupChestRewards(const TArray<UDELevelUpChoiceBase*>& InRewards);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeOnInitialized() override;

    // 아무 곳이나 클릭했을 때 이벤트를 받기 위한 C++ 함수 오버라이드
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;


protected:
    // 2. 블루프린트 애니메이션 재생용 (바닥에서 튀어오르기)
    UFUNCTION(BlueprintImplementableEvent)
    void PlayChestAppearAnimation();

    // 3. 블루프린트 애니메이션 재생용 (덜컹덜컹 흔들기)
    UFUNCTION(BlueprintImplementableEvent)
    void PlayChestShakeAnimation();

    // 실제 보상 데이터 배열 (블루프린트에서 카드 UI를 생성하고 그릴 때 사용)
    UPROPERTY(BlueprintReadOnly, Category = "Chest UI")
    TArray<UDELevelUpChoiceBase*> ChestRewards;

    // UI에서 유저가 '확인(혹은 닫기)' 버튼을 눌렀을 때 C++에서 실제 보상을 적용할 함수
    UFUNCTION(BlueprintCallable, Category = "Chest UI")
    void ApplyAllRewardsAndClose();

    // 4. 애니메이션이 끝난 후, BP에서 호출해 줄 "진짜 보상 출력" 함수
    UFUNCTION(BlueprintCallable, Category = "Chest")
    void RevealRewards();

    EChestWidgetState CurrentState = EChestWidgetState::Appearing;

    // 1. 에디터에서 방금 만든 '슬롯 위젯 블루프린트(WBP_ChestRewardSlot)'를 지정할 변수
    UPROPERTY(EditDefaultsOnly, Category = "Chest UI")
    TSubclassOf<UDEChestRewardSlotWidget> RewardSlotClass;

    // 2. 슬롯들을 가로로 나란히 담을 바구니 (UMG에서 똑같은 이름으로 Horizontal Box 생성 필수!)
    UPROPERTY(meta = (BindWidget))
    UHorizontalBox* Box_Rewards;

    UPROPERTY(meta=(BindWidget))
    UButton* Btn_Close;

    UPROPERTY()
    ADEPlayerController* CachedPC;
};
