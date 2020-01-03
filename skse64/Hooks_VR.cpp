#include "Hooks_VR.h"
#include "skse64_common/SafeWrite.h"
#include "skse64_common/Utilities.h"
#include "GameVR.h"
#include "InternalVR.h"

// ??_7BSOpenVR@@6B@
RelocAddr<uintptr_t> BSOpenVR_Vtable(0x017E6AA0);

static vr::Exports::VR_GetGenericInterface VR_GetGenericInterface_RealFunc;

static uint32_t packetNum = 0;

class IVRSystemHooked : public IVRSystemProxy
{
public:
	IVRSystemHooked(vr_dst::IVRSystem* targetSystem) : IVRSystemProxy(targetSystem) { }

	virtual bool GetControllerState(vr_src::TrackedDeviceIndex_t unControllerDeviceIndex, vr_src::VRControllerState_t *pControllerState, uint32_t unControllerStateSize) override
	{
#if 0
		vr::EVRInputError err;

		uint32_t leftBoneCount = 0, rightBoneCount = 0;
		vr_context.VRInput()->GetBoneCount(vr_actionHandles.m_leftSkeletonHandle, &leftBoneCount);
		vr_context.VRInput()->GetBoneCount(vr_actionHandles.m_rightSkeletonHandle, &rightBoneCount);

		vr::VRBoneTransform_t* leftBones = new vr::VRBoneTransform_t[leftBoneCount];
		vr_context.VRInput()->GetSkeletalBoneData(vr_actionHandles.m_leftSkeletonHandle, vr::EVRSkeletalTransformSpace::VRSkeletalTransformSpace_Model, vr::EVRSkeletalMotionRange::VRSkeletalMotionRange_WithController, leftBones, leftBoneCount);
		delete[] leftBones;

		vr::VRBoneTransform_t* rightBones = new vr::VRBoneTransform_t[rightBoneCount];
		vr_context.VRInput()->GetSkeletalBoneData(vr_actionHandles.m_rightSkeletonHandle, vr::EVRSkeletalTransformSpace::VRSkeletalTransformSpace_Model, vr::EVRSkeletalMotionRange::VRSkeletalMotionRange_WithController, rightBones, rightBoneCount);
		delete[] rightBones;
#endif

		// We need to emulate controller state calls using Action bindings instead
		if (!vr_context.VRInput()->IsUsingLegacyInput())
		{
			
			vr_src::TrackedDeviceIndex_t leftDevice = GetTrackedDeviceIndexForControllerRole(vr_src::ETrackedControllerRole::TrackedControllerRole_LeftHand);
			vr_src::TrackedDeviceIndex_t rightDevice = GetTrackedDeviceIndexForControllerRole(vr_src::ETrackedControllerRole::TrackedControllerRole_RightHand);

			vr::ActionHandles::Role* role = nullptr;
			if (unControllerDeviceIndex == leftDevice)
			{
				role = &vr_actionHandles.m_leftRole;
			}
			else if (unControllerDeviceIndex == rightDevice)
			{
				role = &vr_actionHandles.m_rightRole;
			}

			if (role)
			{
				memset(pControllerState, 0, unControllerStateSize);

				for (auto& button : role->m_pressedMap)
				{
					vr::InputDigitalActionData_t data;
					vr::EVRInputError err = vr_context.VRInput()->GetDigitalActionData(button.second, &data, sizeof(vr::InputDigitalActionData_t), vr::k_ulInvalidInputValueHandle);
					if (err == vr::EVRInputError::VRInputError_None) {
						if (data.bState && data.bActive) {
							pControllerState->ulButtonPressed |= (1ull << button.first);
						}
					}
				}
				for (auto& button : role->m_touchedMap)
				{
					vr::InputDigitalActionData_t data;
					vr::EVRInputError err = vr_context.VRInput()->GetDigitalActionData(button.second, &data, sizeof(vr::InputDigitalActionData_t), vr::k_ulInvalidInputValueHandle);
					if (err == vr::EVRInputError::VRInputError_None) {
						if (data.bState && data.bActive) {
							pControllerState->ulButtonTouched |= (1ull << button.first);
						}
					}
				}

				// Both X and Y
				for (auto& button : role->m_valueMap)
				{
					vr::InputAnalogActionData_t data;
					vr::EVRInputError err = vr_context.VRInput()->GetAnalogActionData(button.second, &data, sizeof(vr::InputAnalogActionData_t), vr::k_ulInvalidInputValueHandle);
					if (err == vr::EVRInputError::VRInputError_None) {
						if (data.bActive) {
							pControllerState->rAxis[button.first - vr::k_EButton_Axis0].x = data.x;
							pControllerState->rAxis[button.first - vr::k_EButton_Axis0].y = data.y;
						}
					}
				}

				// Only X
				for (auto& button : role->m_value1Map)
				{
					vr::InputAnalogActionData_t data;
					vr::EVRInputError err = vr_context.VRInput()->GetAnalogActionData(button.second, &data, sizeof(vr::InputAnalogActionData_t), vr::k_ulInvalidInputValueHandle);
					if (err == vr::EVRInputError::VRInputError_None) {
						if (data.bActive) {
							pControllerState->rAxis[button.first - vr::k_EButton_Axis0].x = data.x;
						}
					}
				}

				// Only Y
				for (auto& button : role->m_value2Map)
				{
					vr::InputAnalogActionData_t data;
					vr::EVRInputError err = vr_context.VRInput()->GetAnalogActionData(button.second, &data, sizeof(vr::InputAnalogActionData_t), vr::k_ulInvalidInputValueHandle);
					if (err == vr::EVRInputError::VRInputError_None) {
						if (data.bActive) {
							pControllerState->rAxis[button.first - vr::k_EButton_Axis0].y = data.y;
						}
					}
				}

				pControllerState->unPacketNum = ++packetNum;
				return true;
			}
		}

		return __super::GetControllerState(unControllerDeviceIndex, pControllerState, unControllerStateSize);
	}

