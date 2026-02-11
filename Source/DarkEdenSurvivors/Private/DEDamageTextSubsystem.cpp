// Fill out your copyright notice in the Description page of Project Settings.


#include "DEDamageTextSubsystem.h"
#include "DEDamageTextWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

void UDEDamageTextSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    //// 클래스 로드만 수행 (객체 생성 X)
    //static ConstructorHelpers::FClassFinder<UDEDamageTextWidget> BPClass(
    //    TEXT("/Game/DarkEden/UI/WBP_DamageTextWidget.WBP_DamageTextWidget_C"));

    //if (BPClass.Succeeded())
    //{
    //    DamageTextWidgetClass = BPClass.Class;
    //}
    Super::Initialize(Collection);

    // [수정] ConstructorHelpers 대신 LoadClass 사용!
    // 1. 경로 설정 (에러 메시지에 있던 경로 그대로 복사함 + 뒤에 _C 확인)
    FString Path = TEXT("/Game/DarkEden/UI/WBP_DamageTextWidget.WBP_DamageTextWidget_C");

    // 2. 런타임 로드 (이건 생성자가 아니라도 아무 데서나 쓸 수 있음)
    DamageTextWidgetClass = LoadClass<UDEDamageTextWidget>(nullptr, *Path);

    // 3. 안전장치 (경로 틀리면 로그 띄우고 넘어가기)
    if (!DamageTextWidgetClass)
    {
        UE_LOG(LogTemp, Error, TEXT("Widegt BOOM NO PATH FUCK: %s"), *Path);
        return;
    }
}

void UDEDamageTextSubsystem::Deinitialize()
{
    WidgetPool.Empty();
    ActiveWidgets.Empty();
    CachedPC.Reset();

    Super::Deinitialize();
}

APlayerController* UDEDamageTextSubsystem::GetPlayerController()
{
    if (CachedPC.IsValid())
    {
        return CachedPC.Get();
    }

    UWorld* World = GetWorld();
    if (!World) return nullptr;

    APlayerController* PC = World->GetFirstPlayerController();
    if (PC)
    {
        CachedPC = PC;
    }

    return PC;
}

UDEDamageTextWidget* UDEDamageTextSubsystem::AcquireWidget()
{
    // 1. 기존 풀에서 비활성 위젯 재사용
    for (int32 i = WidgetPool.Num() - 1; i >= 0; --i)
    {
        if (!WidgetPool[i]->IsActive())
        {
            return WidgetPool[i];
        }
    }

    // 2. 없으면 풀 확장 (Lazy Create)
    APlayerController* PC = GetPlayerController();
    if (!PC || !DamageTextWidgetClass) return nullptr;

    UDEDamageTextWidget* NewWidget =
        CreateWidget<UDEDamageTextWidget>(PC, DamageTextWidgetClass);

    if (NewWidget)
    {
        NewWidget->AddToViewport();
        NewWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
        NewWidget->SetPositionInViewport(FVector2D::ZeroVector, true);

        //NewWidget->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));
        WidgetPool.Add(NewWidget);
    }

    return NewWidget;
}

void UDEDamageTextSubsystem::ShowDamage(const FDamageVisualInfo& Info)
{
    UDEDamageTextWidget* Widget = AcquireWidget();
    if (!Widget) return;

    // 위치 랜덤 오프셋 (겹침 방지)
    FDamageVisualInfo FinalInfo = Info;
    FinalInfo.WorldLocation += FVector(
        FMath::RandRange(-20.f, 20.f),
        FMath::RandRange(-20.f, 20.f),
        FMath::RandRange(50.f, 100.f));

    Widget->Activate(FinalInfo);
    ActiveWidgets.Add(Widget);
}

void UDEDamageTextSubsystem::Tick(float DeltaTime)
{

    if (IsTemplate()) return;

    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PC = GetPlayerController();
    if (!PC) return;

    // 1. DPI 스케일 가져오기 (필수!)
    float DPIScale = UWidgetLayoutLibrary::GetViewportScale(PC);
    if (DPIScale <= 0.0f) DPIScale = 1.0f;

    for (int32 i = ActiveWidgets.Num() - 1; i >= 0; --i)
    {
        UDEDamageTextWidget* Widget = ActiveWidgets[i];

        if (!Widget || !Widget->IsActive())
        {
            ActiveWidgets.RemoveAt(i);
            continue;
        }
        // ... (유효성 검사 및 비활성 제거 로직은 그대로) ...

        FVector2D ScreenPos;

        // 2. 월드 좌표 -> 스크린 좌표 변환 (false = DPI 적용 안 된 Raw Pixel 값)
        bool bProjected = UGameplayStatics::ProjectWorldToScreen(
            PC,
            Widget->GetWorldLocation(),
            ScreenPos,
            false
        );

        if (bProjected)
        {
            // 3. [핵심] 렌더링 좌표계로 변환 (ScreenPos / DPIScale)
            // 이렇게 하면 성능(RenderTranslation)과 정확도(DPI) 둘 다 잡음!
            const FVector2D SlatePos = ScreenPos / DPIScale;

            Widget->SetRenderTranslation(SlatePos);

            if (Widget->GetVisibility() != ESlateVisibility::HitTestInvisible)
            {
                Widget->SetVisibility(ESlateVisibility::HitTestInvisible);
            }
        }
        else
        {
            Widget->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

   
}