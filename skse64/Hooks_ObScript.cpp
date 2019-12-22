#include "Hooks_ObScript.h"
#include "ObScript.h"
#include "GameAPI.h"
//#include "GameReferences.h"
#include "skse64_common/SafeWrite.h"
#include "skse64_common/skse_version.h"
#include "GameData.h"
#include "GameObjects.h"
#include "GameReferences.h"
#include "Hooks_Gameplay.h"
#include "GameInput.h"

// currently ForceRSXCrash, could also use ToggleESRAM and several others
static ObScriptCommand * s_hijackedCommand = nullptr;

void Hooks_ObScript_Init()
{
	// instead of hooking the entire classic scripting system, we're just hijacking some unused commands

	for(ObScriptCommand * iter = g_firstConsoleCommand; iter->opcode < kObScript_NumConsoleCommands + kObScript_ConsoleOpBase; ++iter)
	{
		if(!strcmp(iter->longName, "ForceRSXCrash"))
		{
			s_hijackedCommand = iter;
			break;
		}
	}
}

#include "NiObjects.h"
#include "NiExtraData.h"
#include "NiGeometry.h"
#include "NiProperties.h"
#include "NiMaterial.h"
#include "NiRTTI.h"
#include "NiTypes.h"
#include "GameMenus.h"
#include "GameRTTI.h"
#include "GameVR.h"

#ifdef _DEBUG
NiTexturePtr * GetTextureFromIndex(BSLightingShaderMaterial * material, UInt32 index)
{
	switch (index)
	{
	case 0:
		return &material->texture1;
		break;
	case 1:
		return &material->texture2;
		break;
	case 2:
	{
		if (material->GetShaderType() == BSShaderMaterial::kShaderType_FaceGen)
		{
			return &static_cast<BSLightingShaderMaterialFacegen*>(material)->unkB0;
		}
		else if (material->GetShaderType() == BSShaderMaterial::kShaderType_GlowMap)
		{
			return &static_cast<BSLightingShaderMaterialFacegen*>(material)->unkB0;
		}
		else
		{
			return &material->texture3;
		}
	}
	break;
	case 3:
	{
		if (material->GetShaderType() == BSShaderMaterial::kShaderType_FaceGen)
		{
			return &static_cast<BSLightingShaderMaterialFacegen*>(material)->unkA8;
		}
		else if (material->GetShaderType() == BSShaderMaterial::kShaderType_Parallax)
		{
			return &static_cast<BSLightingShaderMaterialParallax*>(material)->unkA0;
		}
		else if (material->GetShaderType() == BSShaderMaterial::kShaderType_Parallax || material->GetShaderType() == BSShaderMaterial::kShaderType_ParallaxOcc)
		{
			return &static_cast<BSLightingShaderMaterialParallaxOcc*>(material)->unkA0;
		}
	}
	break;
	case 4:
	{
		if (material->GetShaderType() == BSShaderMaterial::kShaderType_Eye)
		{
			return &static_cast<BSLightingShaderMaterialEye*>(material)->unkA0;
		}
		else if (material->GetShaderType() == BSShaderMaterial::kShaderType_EnvironmentMap)
		{
			return &static_cast<BSLightingShaderMaterialEnvmap*>(material)->unkA0;
		}
		else if (material->GetShaderType() == BSShaderMaterial::kShaderType_MultilayerParallax)
		{
			return &static_cast<BSLightingShaderMaterialMultiLayerParallax*>(material)->unkA8;
		}
	}
	break;
	case 5:
	{
		if (material->GetShaderType() == BSShaderMaterial::kShaderType_Eye)
		{
			return &static_cast<BSLightingShaderMaterialEye*>(material)->unkA8;
		}
		else if (material->GetShaderType() == BSShaderMaterial::kShaderType_EnvironmentMap)
		{
			return &static_cast<BSLightingShaderMaterialEnvmap*>(material)->unkA0;
		}
		else if (material->GetShaderType() == BSShaderMaterial::kShaderType_MultilayerParallax)
		{
			return &static_cast<BSLightingShaderMaterialMultiLayerParallax*>(material)->unkB0;
		}
	}
	break;
	case 6:
	{
		if (material->GetShaderType() == BSShaderMaterial::kShaderType_FaceGen)
		{
			return &static_cast<BSLightingShaderMaterialFacegen*>(material)->renderedTexture;
		}
		else if (material->GetShaderType() == BSShaderMaterial::kShaderType_MultilayerParallax)
		{
			return &static_cast<BSLightingShaderMaterialMultiLayerParallax*>(material)->unkA0;
		}
	}
	break;
	case 7:
		return &material->texture4;
		break;
	}

	return nullptr;
}

