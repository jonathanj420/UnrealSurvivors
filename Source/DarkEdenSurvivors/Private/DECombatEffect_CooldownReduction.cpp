// Fill out your copyright notice in the Description page of Project Settings.


#include "DECombatEffect_CooldownReduction.h"
#include "DESkillManagerComponent.h"

void UDECombatEffect_CooldownReduction::OnExecuteEffect(FCombatEventData& EventData)
{
    //UE_LOG(LogTemp, Warning, TEXT("Try Cooldown Reduction Effect . . . "));
    if (!EventData.Instigator) return;
    //UE_LOG(LogTemp, Warning, TEXT("Cooldown Reduction : Instigator Passed . . ."));
    UDESkillManagerComponent* SkillMgr = EventData.Instigator->FindComponentByClass<UDESkillManagerComponent>();
    if (!SkillMgr) return;
    //UE_LOG(LogTemp, Warning, TEXT("Cooldown Reduction : Skill Manager Passed . . . "));
    switch (TargetSkill)
    {
    case ECDRTargetSkill::AllSkills:
        // 모든 스킬 순회하면서 깎아주기
        for (const auto& Pair : SkillMgr->GetActiveSkills())
        {
            if (bInstantReset)
            {
                SkillMgr->ResetCooldownInstant(Pair.Key);
            }
            else
            {
                SkillMgr->ReduceCooldown(Pair.Key, Amount, ReduceType);
            }
        }
        break;

    case ECDRTargetSkill::SpecificSkill:
        if (SpecificSkillID >= 0)
        {
            //UE_LOG(LogTemp, Warning, TEXT("Specific ID Checked"));
            if (bInstantReset)
            {
                SkillMgr->ResetCooldownInstant(SpecificSkillID);
            }
            else
            {
                SkillMgr->ReduceCooldown(SpecificSkillID, Amount, ReduceType);
            }
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("Target Skill Set to Specific but ID INVALID"));
        }
        
        break;
    }
}