	virtual bool GetControllerStateWithPose(vr_src::ETrackingUniverseOrigin eOrigin, vr_src::TrackedDeviceIndex_t unControllerDeviceIndex, vr_src::VRControllerState_t *pControllerState, uint32_t unControllerStateSize, vr_src::TrackedDevicePose_t *pTrackedDevicePose) override
	{
		return __super::GetControllerStateWithPose(eOrigin, unControllerDeviceIndex, pControllerState, unControllerStateSize, pTrackedDevicePose);
	}

	virtual uint32_t GetStringTrackedDeviceProperty(vr_src::TrackedDeviceIndex_t unDeviceIndex, vr_src::ETrackedDeviceProperty prop, VR_OUT_STRING() char *pchValue, uint32_t unBufferSize, vr_src::ETrackedPropertyError *pError) override
	{
		uint32_t ret = __super::GetStringTrackedDeviceProperty(unDeviceIndex, prop, pchValue, unBufferSize, pError);

		if (prop == vr_src::ETrackedDeviceProperty::Prop_TrackingSystemName_String && !vr_context.VRInput()->IsUsingLegacyInput())
		{
			vr_src::TrackedDeviceIndex_t leftDevice = GetTrackedDeviceIndexForControllerRole(vr_src::ETrackedControllerRole::TrackedControllerRole_LeftHand);
			if (leftDevice != vr::k_unTrackedDeviceIndexInvalid)
			{
				char buff[0x80];
				uint32_t ret = __super::GetStringTrackedDeviceProperty(unDeviceIndex, vr_src::Prop_RenderModelName_String, buff, 0x80, pError);
				if (_stricmp(buff, "{indexcontroller}valve_controller_knu_1_0_left") == 0) {
					strcpy_s(pchValue, unBufferSize, "oculus");
					return 6;
				}
			}
			vr_src::TrackedDeviceIndex_t rightDevice = GetTrackedDeviceIndexForControllerRole(vr_src::ETrackedControllerRole::TrackedControllerRole_RightHand);
			if (rightDevice != vr::k_unTrackedDeviceIndexInvalid)
			{
				char buff[0x80];
				uint32_t ret = __super::GetStringTrackedDeviceProperty(unDeviceIndex, vr_src::Prop_RenderModelName_String, buff, 0x80, pError);
				if (_stricmp(buff, "{indexcontroller}valve_controller_knu_1_0_right") == 0) {
					strcpy_s(pchValue, unBufferSize, "oculus");
					return 6;
				}
			}
		}

		return ret;
	}

	virtual int32_t GetInt32TrackedDeviceProperty(vr_src::TrackedDeviceIndex_t unDeviceIndex, vr_src::ETrackedDeviceProperty prop, vr_src::ETrackedPropertyError *pError) override
	{
		int32_t ret = __super::GetInt32TrackedDeviceProperty(unDeviceIndex, prop, pError);

		// Is this wrong? It reports Axis3 as None when it's actually a Joystick?
		if (prop == vr_src::ETrackedDeviceProperty::Prop_Axis3Type_Int32 && !vr_context.VRInput()->IsUsingLegacyInput())
		{
			vr_src::TrackedDeviceIndex_t leftDevice = GetTrackedDeviceIndexForControllerRole(vr_src::ETrackedControllerRole::TrackedControllerRole_LeftHand);
			if (leftDevice == unDeviceIndex)
			{
				char buff[0x80];
				uint32_t ret = __super::GetStringTrackedDeviceProperty(unDeviceIndex, vr_src::Prop_RenderModelName_String, buff, 0x80, pError);
				if (_stricmp(buff, "{indexcontroller}valve_controller_knu_1_0_left") == 0) {
					return vr_src::k_eControllerAxis_Joystick;
				}
			}
			vr_src::TrackedDeviceIndex_t rightDevice = GetTrackedDeviceIndexForControllerRole(vr_src::ETrackedControllerRole::TrackedControllerRole_RightHand);
			if (rightDevice == unDeviceIndex)
			{
				char buff[0x80];
				uint32_t ret = __super::GetStringTrackedDeviceProperty(unDeviceIndex, vr_src::Prop_RenderModelName_String, buff, 0x80, pError);
				if (_stricmp(buff, "{indexcontroller}valve_controller_knu_1_0_right") == 0) {
					return vr_src::k_eControllerAxis_Joystick;
				}
			}
		}

		return ret;
	}
};

