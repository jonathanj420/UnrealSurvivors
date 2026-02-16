// Fill out your copyright notice in the Description page of Project Settings.


#include "DESkill_BloodySpear.h"
#include "DEMonsterBase.h"
#include "DEMonsterSpawnManager.h"

UDESkill_BloodySpear::UDESkill_BloodySpear()
{

}

//void UDESkill_BloodySpear::ActivateSkill(FDESkillData* SkillData)
//{
//    ADEMonsterBase* Target = FindBestTarget();
//    if (!Target) return;
//
//    TWeakObjectPtr<ADEMonsterBase> LockedTarget = Target;
//
//    // 예고 FX
//    //SpawnNiagara(WarningFX, Target->GetActorLocation());
//
//    // 지연 타격
//    /*GetWorld()->GetTimerManager().SetTimer(
//        Timer,
//        [this, LockedTarget]()
//        {
//            if (!LockedTarget.IsValid()) return;
//            LockedTarget->ApplySkillDamage(Damage, ...);
//            SpawnNiagara(ImpactFX, LockedTarget->GetActorLocation());
//        },
//        Delay,
//        false*/
//}

void UDESkill_BloodySpear::InitBehaviors()
{
    Super::InitBehaviors();
}

//ADEMonsterBase* UDESkill_BloodySpear::FindBestTarget()
//{
//    ADEMonsterBase* Best = nullptr;
//    float BestScore = -1.f;
//    ADEGameMode* DEGM = Cast<ADEGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
//    ADEMonsterSpawnManager* MonsterSpawnManager=DEGM->GetMonsterSpawnManager();
//    const TArray<ADEMonsterBase*>& ActiveMonsters = MonsterSpawnManager->GetActiveMonsters();
//    //const TArray<ADEMonsterBase*>& 
//    for (ADEMonsterBase* Monster : ActiveMonsters)
//    {
//        if (!IsValid(Monster)) continue;
//
//        float Score = 0.f;
//
//        if (Monster->IsBoss()) Score += 10000.f;
//        Score += Monster->GetMaxHP() * 0.01f;
//        //Score += Monster->IsElite() ? 500.f : 0.f;
//
//        if (Score > BestScore)
//        {
//            BestScore = Score;
//            Best = Monster;
//        }
//    }
//
//    return Best;
//}