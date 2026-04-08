#pragma once
enum class CB_TYPE
{
	TRANSFORM,	// b0
	MATERIAL,	// b1
	GLOBAL,		// b2
	END,
};

enum class RS_TYPE
{
	CULL_BACK,	// 뒷면(정점 반시계방향으로..)
	CULL_FRONT,	// 앞면
	CULL_NONE,	// Culling X
	WIRE_FRAME,	// 정점과 정점 사이를 잇는 부분을 제외
	END,
};

enum class BS_TYPE
{
	DEFAULT,		// (SrcRGB * 1) + (DestRGB * 0)
	ALPHABLEND,		// (SrcRGB * SrcA) + (DestRGB * (1 - SrcA))
	ONE_ONE,		// (SrcRGB * 1) + (DestRGB * 1 *)
	END,
};

enum class DS_TYPE
{
	LESS,				// 기본옵션 깊이가 더 가까워야 통과됨, 성공 - 깊이쓰기 0
	LESS_EQUL,			// 작거나 같으면 통과, 깊이판정 LESS_EQUAL
	NO_TEST,			// 깊이판정 X(무조건 성공)			성공 - 깊이쓰기 O
	NO_TEST_NO_WRITE,	// 깊이판정 X(무조건 성공)			성공 - 깊이쓰기 X
	END,
};

enum class ASSET_TYPE
{
	MESH,
	MATERIAL,
	TEXTURE,
	SOUND,
	GRAPHICSHADER,
	COMPUTESHADER,
	LEVEL,
	SPRITE,
	FLIPBOOK,
	TILEMAP,
	PREFAB,

	END,
};

enum class COMPONENT_TYPE
{
	TRANSFORM,
	CAMERA,
	RIGIDBODY2D,
	COLLIDER2D,
	COLLIDER3D,

	LIGHT2D,
	LIGHT3D,
	MESHRENDER,
	SPRITE_RENDER,
	BILLBOARD_RENDER,
	FLIPBOOK_RENDER,
	PARTICLE_RENDER,
	TILE_RENDER,
	CLICKABLE,

	END,

	SCRIPT, // 여러가지 스크립트를 가지게 하기 위해서 END 뒤에 선언
};

enum class DIR
{
	RIGHT,
	UP,
	FRONT,

	END,
};

enum TEX_PARAM
{
	TEX_0,
	TEX_1,
	TEX_2,
	TEX_3,
	TEX_4,
	TEX_5,

	TEX_END,
};

enum SCALAR_PARAM
{
	INT_0,
	INT_1,
	INT_2,
	INT_3,

	FLOAT_0,
	FLOAT_1,
	FLOAT_2,
	FLOAT_3,

	VEC2_0,
	VEC2_1,
	VEC2_2,
	VEC2_3,

	VEC4_0,
	VEC4_1,
	VEC4_2,
	VEC4_3,

	MAT_0,
	MAT_1,
};

enum class DBG_SHAPE
{
	RECT,
	CIRCLE,

	CUBE,
	SPHERE,
};
enum class	TASK_TYPE
{
	CREATE_OBJECT,
	DESTROY_OBJECT,
	CHANGE_LEVEL,
	CHANGE_LEVEL_STATE,
};

enum class LIGHT_TYPE
{
	DIRECTIONAL,	// 방향성 광원	-	멀리서 부터 오는 광원에게 설정(빛이 월드로 오는 방향이 모두 같다고 본다)
	POINT,			// 점 광원		-	기본적인 광원, 한 점으로부터 주변으로 빛이 뻗어나감
	SPOT,			// 스포트라이트	-	특수한 광원, 빛을 특정 방향으로 모아서 보내는 것으로 봄
};

// 재질이 랜더링 되는 시점
enum class RENDER_DOMAIN
{
	DOMAIN_OPAQUE,		// 불투명
	DOMAIN_MASKED,		// 불투명 + 완전 투명
	DOMAIN_TRANSPARENT,	// 반투명
	DOMAIN_POSTPROCESS,	// 후처리 - 가장 마지막에 동작, 기존에 그려진 장면을 재 가공
	DOMAIN_DEBUG,
	DOMAIN_NONE,		// 미지정
};

enum class BODY_TYPE
{
	STATIC,
	DYNAMIC,
	KINEMATIC,
};

enum class LEVEL_STATE
{
	PLAY,
	PAUSE,
	STOP,
};