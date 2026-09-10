#include "Misc/AutomationTest.h"
#include "UI/Screens/PadmaGameScreen.h"
#include "UI/Screens/PadmaActivatableLayer.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "UObject/GarbageCollection.h"

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPadmaUISessionReleaseTest,"DreamOfPadma.UI.SessionReleaseDoesNotRetainGameInstance",
 EAutomationTestFlags::EditorContext|EAutomationTestFlags::ProductFilter)
bool FPadmaUISessionReleaseTest::RunTest(const FString& Parameters)
{
 auto* Instance=NewObject<UGameInstance>(GEngine);
 Instance->InitializeStandalone();
 auto* World=Instance->GetWorld();
 auto* Screen=CreateWidget<UPadmaGameScreen>(Instance,UPadmaGameScreen::StaticClass());
 if(!TestNotNull(TEXT("Screen owned by GameInstance"),Screen))
 {Instance->Shutdown();World->DestroyWorld(false);GEngine->DestroyWorldContext(World);return false;}
 TWeakObjectPtr<UGameInstance> WeakInstance=Instance;
 TWeakObjectPtr<UPadmaGameScreen> WeakScreen=Screen;
 TSharedPtr<SWidget> SlateRoot=Screen->TakeWidget();
 FPadmaGameView View;View.bWorldMap=true;View.bPaintedTutorial=true;View.PageKey=TEXT("map");
 View.ModalTitle=TEXT("PIE cleanup modal");View.ModalText=TEXT("A pooled overlay must release with its page.");
 View.OverlayPath={FName(TEXT("cleanup-modal"))};
 Screen->Present(View);
 TWeakObjectPtr<UPadmaActivatableLayer> WeakPage=Screen->PageLayer;
 TWeakObjectPtr<UPadmaActivatableLayer> WeakOverlay=Screen->OverlayLayers.IsEmpty()?nullptr:Screen->OverlayLayers[0].Get();
 TWeakObjectPtr<UCommonActivatableWidgetStack> WeakStack=Screen->PageStack;
 // A leaf retained by Slate focus/painting may outlive its owning HUD; its MID must not root that session.
 FPadmaUIEntry Action;Action.Title=TEXT("Retained primary action");Action.bPrimary=true;
 TSharedPtr<SWidget> RetainedButton=Screen->Entry(Action);
 Screen->ShutdownPresentation();Screen->ShutdownPresentation();
 TestTrue(TEXT("Shutdown clears both explicit stack references"),!Screen->PageStack&&!Screen->OverlayStack);
 SlateRoot.Reset();
 Instance->Shutdown();World->DestroyWorld(false);GEngine->DestroyWorldContext(World);
 Screen=nullptr;Instance=nullptr;World=nullptr;
 CollectGarbage(RF_NoFlags,true);
 TestFalse(TEXT("One GC releases the HUD"),WeakScreen.IsValid());
 TestFalse(TEXT("One GC releases the pooled page"),WeakPage.IsValid());
 TestFalse(TEXT("One GC releases the pooled overlay"),WeakOverlay.IsValid());
 TestFalse(TEXT("One GC releases the manually created stack"),WeakStack.IsValid());
 TestFalse(TEXT("A retained primary button does not retain GameInstance"),WeakInstance.IsValid());
 RetainedButton.Reset();
 return !HasAnyErrors();
}
#endif
