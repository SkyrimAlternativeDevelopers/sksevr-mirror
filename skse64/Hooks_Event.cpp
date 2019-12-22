#include "Hooks_Event.h"

#include "skse64_common/Utilities.h"
#include "skse64/GameInput.h"
#include "skse64/GameMenus.h"
#include "skse64/PapyrusEvents.h"
#include "skse64/PluginManager.h"
#include "skse64_common/BranchTrampoline.h"

RelocAddr <uintptr_t> PlayerControls_ctor_Call(0x005B42E0 + 0xEF7);

RelocPtr<UInt32> g_textureResX(0x03186C34);
RelocPtr<UInt32> g_textureResY(0x03186C38);
RelocPtr<UInt32> g_textureRes2X(0x03186C3C);
RelocPtr<UInt32> g_textureRes2Y(0x03186C40);

PlayerControls * PlayerControls::ctor_Hook(void)
{
	PlayerControls	* result = CALL_MEMBER_FN(this, ctor)();

	MenuManager * mm = MenuManager::GetSingleton();
	if (mm) {
		mm->MenuOpenCloseEventDispatcher()->AddEventSink(&g_menuEventHandler);
	} else {
		_MESSAGE("Failed to register SKSE menuEventHandler!");
	}

	g_modCallbackEventDispatcher.AddEventSink(&g_modCallbackEventHandler);
	g_cameraEventDispatcher.AddEventSink(&g_cameraEventHandler);
	g_crosshairRefEventDispatcher.AddEventSink(&g_crosshairRefEventHandler);
	g_actionEventDispatcher.AddEventSink(&g_actionEventHandler);
	g_ninodeUpdateEventDispatcher.AddEventSink(&g_ninodeUpdateEventHandler);

	InputEventDispatcher *inputEventDispatcher = InputEventDispatcher::GetSingleton();
	if (inputEventDispatcher) {
		inputEventDispatcher->AddEventSink(&g_inputEventHandler);
	} else {
		_MESSAGE("Failed to register SKSE inputEventHandler");
	}

	PluginManager::Dispatch_Message(0, SKSEMessagingInterface::kMessage_InputLoaded, NULL, 0, NULL);

	*g_textureResX = 256;
	*g_textureResY = 256;
	*g_textureRes2X = 256;
	*g_textureRes2Y = 256;

	return result;
}

void Hooks_Event_Commit(void)
{
	g_branchTrampoline.Write5Call(PlayerControls_ctor_Call, GetFnAddr(&PlayerControls::ctor_Hook));
}
