// Fill out your copyright notice in the Description page of Project Settings.


#include "DEMonsterRenderComponent.h"
#include "DarkEdenSurvivors.h"
#include "DEMonsterBase.h"
#include "Data/DEMonsterData.h"
#include "AnimToTextureDataAsset.h"
#include "AnimToTextureInstancePlaybackHelpers.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Actor.h"

namespace
{
    // AnimToTexture 자동 재생 머티리얼이 읽는 per-instance 데이터: TimeOffset, PlayRate, StartFrame, EndFrame
    constexpr int32 NumAutoPlayFloats = sizeof(FAnimToTextureAutoPlayData) / sizeof(float);
    static_assert(NumAutoPlayFloats == 4, "AnimToTexture auto play data layout changed");
}

UDEMonsterRenderComponent::UDEMonsterRenderComponent()
{
    // SpawnManager Tick에서 직접 부른다
    PrimaryComponentTick.bCanEverTick = false;
}

void UDEMonsterRenderComponent::AssignMonster(ADEMonsterBase* Monster, const FDEMonsterData* Data)
{
    if (!Monster) return;

    // 보스는 전체 애니메이션을 쓰므로 항상 SkeletalMesh
    UAnimToTextureDataAsset* VATData = nullptr;
    if (Data && Data->MonsterRank != EDEMonsterRank::Boss)
    {
        VATData = Data->VATData.Get();
    }
    UStaticMesh* VATMesh = VATData ? VATData->GetStaticMesh() : nullptr;

    // 풀에서 재사용될 때 다른 종류였을 수 있으니 이전 인스턴스는 항상 지우고 새로 만든다
    ReleaseMonster(Monster);

    if (!VATMesh)
    {
        Monster->SetSkeletalMeshActive(true);
        return;
    }

    UInstancedStaticMeshComponent* ISM = FindOrCreateISM(VATMesh);
    if (!ISM)
    {
        Monster->SetSkeletalMeshActive(true);
        return;
    }

    Monster->SetSkeletalMeshActive(false);

    FMonsterInstance& Entry = Instances.Add(Monster);
    Entry.ISM = ISM;
    Entry.InstanceId = ISM->AddInstanceById(Monster->GetRenderTransform(), /*bWorldSpace*/ true);

    FAnimToTextureAutoPlayData AutoPlay;
    if (VATData->Animations.IsValidIndex(Data->VATAnimationIndex))
    {
        const FAnimToTextureAnimInfo& Anim = VATData->Animations[Data->VATAnimationIndex];
        AutoPlay.StartFrame = Anim.StartFrame;
        AutoPlay.EndFrame = Anim.EndFrame;
    }
    AutoPlay.TimeOffset = FMath::FRandRange(0.0f, MaxAnimTimeOffset);
    AutoPlay.PlayRate = 1.0f;
    ISM->SetCustomDataById(Entry.InstanceId, MakeArrayView(reinterpret_cast<const float*>(&AutoPlay), NumAutoPlayFloats));
}

void UDEMonsterRenderComponent::ReleaseMonster(ADEMonsterBase* Monster)
{
    FMonsterInstance Entry;
    if (!Instances.RemoveAndCopyValue(Monster, Entry)) return;

    if (Entry.ISM && Entry.ISM->IsValidId(Entry.InstanceId))
    {
        Entry.ISM->RemoveInstanceById(Entry.InstanceId);
    }
}

void UDEMonsterRenderComponent::UpdateInstances()
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UDEMonsterRenderComponent::UpdateInstances);

    // 인스턴스 단위 갱신은 InstanceDataManager가 바뀐 것만 렌더 스레드로 보낸다. MarkRenderStateDirty는 부르지 않는다
    for (const TPair<ADEMonsterBase*, FMonsterInstance>& Pair : Instances)
    {
        Pair.Value.ISM->UpdateInstanceTransformById(Pair.Value.InstanceId, Pair.Key->GetRenderTransform(), /*bWorldSpace*/ true, /*bTeleport*/ true);
    }
}

void UDEMonsterRenderComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Instances.Empty();
    MeshISMs.Empty();
    Super::EndPlay(EndPlayReason);
}

UInstancedStaticMeshComponent* UDEMonsterRenderComponent::FindOrCreateISM(UStaticMesh* Mesh)
{
    if (TObjectPtr<UInstancedStaticMeshComponent>* Found = MeshISMs.Find(Mesh))
    {
        return *Found;
    }

    AActor* Owner = GetOwner();
    if (!Owner) return nullptr;

    UInstancedStaticMeshComponent* ISM = NewObject<UInstancedStaticMeshComponent>(Owner);
    ISM->SetStaticMesh(Mesh);
    ISM->SetMobility(EComponentMobility::Movable);
    ISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ISM->SetGenerateOverlapEvents(false);
    ISM->SetCanEverAffectNavigation(false);
    // 몬스터 SkeletalMesh와 같은 설정
    ISM->SetCastShadow(false);
    ISM->SetNumCustomDataFloats(NumAutoPlayFloats);

    // 인스턴스 트랜스폼을 월드 좌표로 넣으므로 컴포넌트는 원점에 고정한다
    ISM->SetUsingAbsoluteLocation(true);
    ISM->SetUsingAbsoluteRotation(true);
    ISM->SetUsingAbsoluteScale(true);
    ISM->SetupAttachment(Owner->GetRootComponent());
    ISM->RegisterComponent();
    ISM->SetWorldTransform(FTransform::Identity);

    MeshISMs.Add(Mesh, ISM);
    UE_LOG(LogDarkEdenSurvivors, Log, TEXT("Monster ISM created for %s"), *Mesh->GetName());
    return ISM;
}