#include <d3d11.h>

void PrintTextureData(NiTexturePtr texture)
{
	if (texture) {
		auto rendererData = texture->rendererData;
		if (rendererData) {
			UInt32 width = rendererData->width;
			UInt32 height = rendererData->height;
			if (width == 0 || height == 0)
			{
				D3D11_TEXTURE2D_DESC desc;
				rendererData->texture->GetDesc(&desc);

				width = desc.Width;
				height = desc.Height;
			}
			_MESSAGE("Texture Dimensions: %dx%d", width, height);
		}
	}
}

void DumpNode(NiAVObject* node)
{
	_MESSAGE("{%s} {%s} {%lX}", node->GetRTTI()->name, node->m_name, node);
	if (node->m_extraDataLen > 0) {
		gLog.Indent();
		for (UInt16 i = 0; i < node->m_extraDataLen; i++) {
			_MESSAGE("{%s} {%s} {%lX}", node->m_extraData[i]->GetRTTI()->name, node->m_extraData[i]->m_pcName, node);
		}
		gLog.Outdent();
	}

	if (node) {
		NiNode * branch = node->GetAsNiNode();
		BSGeometry * geometry = node->GetAsBSGeometry();
		if (geometry) {
			NiPointer<BSShaderProperty> shaderProperty = niptr_cast<BSShaderProperty>(geometry->m_spEffectState);
			if (shaderProperty) {
				BSLightingShaderProperty * lightingShader = ni_cast(shaderProperty, BSLightingShaderProperty);
				if (lightingShader) {
					BSLightingShaderMaterial * material = (BSLightingShaderMaterial *)lightingShader->material;

					gLog.Indent();
					for (int i = 0; i < BSTextureSet::kNumTextures; ++i)
					{
						const char * texturePath = material->textureSet ? material->textureSet->GetTexturePath(i) : nullptr;
						const char * textureName = "";
						NiTexturePtr * texture = GetTextureFromIndex(material, i);
						if (texture && *texture) {
							textureName = texture->get()->name;
						}

						if ((texture && *texture) || texturePath) {
							_MESSAGE("Texture %d - %s (%s)", i, texturePath ? texturePath : "", textureName);
							gLog.Indent();
							PrintTextureData(texture ? *texture : nullptr);
							gLog.Outdent();
						}
					}

					gLog.Outdent();
				}

				BSEffectShaderProperty * effectShader = ni_cast(shaderProperty, BSEffectShaderProperty);
				if (effectShader) {
					gLog.Indent();

					BSEffectShaderMaterial * material = static_cast<BSEffectShaderMaterial*>(effectShader->material);
					const char * texture1Name = "";
					auto texture1 = material->unk58;
					if (texture1) {
						texture1Name = texture1->name ? texture1->name : "";

						_MESSAGE("Texture (Src) - %s (%s)", material->sourceTexture.c_str(), texture1Name);
						gLog.Indent();
						PrintTextureData(texture1);
						gLog.Outdent();
					}
					const char * texture2Name = "";
					auto texture2 = material->unk60;
					if (texture2) {
						texture2Name = texture2->name ? texture2->name : "";

						_MESSAGE("Texture (Grey) - %s (%s)", material->greyscaleTexture.c_str(), texture2Name);
						gLog.Indent();
						PrintTextureData(texture2);
						gLog.Outdent();
					}

					gLog.Outdent();
				}
			}
		}
		else if (branch) {
			gLog.Indent();
			for (int i = 0; i < branch->m_children.m_emptyRunStart; i++)
			{
				NiAVObject * object = branch->m_children.m_data[i];
				if (object) {
					DumpNode(object);
				}
			}
			gLog.Outdent();
		}
	}
}
#endif

