#include "InternalVR.h"
#include "skse64_common/Utilities.h"

#include <stdio.h>
#include <WinBase.h>

vr::Exported vr_exports;
vr::COpenVRContext vr_context(vr_exports);
vr::ActionHandles vr_actionHandles;

void RegisterActionBindings()
{
	UInt32	actionBindings = 0;
	if (GetConfigOption_UInt32("Input", "bActionBindings", &actionBindings) && actionBindings)
	{
		char buff[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, buff);
		strcat_s(buff, "\\Data\\bindings\\actions.json");
		vr::IVRInput* vrInput = vr_context.VRInput();
		if (vrInput) {
			vr::EVRInputError error = vrInput->SetActionManifestPath(buff);

			if (error != vr::EVRInputError::VRInputError_None)
				_MESSAGE("Error in registering action manifest to SteamVR Input (%d)", error);

			error = vrInput->GetActionSetHandle("/actions/legacy", &vr_actionHandles.m_legacySetHandle);

			error = vrInput->GetActionHandle("/actions/legacy/in/Left_System_Press", &vr_actionHandles.m_leftRole.m_pressedMap[vr::k_EButton_System]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Left_System_Touch", &vr_actionHandles.m_leftRole.m_touchedMap[vr::k_EButton_System]);

			error = vrInput->GetActionHandle("/actions/legacy/in/Left_ApplicationMenu_Press", &vr_actionHandles.m_leftRole.m_pressedMap[vr::k_EButton_ApplicationMenu]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Left_ApplicationMenu_Touch", &vr_actionHandles.m_leftRole.m_touchedMap[vr::k_EButton_ApplicationMenu]);

			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Grip_Press", &vr_actionHandles.m_leftRole.m_pressedMap[vr::k_EButton_Grip]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Grip_Touch", &vr_actionHandles.m_leftRole.m_touchedMap[vr::k_EButton_Grip]);

			error = vrInput->GetActionHandle("/actions/legacy/in/Left_A_Press", &vr_actionHandles.m_leftRole.m_pressedMap[vr::k_EButton_A]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Left_A_Touch", &vr_actionHandles.m_leftRole.m_touchedMap[vr::k_EButton_A]);

			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Axis0_Press", &vr_actionHandles.m_leftRole.m_pressedMap[vr::k_EButton_Axis0]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Axis0_Touch", &vr_actionHandles.m_leftRole.m_touchedMap[vr::k_EButton_Axis0]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Axis0_Value", &vr_actionHandles.m_leftRole.m_valueMap[vr::k_EButton_Axis0]);

			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Axis1_Press", &vr_actionHandles.m_leftRole.m_pressedMap[vr::k_EButton_Axis1]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Axis1_Touch", &vr_actionHandles.m_leftRole.m_touchedMap[vr::k_EButton_Axis1]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Axis1_Value", &vr_actionHandles.m_leftRole.m_valueMap[vr::k_EButton_Axis1]);

			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Axis2_Press", &vr_actionHandles.m_leftRole.m_pressedMap[vr::k_EButton_Axis2]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Axis2_Touch", &vr_actionHandles.m_leftRole.m_touchedMap[vr::k_EButton_Axis2]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Axis2_Value1", &vr_actionHandles.m_leftRole.m_value1Map[vr::k_EButton_Axis2]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Axis2_Value2", &vr_actionHandles.m_leftRole.m_valueMap[vr::k_EButton_Axis2]);

			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Axis3_Press", &vr_actionHandles.m_leftRole.m_pressedMap[vr::k_EButton_Axis3]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Axis3_Touch", &vr_actionHandles.m_leftRole.m_touchedMap[vr::k_EButton_Axis3]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Axis3_Value", &vr_actionHandles.m_leftRole.m_valueMap[vr::k_EButton_Axis3]);

			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Axis4_Press", &vr_actionHandles.m_leftRole.m_pressedMap[vr::k_EButton_Axis4]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Axis4_Touch", &vr_actionHandles.m_leftRole.m_touchedMap[vr::k_EButton_Axis4]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Axis4_Value_e0", &vr_actionHandles.m_leftRole.m_value1Map[vr::k_EButton_Axis4]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Left_Axis4_Value_e1", &vr_actionHandles.m_leftRole.m_value2Map[vr::k_EButton_Axis4]);

			error = vrInput->GetActionHandle("/actions/legacy/in/Right_System_Press", &vr_actionHandles.m_rightRole.m_pressedMap[vr::k_EButton_System]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Right_System_Touch", &vr_actionHandles.m_rightRole.m_touchedMap[vr::k_EButton_System]);

			error = vrInput->GetActionHandle("/actions/legacy/in/Right_ApplicationMenu_Press", &vr_actionHandles.m_rightRole.m_pressedMap[vr::k_EButton_ApplicationMenu]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Right_ApplicationMenu_Touch", &vr_actionHandles.m_rightRole.m_touchedMap[vr::k_EButton_ApplicationMenu]);

			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Grip_Press", &vr_actionHandles.m_rightRole.m_pressedMap[vr::k_EButton_Grip]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Grip_Touch", &vr_actionHandles.m_rightRole.m_touchedMap[vr::k_EButton_Grip]);

			error = vrInput->GetActionHandle("/actions/legacy/in/Right_A_Press", &vr_actionHandles.m_rightRole.m_pressedMap[vr::k_EButton_A]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Right_A_Touch", &vr_actionHandles.m_rightRole.m_touchedMap[vr::k_EButton_A]);

			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Axis0_Press", &vr_actionHandles.m_rightRole.m_pressedMap[vr::k_EButton_Axis0]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Axis0_Touch", &vr_actionHandles.m_rightRole.m_touchedMap[vr::k_EButton_Axis0]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Axis0_Value", &vr_actionHandles.m_rightRole.m_valueMap[vr::k_EButton_Axis0]);

			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Axis1_Press", &vr_actionHandles.m_rightRole.m_pressedMap[vr::k_EButton_Axis1]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Axis1_Touch", &vr_actionHandles.m_rightRole.m_touchedMap[vr::k_EButton_Axis1]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Axis1_Value", &vr_actionHandles.m_rightRole.m_valueMap[vr::k_EButton_Axis1]);

			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Axis2_Press", &vr_actionHandles.m_rightRole.m_pressedMap[vr::k_EButton_Axis2]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Axis2_Touch", &vr_actionHandles.m_rightRole.m_touchedMap[vr::k_EButton_Axis2]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Axis2_Value1", &vr_actionHandles.m_rightRole.m_value1Map[vr::k_EButton_Axis2]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Axis2_Value2", &vr_actionHandles.m_rightRole.m_valueMap[vr::k_EButton_Axis2]);

			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Axis3_Press", &vr_actionHandles.m_rightRole.m_pressedMap[vr::k_EButton_Axis3]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Axis3_Touch", &vr_actionHandles.m_rightRole.m_touchedMap[vr::k_EButton_Axis3]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Axis3_Value", &vr_actionHandles.m_rightRole.m_valueMap[vr::k_EButton_Axis3]);

			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Axis4_Press", &vr_actionHandles.m_rightRole.m_pressedMap[vr::k_EButton_Axis4]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Axis4_Touch", &vr_actionHandles.m_rightRole.m_touchedMap[vr::k_EButton_Axis4]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Axis4_Value_e0", &vr_actionHandles.m_rightRole.m_value1Map[vr::k_EButton_Axis4]);
			error = vrInput->GetActionHandle("/actions/legacy/in/Right_Axis4_Value_e1", &vr_actionHandles.m_rightRole.m_value2Map[vr::k_EButton_Axis4]);
		}
	}
}