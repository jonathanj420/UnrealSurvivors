// Fill out your copyright notice in the Description page of Project Settings.


#include "DEStatusEffect_Stun.h"
#include "DEMonsterBase.h"

UDEStatusEffect_Stun::UDEStatusEffect_Stun()
{
}

void UDEStatusEffect_Stun::OnApply(AActor* Target, FActiveStatusEffect& EffectData) const
{
    // 부모 로직이 있다면 먼저 실행 (현재는 비어있겠지만 관례상 호출)
    Super::OnApply(Target, EffectData);

    if (ADEMonsterBase* Monster = Cast<ADEMonsterBase>(Target))
    {
        // 1. 매니저가 관리하는 이동/추적 로직 정지
        Monster->SetCanMove(false);

        //// 2. 베이스에 만들어둔 변수를 활용해 '돌덩이 컬러' 씌우기
        //if (bApplyMaterialTint)
        //{
        //    if (USkeletalMeshComponent* MeshComp = Monster->GetMesh())
        //    {
        //        // Vector(LinearColor) 값을 머티리얼 전체에 쫙 뿌려줍니다.
        //        MeshComp->SetVectorParameterValueOnMaterials(TintParameterName, TintColor.ToFVector());
        //    }
        //}
    }
}

void UDEStatusEffect_Stun::OnRemove(AActor* Target, FActiveStatusEffect& EffectData) const
{
    Super::OnRemove(Target, EffectData);

    if (ADEMonsterBase* Monster = Cast<ADEMonsterBase>(Target))
    {
        // 1. 스턴 해제, 이동 재개
        Monster->SetCanMove(true);

        //// 2. 원래 색상(보통 흰색/1.0)으로 원상 복구
        //if (bApplyMaterialTint)
        //{
        //    if (USkeletalMeshComponent* MeshComp = Monster->GetMesh())
        //    {
        //        MeshComp->SetVectorParameterValueOnMaterials(TintParameterName, OriginalColor.ToFVector());
        //    }
        //}
    }
}
