#pragma once

// Task
void CreateObject(class GameObject* _Object, int LayerIdx);
void ChangeLevel(const wstring& _NextLevelName);
void ChangeLevelState(LEVEL_STATE _NextState);

// DebugRender
void DrawDebugRect(Vec3 _Pos, Vec3 _Scale, Vec3 _Rot, Vec4 _Color, float _Duration, bool _DepthTest = false);
void DrawDebugRect(const Matrix& _matWorld, Vec4 _Color, float _Duration, bool _DepthTest = false);
void DrawDebugCircle(Vec3 _Pos, float _Radius, Vec4 _Color, float _Duration, bool _DepthTest = false);
void DebugPrint(const wchar_t* format, ...);

void SaveWString(FILE* _File, const wstring& _String);
wstring LoadWString(FILE* _File);
void SaveAssetRef(FILE* pFile, class Asset* _Asset);

// 0 ~ 1 로 제한
float Saturate(float _Data);

string WStrToStr(const wstring& _wstr);
wstring StrToWStr(const string& _str);

float Dot(const Vec3& a, const Vec3& b);
Vec3 ToEuler(const Quat& q);
void CreateTestLevel();
