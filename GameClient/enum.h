#pragma once
enum class CB_TYPE
{
	TRANSFORM,
	END,
};

enum class BS_TYPE
{
	DEFAULT,		// (SrcRGB * 1) + (DestRGB * 0)
	ALPHABLEND,		// (SrcRGB * SrcA) + (DestRGB * (1 - SrcA))
	ONE_ONE,		// (SrcRGB * 1) + (DestRGB * 1 *)
	END,
};

enum class ASSET_TYPE
{
	MESH,
	TEXTURE,
	SOUND,
	GRAPHICSHADER,
	LEVEL,

	END,
};

enum class COMPONENT_TYPE
{
	TRANSFORM,
	MESHRENDER,
	COLLIDER,
	FLIPBOOK_RENDER,
	PARTICLE_RENDER,
	LIGHT,
	TILE_RENDER,
	END,

	SCRIPT, // 여러가지 스크립트를 가지게 하기 위해서 END 뒤에 선언
};