void * VR_CALLTYPE Hook_VR_GetGenericInterface_Execute(const char *pchInterfaceVersion, vr_src::EVRInitError *peError)
{
	if (_stricmp(pchInterfaceVersion, vr_src::IVRSystem_Version) == 0)
	{
		return new IVRSystemHooked(static_cast<vr_dst::IVRSystem*>(VR_GetGenericInterface_RealFunc(vr_dst::IVRSystem_Version, peError)));
	}
	return VR_GetGenericInterface_RealFunc(pchInterfaceVersion, peError);
}

void PollNextEvent_Hooked(BSOpenVR* thisPtr)
{
	if (!vr_context.VRInput()->IsUsingLegacyInput())
	{
		vr::VRActiveActionSet_t actionSet;
		actionSet.ulActionSet = vr_actionHandles.m_legacySetHandle;
		actionSet.ulRestrictedToDevice = vr::k_ulInvalidInputValueHandle;
		actionSet.ulSecondaryActionSet = vr::k_ulInvalidInputValueHandle;
		actionSet.nPriority = 10;
		vr::EVRInputError err = vr_context.VRInput()->UpdateActionState(&actionSet, sizeof(vr::VRActiveActionSet_t), 1);
	}

	thisPtr->PollNextEvent_Internal();
}

void Hooks_VR_Init(void)
{
	vr::IAT::VR_GetGenericInterface = (uintptr_t)GetIATAddr((UInt8 *)GetModuleHandle(NULL), "openvr_api.dll", "VR_GetGenericInterface");
	vr::IAT::VR_InitInternal2 = (uintptr_t)GetIATAddr((UInt8 *)GetModuleHandle(NULL), "openvr_api.dll", "VR_InitInternal2");
	vr::IAT::VR_ShutdownInternal = (uintptr_t)GetIATAddr((UInt8 *)GetModuleHandle(NULL), "openvr_api.dll", "VR_ShutdownInternal");
	vr::IAT::VR_GetInitToken = (uintptr_t)GetIATAddr((UInt8 *)GetModuleHandle(NULL), "openvr_api.dll", "VR_GetInitToken");

	// This occurs in Init on purpose so plugins can wrap this interface, SKSE should be first
	// so that it can emulate GetControllerState and pass the data along to existing plugins wrapping this interface
	UInt32 vrInterfaceHooked = 0;
	if (GetConfigOption_UInt32("Input", "bActionBindings", &vrInterfaceHooked) && vrInterfaceHooked)
	{
		VR_GetGenericInterface_RealFunc = (vr::Exports::VR_GetGenericInterface)*(uintptr_t *)vr::IAT::VR_GetGenericInterface;
		SafeWrite64(vr::IAT::VR_GetGenericInterface, (uintptr_t)Hook_VR_GetGenericInterface_Execute);
	}

	vr_exports.VR_GetGenericInterface = (vr::Exports::VR_GetGenericInterface)*(uintptr_t *)vr::IAT::VR_GetGenericInterface;
	vr_exports.VR_InitInternal2 = (vr::Exports::VR_InitInternal2)*(uintptr_t *)vr::IAT::VR_InitInternal2;
	vr_exports.VR_ShutdownInternal = (vr::Exports::VR_ShutdownInternal)*(uintptr_t *)vr::IAT::VR_ShutdownInternal;
	vr_exports.VR_GetInitToken = (vr::Exports::VR_GetInitToken)*(uintptr_t *)vr::IAT::VR_GetInitToken;
}

void Hooks_VR_Commit(void)
{
	UInt32 vrInterfaceHooked = 0;
	if (GetConfigOption_UInt32("Input", "bActionBindings", &vrInterfaceHooked) && vrInterfaceHooked)
	{
		SafeWrite64(BSOpenVR_Vtable.GetUIntPtr() + 0x58, (uintptr_t)PollNextEvent_Hooked);
	}
}
