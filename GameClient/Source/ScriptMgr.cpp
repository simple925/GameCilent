#include "pch.h"
#include "ScriptMgr.h"

#include "Scripts/CCamMoveScript.h"
#include "Scripts/CCubeScript.h"
#include "Scripts/CMapScript.h"
#include "Scripts/CMissileScript.h"
#include "Scripts/CMonsterScript.h"
#include "Scripts/CPlanetControllerScript.h"
#include "Scripts/CPlayerScript.h"
#include "Scripts/CShockWaveScript.h"
#include "Scripts/CStateScript.h"

void ScriptMgr::GetScriptInfo(vector<wstring>& _vec)
{
	_vec.push_back(L"CCamMoveScript");
	_vec.push_back(L"CCubeScript");
	_vec.push_back(L"CMapScript");
	_vec.push_back(L"CMissileScript");
	_vec.push_back(L"CMonsterScript");
	_vec.push_back(L"CPlanetControllerScript");
	_vec.push_back(L"CPlayerScript");
	_vec.push_back(L"CShockWaveScript");
	_vec.push_back(L"CStateScript");
}

CScript * ScriptMgr::GetScript(const wstring& _strScriptName)
{
	if (L"CCamMoveScript" == _strScriptName)
		return new CCamMoveScript;
	if (L"CCubeScript" == _strScriptName)
		return new CCubeScript;
	if (L"CMapScript" == _strScriptName)
		return new CMapScript;
	if (L"CMissileScript" == _strScriptName)
		return new CMissileScript;
	if (L"CMonsterScript" == _strScriptName)
		return new CMonsterScript;
	if (L"CPlanetControllerScript" == _strScriptName)
		return new CPlanetControllerScript;
	if (L"CPlayerScript" == _strScriptName)
		return new CPlayerScript;
	if (L"CShockWaveScript" == _strScriptName)
		return new CShockWaveScript;
	if (L"CStateScript" == _strScriptName)
		return new CStateScript;
	return nullptr;
}

CScript * ScriptMgr::GetScript(UINT _iScriptType)
{
	switch (_iScriptType)
	{
	case (UINT)SCRIPT_TYPE::CAMMOVESCRIPT:
		return new CCamMoveScript;
		break;
	case (UINT)SCRIPT_TYPE::CUBESCRIPT:
		return new CCubeScript;
		break;
	case (UINT)SCRIPT_TYPE::MAPSCRIPT:
		return new CMapScript;
		break;
	case (UINT)SCRIPT_TYPE::MISSILESCRIPT:
		return new CMissileScript;
		break;
	case (UINT)SCRIPT_TYPE::MONSTERSCRIPT:
		return new CMonsterScript;
		break;
	case (UINT)SCRIPT_TYPE::PLANETCONTROLLERSCRIPT:
		return new CPlanetControllerScript;
		break;
	case (UINT)SCRIPT_TYPE::PLAYERSCRIPT:
		return new CPlayerScript;
		break;
	case (UINT)SCRIPT_TYPE::SHOCKWAVESCRIPT:
		return new CShockWaveScript;
		break;
	case (UINT)SCRIPT_TYPE::STATESCRIPT:
		return new CStateScript;
		break;
	}
	return nullptr;
}

const wchar_t * ScriptMgr::GetScriptName(CScript * _pScript)
{
	switch ((SCRIPT_TYPE)_pScript->GetScriptType())
	{
	case SCRIPT_TYPE::CAMMOVESCRIPT:
		return L"CCamMoveScript";
		break;

	case SCRIPT_TYPE::CUBESCRIPT:
		return L"CCubeScript";
		break;

	case SCRIPT_TYPE::MAPSCRIPT:
		return L"CMapScript";
		break;

	case SCRIPT_TYPE::MISSILESCRIPT:
		return L"CMissileScript";
		break;

	case SCRIPT_TYPE::MONSTERSCRIPT:
		return L"CMonsterScript";
		break;

	case SCRIPT_TYPE::PLANETCONTROLLERSCRIPT:
		return L"CPlanetControllerScript";
		break;

	case SCRIPT_TYPE::PLAYERSCRIPT:
		return L"CPlayerScript";
		break;

	case SCRIPT_TYPE::SHOCKWAVESCRIPT:
		return L"CShockWaveScript";
		break;

	case SCRIPT_TYPE::STATESCRIPT:
		return L"CStateScript";
		break;

	}
	return nullptr;
}