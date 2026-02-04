// Fill out your copyright notice in the Description page of Project Settings.


#include "DEAOE_Orbit_DanseMacabre.h"

ADEAOE_Orbit_DanseMacabre::ADEAOE_Orbit_DanseMacabre()
{

    static ConstructorHelpers::FObjectFinder<UStaticMesh> SkullMeshAsset(TEXT("/Game/DarkEden/StaticMesh/Skull/human_skull.human_skull"));

    if (SkullMeshAsset.Succeeded() && MeshComp)
    {
        MeshComp->SetStaticMesh(SkullMeshAsset.Object);

    }
}
