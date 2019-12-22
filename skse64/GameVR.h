#pragma once

#include "GameEvents.h"
#include "NiTypes.h"
#include "NiTextures.h"
#include "openvr_1_0_12.h"

struct VROverlayChange
{

};

struct VRDeviceConnectionChange
{

};

struct VRResetHMDHeight
{

};

// 110
class BSVRInterface
	: public BSTEventSource<VROverlayChange>
	, public BSTEventSource<VRDeviceConnectionChange>
	, public BSTEventSource<VRResetHMDHeight>
{
public:
	virtual ~BSVRInterface() = 0;

	// This is ETrackedControllerRole - 1
	enum BSControllerHand
	{
		kControllerHand_Left,
		kControllerHand_Right
	};

	virtual void Shutdown() = 0;
	virtual vr::IVRCompositor* GetCompositor() = 0;
	virtual void Unk_03() = 0;
	virtual void Submit(vr::EVREye eye, const vr::Texture_t* texture) = 0;
	virtual void SetStanding() = 0; // Sets tracking space to standing
	virtual void SetSeated() = 0; // Sets tracking space to seated
	virtual void GetProjectionMatrix(void*) = 0;
	virtual void Unk_08() = 0;
	virtual void Unk_09() = 0;
	virtual void Unk_0A() = 0;
	virtual void Unk_0B() = 0;
	virtual void Unk_0C() = 0;
	virtual vr::TrackedDeviceIndex_t GetTrackedDeviceHand(BSControllerHand hand) = 0;
	virtual void TriggerHapticPulse(BSControllerHand hand, float duration) = 0; // Duration is multiplied by 0x1417E6E50 which is default 3999.0
	virtual void Unk_0F() = 0;
	virtual void Unk_10() { };
	virtual void Unk_11() = 0;
	virtual void Unk_12() = 0;
	virtual void Unk_13() = 0;
	virtual void Unk_14() { };
	virtual void Unk_15() = 0;

	enum ControllerTypes
	{
		kControllerType_Vive = 0,
		kControllerType_Oculus,
		kControllerType_WindowsMR
	};

	virtual SInt32 GetControllerType() = 0; // Gets unk398
	virtual void Unk_17() = 0; // Creates the controller meshes?
};
STATIC_ASSERT(sizeof(BSVRInterface) == 0x110);

// 408
class BSOpenVR : public BSVRInterface
{
public:
	UInt64					unk110;				// 110 - Compositor Initialized?
	void*					unk118;				// 118
	UInt64					unk120;				// 120
	float					unk128[(0x190 - 0x128) >> 2];	// 128
	vr::COpenVRContext		vrContext;			// 190
	vr::VROverlayHandle_t	inputOverlay;		// 1F8 - Overlay handle for Input
	UInt64					unk200;				// 200
	vr::IVRSystem*			vrSystem;			// 208 - vrclient_x64 - IVRSystem_019
	void*					unk210;				// 210
	UInt64					unk218;				// 218
	void*					unk220;				// 220
	UInt64					unk228;				// 228
	NiTexturePtr			unk230;				// 230 "SIMPLE_NORMAL_MAP" NiSourceTexture

	// 40
	struct Data238
	{
		UInt64	unk00;
		UInt64	unk08;
		UInt64	unk10;
		UInt64	unk18;
		UInt64	unk20;
		UInt64	unk28;
		UInt64	unk30;
		UInt64	unk38;
	};

	Data238					unk238[4];			// 238
	UInt64					unk338[(0x388 - 0x338) >> 3];
	NiPointer<NiNode>		controller[2];		// 388 - Left, Right
	UInt64					unk398;				// 398
	float					unk3A0[(0x408 - 0x3A0) >> 2];
};
STATIC_ASSERT(offsetof(BSOpenVR, controller) == 0x388);
STATIC_ASSERT(sizeof(BSOpenVR) == 0x408);

extern RelocPtr<BSOpenVR *> g_openVR;
