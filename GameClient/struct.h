#pragma once

struct Vertex
{
	Vec3	vPos;	// 3D 공간상에서 정점의 위치
	Vec2	vUV;	// 정점위에 이미지를 띄울 경우, 이미지의 어느지점을 참고하는지 좌표 Texture Coordinate UV
	Vec4	vColor;	// 정점 자체의 색상
	Vec3	vNormal; // [추가] 정점 법선 (Light 연산용)
}; typedef Vertex Vtx;

// DebugInfo : 디버그 렌더링 요청사항 정보
struct DbgInfo
{
	DBG_SHAPE	Shape;
	Vec3		Pos;
	Vec3		Scale;
	Vec3		Rotation;

	Matrix		matWorld;

	Vec4		Color;

	float		Age;		// 현재 수명 0
	float		Life;		// 최대 수명 3

	bool		DepthTest;
};

// TaskMgr 가 처리할 작업 단위
struct TaskInfo
{	
	// DWORD_PTR 64 32 에따라 8 4 바이트 그덕에 주소값도 저장이 가능함 
	TASK_TYPE	Type;
	DWORD_PTR	Param_0;
	DWORD_PTR	Param_1;
	DWORD_PTR	Param_2;
};

// Light2D 정보
struct Light2DInfo
{
	LIGHT_TYPE	Type;
	Vec3		Color;		// 빛의 색상
	Vec3		Ambient;	// 환경광, 광원이 존재하면서 최소한으로 발생하는 빛의 세기 == 반사광의 개념
	Vec3		LightDir;	// 광원의 빛이 향하는 방향
	Vec3		WorldPos;	// 광원의 위치(포인트, 스포트)
	float		Radius;		// 빛의 영향 반경(포인트, 스포트)
	float		Angle;		// SpotLight 범위 각
};

struct Light3DInfo
{
	Vec4        Color;      // 16 bytes
	Vec4        Ambient;    // 16 bytes
	Vec4        Specular;   // 16 bytes

	Vec3        LightPos;   // 12 bytes
	float       Radius;     // 4 bytes  -> 합쳐서 16 bytes (OK!)

	Vec3        LightDir;   // 12 bytes
	int         Type;       // 4 bytes  -> 합쳐서 16 bytes (OK!)

	float       Angle;      // 4 bytes
	Vec3        Padding;    // 12 bytes -> 마지막 줄도 16바이트를 채워주는게 매너!
};

struct TransformMatrix
{
	Matrix matWorld;		// 물체가 채움
	Matrix matView;			// 뷰행렬
	Matrix matProj;			// NDC 좌표계로 투영(압축)시킴
};
// extern, 전방선언, 유일한 전역변수, 모든파일(cpp) 에서 접근 가능
extern TransformMatrix g_Trans;

struct MtrlConst
{
	int		iArr[4];
	float	fArr[4];
	Vec2	v2Arr[4];
	Vec4	v4Arr[4];
	Matrix	mat[2];

	int		IsTex[TEX_END];
	int		Padding[2];
};

struct GlobalData
{
	Vec2	Resolution;		// 화면 해상도
	int		Light2DCount;	// 2D 광원 개수
	int		Light3DCount;	// 3D 광원 개수
	float	DeltaTime;		// DeltaTime
	float	Time;			// 누적 시간값
	float	EngineDT;		// DeltaTime
	float	EngineTime;		// 누적 시간값
};
extern GlobalData g_Global;

struct AABB
{
	Vec3 min;
	Vec3 max;
};

struct LevelDesc
{
	string name;
	Vec3 size;
	Vec3 playerStart;
	int startFace;	// enum
	string sky;
	string bgm;
};

struct GLB_MeshContainer
{
	string name;
	Matrix localMatrix;       // Node Transform 적용
	Vec3 scale;
	Vec4 baseColor;
	bool hasTexture = false;
	int width = 0;
	int height = 0;
	vector<uint8_t> pixelData;
	vector<Vtx> vertices;
	vector<UINT> indices;

	string meshName;
	int trileID;

	// 🔥 새 필드
	int nodeIndex = -1;       // 메시가 속한 노드
	int meshIndex = -1;       // GLTF mesh index
	int primitiveIndex = -1;  // 메시 안의 primitive index
};

enum class FaceOrientation : int
{
	Left,
	Down,
	Back,
	Right,
	Top,
	Front,
};

enum class CollisionType : int
{
	AllSides,
	TopOnly,
	None,
	Immaterial,
	TopNoStraightLedge,
};

enum class ActorType : int
{
	None,
	Ladder,
	Bouncer,
	Sign,
	GoldenCube,
	PickUp,
	Bomb,
	Destructible,
	DestructiblePermanent,
	Vase,
	Door,
	Heart,
	Watcher,
	Crystal,
	BlackHole,
	Vine,
	BigBomb,
	TntBlock,
	TntPickup,
	MotorBlock,
	Hurt,
	Checkpoint,
	TreasureChest,
	CubeShard,
	BigHeart,
	SkeletonKey,
	ExploSwitch,
	PushSwitch,
	EightBitDoor,
	PushSwitchSticky,
	PushSwitchPermanent,
	SuckBlock,
	WarpGate,
	OneBitDoor,
	SpinBlock,
	PivotHandle,
	FourBitDoor,
	LightningPlatform,
	LightningGhost,
	Tombstone,
	SplitUpCube,
	UnlockedDoor,
	Hole,
	Couch,
	Valve,
	Rumbler,
	Waterfall,
	Trickle,
	Drips,
	Geyser,
	ConnectiveRail,
	BoltHandle,
	BoltNutBottom,
	BoltNutTop,
	CodeMachine,
	NumberCube,
	LetterCube,
	TriSkull,
	Tome,
	SecretCube,
	LesserGate,
	Crumbler,
	LaserEmitter,
	LaserBender,
	LaserReceiver,
	RebuildingHexahedron,
	TreasureMap,
	Timeswitch,
	TimeswitchMovingPart,
	Mail,
	Mailbox,
	Bookcase,
	TwoBitDoor,
	SixteenBitDoor,
	ThirtyTwoBitDoor,
	SixtyFourBitDoor,
	Owl,
	Bell,
	RotatingGroup,
	BigWaterfall,
	Telescope,
	SinkPickup,
	QrCode,
	FpsPost,
	PieceOfHeart,
	SecretPassage,
	Piston
};

enum class SurfaceType : int
{
	Grass,
	Metal,
	Stone,
	Wood
};


struct TrileMeta
{
	int         Id = -1;
	std::string Name;
	std::string CubemapPath;

	Vec3        Size = {};
	Vec3        Offset = {};
	Vec2        AtlasOffset = {};

	bool        Immaterial = false;
	bool        SeeThrough = false;
	bool        Thin = false;
	bool        ForceHugging = false;

	SurfaceType  SurfaceType = SurfaceType::Grass;
	FaceOrientation Face = FaceOrientation::Left;
	ActorType    Type = ActorType::None;

	// key: FaceOrientation(int), value: CollisionType(int)
	std::map<FaceOrientation, CollisionType> Faces;
};