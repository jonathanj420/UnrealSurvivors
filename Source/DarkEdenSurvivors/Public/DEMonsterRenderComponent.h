// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InstanceDataTypes.h"
#include "DEMonsterRenderComponent.generated.h"

class ADEMonsterBase;
class UInstancedStaticMeshComponent;
class UStaticMesh;
struct FDEMonsterData;

/**
 * VAT(AnimToTexture) 몬스터 렌더링. DT 행에 VATData가 있는 일반/엘리트 몬스터는 SkeletalMesh 대신
 * 메시마다 하나인 ISM의 인스턴스로 그린다. VATData가 없거나 보스면 몬스터 자신의 SkeletalMesh를 그대로 쓴다.
 *
 * 인스턴스는 ID(FPrimitiveInstanceId)로 들고 있어서 다른 인스턴스가 지워져도 바뀌지 않는다.
 * 트랜스폼은 SpawnManager Tick에서 이동이 끝난 뒤 UpdateInstances로 한 번에 쓴다.
 */
UCLASS(ClassGroup = (Custom))
class DARKEDENSURVIVORS_API UDEMonsterRenderComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDEMonsterRenderComponent();

    // 스폰(ResetMonster 직후)마다 부른다. VAT로 그릴 몬스터면 인스턴스를 만들고 SkeletalMesh를 끈다. 아니면 SkeletalMesh를 켠다
    void AssignMonster(ADEMonsterBase* Monster, const FDEMonsterData* Data);

    // 풀에 반환할 때 부른다
    void ReleaseMonster(ADEMonsterBase* Monster);

    // VAT로 그리는 몬스터의 현재 트랜스폼을 인스턴스에 쓴다
    void UpdateInstances();

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UInstancedStaticMeshComponent* FindOrCreateISM(UStaticMesh* Mesh);

    struct FMonsterInstance
    {
        UInstancedStaticMeshComponent* ISM = nullptr;
        FPrimitiveInstanceId InstanceId;
    };

    // 몬스터는 풀링되어 레벨이 끝날 때까지 Destroy되지 않으므로 raw 포인터 키를 쓴다 (SpawnManager가 UPROPERTY로 잡고 있음)
    TMap<ADEMonsterBase*, FMonsterInstance> Instances;

    UPROPERTY()
    TMap<TObjectPtr<UStaticMesh>, TObjectPtr<UInstancedStaticMeshComponent>> MeshISMs;

    // 같은 종류 몬스터의 걷기가 똑같이 맞물리지 않게 인스턴스마다 시간을 이 범위에서 무작위로 민다 (초)
    UPROPERTY(EditAnywhere, Category = "VAT")
    float MaxAnimTimeOffset = 10.0f;
};
