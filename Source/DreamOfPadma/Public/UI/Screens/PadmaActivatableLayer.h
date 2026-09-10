#pragma once
#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "PadmaActivatableLayer.generated.h"

class SBox;

/** A real CommonUI activation node; the view supplies drawing and intent only. */
UCLASS()
class DREAMOFPADMA_API UPadmaActivatableLayer : public UCommonActivatableWidget
{
 GENERATED_BODY()
public:
 UPadmaActivatableLayer();
 void Configure(FName InKey, bool bOverlay, TFunction<void()> InBack);
 void SetSurface(TSharedRef<SWidget> InSurface);
 void SetNavigationKeyHandler(TFunction<bool(const FKeyEvent&,bool)> InHandler);
 FName GetLayerKey() const { return Key; }
 virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
protected:
 virtual TSharedRef<SWidget> RebuildWidget() override;
 virtual void ReleaseSlateResources(bool bReleaseChildren) override;
 virtual bool NativeOnHandleBackAction() override;
 virtual void NativeOnActivated() override;
 virtual void NativeOnDeactivated() override;
 virtual UWidget* NativeGetDesiredFocusTarget() const override;
 virtual FReply NativeOnKeyDown(const FGeometry& Geometry,const FKeyEvent& Event) override;
 virtual FReply NativeOnKeyUp(const FGeometry& Geometry,const FKeyEvent& Event) override;
private:
 FName Key;
 bool bBlocking=false;
 TFunction<void()> Back;
 TFunction<bool(const FKeyEvent&,bool)> NavigationKeyHandler;
 TSharedPtr<SBox> Host;
 TSharedPtr<SWidget> Surface;
};
