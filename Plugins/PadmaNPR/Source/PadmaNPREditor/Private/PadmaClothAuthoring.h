#pragma once
#include "CoreMinimal.h"
class UMaterial;
namespace PadmaClothAuthoring
{
UMaterial *BuildMaterial(UPackage *Package, FName Name);
bool CreateTemplates(FString &Message);
} // namespace PadmaClothAuthoring
