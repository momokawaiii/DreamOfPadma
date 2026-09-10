#include "World/PCG/PadmaDecorationPoints.h"
#include "PCGContext.h"
#include "PCGElement.h"
#include "Helpers/PCGHelpers.h"

namespace
{
class FPadmaDecorationPointsElement : public IPCGElement
{
public:
	virtual bool IsCacheable(const UPCGSettings* Settings) const override { return false; }
protected:
	virtual bool ExecuteInternal(FPCGContext* Context) const override
	{
		const auto* Settings = Context->GetInputSettings<UPadmaDecorationPointsSettings>();
		if (!Settings) { return true; }
		UPCGPointData* Data = FPCGContext::NewObject_AnyThread<UPCGPointData>(Context);
		Data->GetMutablePoints() = Settings->Points;
		for (FPCGPoint& Point : Data->GetMutablePoints()) { Point.Seed = PCGHelpers::ComputeSeed(Context->GetSeed(), Point.Seed); }
		FPCGTaggedData& Output = Context->OutputData.TaggedData.Emplace_GetRef();
		Output.Pin = PCGPinConstants::DefaultOutputLabel;
		Output.Data = Data;
		return true;
	}
};
}

FPCGElementPtr UPadmaDecorationPointsSettings::CreateElement() const
{
	return MakeShared<FPadmaDecorationPointsElement>();
}
