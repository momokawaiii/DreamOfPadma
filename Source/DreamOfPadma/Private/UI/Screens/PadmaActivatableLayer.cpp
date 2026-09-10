#include "UI/Screens/PadmaActivatableLayer.h"
#include "Widgets/Layout/SBox.h"

UPadmaActivatableLayer::UPadmaActivatableLayer()
{
 // This native slice has no CommonInput default-back DataTable. Handle Escape/B
 // on the focused activation node instead of registering an empty action.
 bIsBackHandler=false;bAutoRestoreFocus=true;SetIsFocusable(true);
}
void UPadmaActivatableLayer::Configure(FName InKey,bool bOverlay,TFunction<void()> InBack)
{
 Key=InKey;bBlocking=bOverlay;bIsModal=bOverlay;Back=MoveTemp(InBack);
}
void UPadmaActivatableLayer::SetSurface(TSharedRef<SWidget> InSurface)
{
 if(Surface==InSurface)return;
 Surface=InSurface;if(Host)Host->SetContent(InSurface);
 if(IsActivated()){ClearFocusRestorationTarget();RequestRefreshFocus();}
}
void UPadmaActivatableLayer::SetNavigationKeyHandler(TFunction<bool(const FKeyEvent&,bool)> InHandler)
{
 NavigationKeyHandler=MoveTemp(InHandler);
}
TSharedRef<SWidget> UPadmaActivatableLayer::RebuildWidget()
{
 Super::RebuildWidget();SAssignNew(Host,SBox);
 if(Surface)Host->SetContent(Surface.ToSharedRef());return Host.ToSharedRef();
}
void UPadmaActivatableLayer::ReleaseSlateResources(bool Children)
{
 Back=nullptr;NavigationKeyHandler=nullptr;
 if(Host)Host->SetContent(SNullWidget::NullWidget);
 Super::ReleaseSlateResources(Children);Host.Reset();Surface.Reset();
}
TOptional<FUIInputConfig> UPadmaActivatableLayer::GetDesiredInputConfig() const
{
 return FUIInputConfig(bBlocking?ECommonInputMode::Menu:ECommonInputMode::All,EMouseCaptureMode::NoCapture,EMouseLockMode::DoNotLock,false);
}
bool UPadmaActivatableLayer::NativeOnHandleBackAction()
{
 if(Back){Back();return true;}return Super::NativeOnHandleBackAction();
}
UWidget* UPadmaActivatableLayer::NativeGetDesiredFocusTarget() const{return const_cast<UPadmaActivatableLayer*>(this);}
FReply UPadmaActivatableLayer::NativeOnKeyDown(const FGeometry& Geometry,const FKeyEvent& Event)
{
 if(Event.GetKey()==EKeys::Escape||Event.GetKey()==EKeys::Gamepad_FaceButton_Right)
 {if(NativeOnHandleBackAction())return FReply::Handled();}
 if(!bBlocking&&NavigationKeyHandler&&NavigationKeyHandler(Event,true))return FReply::Handled();
 return Super::NativeOnKeyDown(Geometry,Event);
}
FReply UPadmaActivatableLayer::NativeOnKeyUp(const FGeometry& Geometry,const FKeyEvent& Event)
{
 if(!bBlocking&&NavigationKeyHandler&&NavigationKeyHandler(Event,false))return FReply::Handled();
 return Super::NativeOnKeyUp(Geometry,Event);
}
void UPadmaActivatableLayer::NativeOnActivated()
{
 Super::NativeOnActivated();UE_LOG(LogTemp,Display,TEXT("[PadmaLayers] activate %s modal=%d"),*Key.ToString(),bBlocking);
}
void UPadmaActivatableLayer::NativeOnDeactivated()
{
 Super::NativeOnDeactivated();UE_LOG(LogTemp,Display,TEXT("[PadmaLayers] deactivate %s"),*Key.ToString());
}
