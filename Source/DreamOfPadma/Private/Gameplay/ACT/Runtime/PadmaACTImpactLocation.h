#pragma once

#include "Gameplay/Combat/PadmaCombatUnit.h"
#include "Components/SkeletalMeshComponent.h"

namespace PadmaACT
{
inline FVector ResolveImpactLocation(const FHitResult& Hit, const APadmaCombatUnit& Target,
    FName Socket, bool bVolumeContact)
{
    // Initial overlaps report the swept shape's center, not a target surface contact.
    // Volume/primary-target queries likewise do not establish a surface impact.
    if (!bVolumeContact && !Hit.bStartPenetrating && !Hit.ImpactPoint.ContainsNaN())
        return Hit.ImpactPoint;
    const auto* Mesh = Target.GetMesh();
    if (Mesh && !Socket.IsNone() && Mesh->DoesSocketExist(Socket))
        return Mesh->GetSocketLocation(Socket);
    return Target.GetActorLocation();
}
}