bool GetSKSEVersion_Execute(const ObScriptParam * paramInfo, ScriptData * scriptData, TESObjectREFR * thisObj, TESObjectREFR* containingObj, Script* scriptObj, ScriptLocals* locals, double& result, UInt32& opcodeOffsetPtr)
{
	_MESSAGE("GetSKSEVersion_Execute");

	if (thisObj && thisObj->formType == Actor::kTypeID)
	{
		Actor* actor = static_cast<Actor*>(thisObj);
		actor->magicTarget.VisitActiveEffects([](ActiveEffect* ae) -> BSContainer::ForEachResult
		{
			if (ae && ae->effect && ae->effect->mgef)
			{
				_MESSAGE("ActiveEffect: %s, magnitude: %g, duration:%g, formID: %08x", ae->effect->mgef->fullName.GetName(), ae->magnitude, ae->duration, ae->effect->mgef->formID);
			}
			return BSContainer::ForEachResult::kContinue;
		});
	}

	DumpClass(*g_openVR, sizeof(BSOpenVR) / 8);

	InputEventDispatcher* inputDispatcher = InputEventDispatcher::GetSingleton();
	if (inputDispatcher) {
		DumpClass(inputDispatcher, sizeof(InputEventDispatcher) / 8 + 100);


		for (int i = kDeviceType_VivePrimary; i < kDeviceType_Num; ++i)
		{
			static_cast<BSTrackedControllerDevice*>(inputDispatcher->devices[i])->actionMap.Dump();
			static_cast<BSTrackedControllerDevice*>(inputDispatcher->devices[i])->hardwareMap.Dump();
		}
	}

	InputManager	* inputManager = InputManager::GetSingleton();
	if (inputManager) {
		
	}
	Inventory3DManager* inventoryManager = Inventory3DManager::GetSingleton();
	if (inventoryManager) {
		DumpClass(inventoryManager, 200);
	}

	MapMenu * mapMenu = DYNAMIC_CAST(MenuManager::GetSingleton()->GetMenu(&UIStringHolder::GetSingleton()->mapMenu), IMenu, MapMenu);
	if (mapMenu) {
		//DumpClass(mapMenu, sizeof(MapMenu) / 8);
	}
	RaceSexMenu * raceSexMenu = DYNAMIC_CAST(MenuManager::GetSingleton()->GetMenu(&UIStringHolder::GetSingleton()->raceSexMenu), IMenu, RaceSexMenu);
	if (raceSexMenu) {
		DumpClass(raceSexMenu, sizeof(RaceSexMenu) / 8);
	}

	Console_Print("SKSE64 version: %d.%d.%d, release idx %d, runtime %08X",
		SKSE_VERSION_INTEGER, SKSE_VERSION_INTEGER_MINOR, SKSE_VERSION_INTEGER_BETA,
		SKSE_VERSION_RELEASEIDX, RUNTIME_VERSION);

	return true;
}

void Hooks_ObScript_Commit()
{
	ObScriptCommand cmd = *s_hijackedCommand;

	cmd.longName = "GetSKSEVersion";
	cmd.shortName = "skse";
	cmd.helpText = "";
	cmd.needsParent = 0;
	cmd.numParams = 0;
	cmd.execute = GetSKSEVersion_Execute;
	cmd.flags = 0;

	SafeWriteBuf((uintptr_t)s_hijackedCommand, &cmd, sizeof(cmd));
}
