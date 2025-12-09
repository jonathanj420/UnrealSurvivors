// Fill out your copyright notice in the Description page of Project Settings.


#include "TESTHud.h"

void ATESTHud::DrawHUD()
{
    Super::DrawHUD();

    if (CrosshairTexture)
    {
        // 캔버스 중앙을 찾습니다.
        FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

        // 텍스처 치수의 절반만큼 오프셋을 적용하여 텍스처 중앙이 캔버스 중앙과 정렬되도록 합니다.
        FVector2D CrossHairDrawPosition(Center.X - (CrosshairTexture->GetSurfaceWidth() * 0.5f), Center.Y - (CrosshairTexture->GetSurfaceHeight() * 0.5f));

        // 중심에 조준선을 그립니다.
        FCanvasTileItem TileItem(CrossHairDrawPosition, CrosshairTexture->GetResource(), FLinearColor::White);
        TileItem.BlendMode = SE_BLEND_Translucent;
        Canvas->DrawItem(TileItem);
    }
}
