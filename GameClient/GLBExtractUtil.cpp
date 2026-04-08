#include "pch.h"
#include "GLBExtractUtil.h"

bool GLBExtractUtil::Extract(const string& _path, vector<GLB_MeshContainer>& _outMeshes, map<int, TrileMeta>& _outTrileMeta)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    string err, warn;

    if (!loader.LoadBinaryFromFile(&model, &err, &warn, _path))
        return false;

    const tinygltf::Scene& scene = model.scenes[model.defaultScene];

    for (int nodeIndex : scene.nodes)
    {
        ProcessNode(model, nodeIndex, Matrix(), _outMeshes, _outTrileMeta);
    }

    return true;
}

void GLBExtractUtil::ProcessNode(tinygltf::Model& model, int nodeIndex, const Matrix& parent, vector<GLB_MeshContainer>& outMeshes, map<int, TrileMeta>& _outTrileMeta)
{
    const tinygltf::Node& node = model.nodes[nodeIndex];

    Matrix local = GetNodeMatrix(node);
    Matrix world = parent * local;
    int trileID = -1;
    string name = "";
    if (node.extras.IsObject())
    {
        const tinygltf::Value::Object& obj = node.extras.Get<tinygltf::Value::Object>();

        // ID
        trileID = ParseTrileMeta(obj, _outTrileMeta);

        // Name
        if (obj.find("Name") != obj.end())
            name = obj.at("Name").Get<std::string>();
    }

    if (node.mesh >= 0)
    {
        ExtractMesh(model, node.mesh, world, trileID, name, outMeshes);
    }

    for (int child : node.children)
    {
        ProcessNode(model, child, world, outMeshes, _outTrileMeta);
    }
}

Matrix GLBExtractUtil::GetNodeMatrix(const tinygltf::Node& node)
{
    if (!node.matrix.empty())
    {
        return Matrix(
            node.matrix[0], node.matrix[1], node.matrix[2], node.matrix[3],
            node.matrix[4], node.matrix[5], node.matrix[6], node.matrix[7],
            node.matrix[8], node.matrix[9], node.matrix[10], node.matrix[11],
            node.matrix[12], node.matrix[13], node.matrix[14], node.matrix[15]
        );
    }

    Matrix T = Matrix();
    Matrix R = Matrix();
    Matrix S = Matrix();

    if (!node.translation.empty())
    {
        T = Matrix::CreateTranslation(
            node.translation[0],
            node.translation[1],
            node.translation[2]
        );
    }

    if (!node.rotation.empty())
    {
        Quat q(
            node.rotation[0],
            node.rotation[1],
            node.rotation[2],
            node.rotation[3]
        );
        R = Matrix::CreateFromQuaternion(q);
    }

    if (!node.scale.empty())
    {
        S = Matrix::CreateScale(
            node.scale[0],
            node.scale[1],
            node.scale[2]
        );
    }

    return S * R * T; // DirectX 기준
}

void GLBExtractUtil::ConvertToRGBA(const tinygltf::Image& img, GLB_MeshContainer& container)
{
    if (img.image.empty()) return;

    container.width = img.width;
    container.height = img.height;

    if (img.component == 4)
    {
        container.pixelData = img.image;
    }
    else if (img.component == 3)
    {
        container.pixelData.resize(img.width * img.height * 4);

        for (size_t i = 0, j = 0; i < img.image.size(); i += 3, j += 4)
        {
            container.pixelData[j + 0] = img.image[i + 0];
            container.pixelData[j + 1] = img.image[i + 1];
            container.pixelData[j + 2] = img.image[i + 2];
            container.pixelData[j + 3] = 255;
        }
    }
}

void GLBExtractUtil::ExtractMesh(
    tinygltf::Model& model,
    int meshIndex,
    const Matrix& world,
    int trileID,
    string meshName,
    vector<GLB_MeshContainer>& outMeshes)
{
    const auto& mesh = model.meshes[meshIndex];

    for (size_t p = 0; p < mesh.primitives.size(); ++p)
    {
        const auto& primitive = mesh.primitives[p];
        GLB_MeshContainer container{};

        // 기본값 (중요)
        container.baseColor = Vec4(1, 1, 1, 1);
        container.hasTexture = false;

        container.name = mesh.name;

        container.meshIndex = meshIndex;
        container.meshName = meshName;
        container.trileID = trileID;

        container.primitiveIndex = (int)p;
        container.localMatrix = world;

        //-----------------------------------
        // POSITION
        //-----------------------------------
        auto posIt = primitive.attributes.find("POSITION");
        if (posIt == primitive.attributes.end())
            continue;

        vector<Vec3> positions;
        ReadAccessor(model, posIt->second, positions, 3);

        container.vertices.resize(positions.size());

        for (size_t i = 0; i < positions.size(); i++)
        {
            Vec3 v = Vec3(positions[i].x, positions[i].y, -positions[i].z);
            container.vertices[i].vPos = v;
        }

        //-----------------------------------
        // NORMAL (🔥 수정됨)
        //-----------------------------------
        auto normalIt = primitive.attributes.find("NORMAL");
        if (normalIt != primitive.attributes.end())
        {
            vector<Vec3> normals;
            ReadAccessor(model, normalIt->second, normals, 3);

            // 🔥 핵심: inverse transpose
            Matrix normalMat = world.Invert().Transpose();

            for (size_t i = 0; i < normals.size(); i++)
            {
                Vec3 n = Vec3(normals[i].x, normals[i].y, -normals[i].z);
                container.vertices[i].vNormal = Vec3::TransformNormal(n, normalMat);
            }
        }

        //-----------------------------------
        // UV
        //-----------------------------------
        auto uvIt = primitive.attributes.find("TEXCOORD_0");
        if (uvIt != primitive.attributes.end())
        {
            vector<Vec2> uvs;
            ReadAccessor(model, uvIt->second, uvs, 2);

            for (size_t i = 0; i < uvs.size(); i++)
            {
                // 필요시 뒤집기
                //container.vertices[i].vUV = Vec2(uvs[i].x, 1.0f - uvs[i].y);
                container.vertices[i].vUV = Vec2(uvs[i].x, uvs[i].y);
            }
        }

        //-----------------------------------
        // INDEX
        //-----------------------------------
        if (primitive.indices >= 0)
        {
            const auto& accessor = model.accessors[primitive.indices];
            const auto& view = model.bufferViews[accessor.bufferView];
            const auto& buffer = model.buffers[view.buffer];

            const unsigned char* dataPtr =
                buffer.data.data() + view.byteOffset + accessor.byteOffset;

            container.indices.resize(accessor.count);

            for (size_t i = 0; i < accessor.count; i++)
            {
                switch (accessor.componentType)
                {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                    container.indices[i] = ((uint8_t*)dataPtr)[i]; break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    container.indices[i] = ((uint16_t*)dataPtr)[i]; break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                    container.indices[i] = ((uint32_t*)dataPtr)[i]; break;
                default:
                    container.indices[i] = 0;
                    break;
                }
            }

            // RH → LH
            for (size_t i = 0; i < container.indices.size(); i += 3)
                swap(container.indices[i + 1], container.indices[i + 2]);
        }

        //-----------------------------------
        // MATERIAL + TEXTURE
        //-----------------------------------
        if (primitive.material >= 0)
        {
            const auto& mat = model.materials[primitive.material];

            // Color
            auto colorIt = mat.values.find("baseColorFactor");
            if (colorIt != mat.values.end())
            {
                const auto& c = colorIt->second.ColorFactor();
                container.baseColor = Vec4(c[0], c[1], c[2], c[3]);
            }

            // Texture
            auto texIt = mat.values.find("baseColorTexture");
            if (texIt != mat.values.end())
            {
                int texIndex = texIt->second.TextureIndex();

                if (texIndex >= 0 && texIndex < model.textures.size())
                {
                    const auto& tex = model.textures[texIndex];

                    if (tex.source >= 0 && tex.source < model.images.size())
                    {
                        const auto& img = model.images[tex.source];

                        ConvertToRGBA(img, container);

                        if (!container.pixelData.empty())
                            container.hasTexture = true;
                    }
                }
            }
        }

        outMeshes.push_back(container);
    }
}



int GLBExtractUtil::ParseTrileMeta(const tinygltf::Value::Object& obj,
    std::map<int, TrileMeta>& _outTrileMeta)
{
    TrileMeta trile{};

    // ── ID (필수) ──────────────────────────────────────────────────────────
    auto itId = obj.find("TrileId");
    if (itId == obj.end() || !itId->second.IsInt())
        return -1;
    trile.Id = itId->second.Get<int>();

    // ── 문자열 필드 ────────────────────────────────────────────────────────
    if (auto it = obj.find("Name");        it != obj.end() && it->second.IsString())
        trile.Name = it->second.Get<std::string>();
    if (auto it = obj.find("CubemapPath"); it != obj.end() && it->second.IsString())
        trile.CubemapPath = it->second.Get<std::string>();

    // ── Vec3 필드 ──────────────────────────────────────────────────────────
    auto ParseVec3 = [&](const std::string& key) -> std::optional<Vec3>
        {
            auto it = obj.find(key);
            if (it == obj.end() || !it->second.IsArray()) return std::nullopt;
            const auto& arr = it->second.Get<tinygltf::Value::Array>();
            if (arr.size() < 3) return std::nullopt;
            return Vec3((float)arr[0].Get<double>(),
                (float)arr[1].Get<double>(),
                (float)arr[2].Get<double>());
        };

    if (auto v = ParseVec3("Size"))   trile.Size = *v;
    if (auto v = ParseVec3("Offset")) trile.Offset = *v;

    // ── Vec2 필드 ──────────────────────────────────────────────────────────
    if (auto it = obj.find("AtlasOffset"); it != obj.end() && it->second.IsArray())
    {
        const auto& arr = it->second.Get<tinygltf::Value::Array>();
        if (arr.size() >= 2)
            trile.AtlasOffset = Vec2((float)arr[0].Get<double>(),
                (float)arr[1].Get<double>());
    }

    // ── bool 플래그 ────────────────────────────────────────────────────────
    auto ParseBool = [&](const std::string& key, bool& out)
        {
            if (auto it = obj.find(key); it != obj.end() && it->second.IsBool())
                out = it->second.Get<bool>();
        };

    ParseBool("Immaterial", trile.Immaterial);
    ParseBool("SeeThrough", trile.SeeThrough);
    ParseBool("Thin", trile.Thin);
    ParseBool("ForceHugging", trile.ForceHugging);

    // ── enum 필드 (int / string 둘 다 처리) ───────────────────────────────
    auto ParseEnumField = [&](const std::string& key, auto& out, auto strParser)
        {
            auto it = obj.find(key);
            if (it == obj.end()) return;
            if (it->second.IsInt())
                out = static_cast<std::remove_reference_t<decltype(out)>>(it->second.Get<int>());
            else if (it->second.IsString())
                out = strParser(it->second.Get<std::string>());
        };

    ParseEnumField("SurfaceType", trile.SurfaceType, ParseSurfaceType);
    ParseEnumField("Face", trile.Face, ParseFaceOrientation);
    ParseEnumField("Type", trile.Type, ParseActorType);

    // ── Faces 맵 ──────────────────────────────────────────────────────────
    if (auto it = obj.find("Faces"); it != obj.end() && it->second.IsObject())
    {
        for (const auto& [key, val] : it->second.Get<tinygltf::Value::Object>())
        {
            FaceOrientation face = ParseFaceOrientation(key);
            CollisionType   collision = CollisionType::None;

            if (val.IsInt())
                collision = static_cast<CollisionType>(val.Get<int>());
            else if (val.IsString())
                collision = ParseCollisionType(val.Get<std::string>());
            else
                continue;

            trile.Faces[face] = collision;
        }
    }

    // ── 저장 ──────────────────────────────────────────────────────────────
    _outTrileMeta.emplace(trile.Id, trile);
    return trile.Id;
}

FaceOrientation GLBExtractUtil::ParseFaceOrientation(const std::string& s)
{
    if (s == "Left")  return FaceOrientation::Left;
    if (s == "Down")  return FaceOrientation::Down;
    if (s == "Back")  return FaceOrientation::Back;
    if (s == "Right") return FaceOrientation::Right;
    if (s == "Top")   return FaceOrientation::Top;
    if (s == "Front") return FaceOrientation::Front;
    return FaceOrientation::Left; // fallback
}

CollisionType GLBExtractUtil::ParseCollisionType(const std::string& s)
{
    if (s == "AllSides")           return CollisionType::AllSides;
    if (s == "TopOnly")            return CollisionType::TopOnly;
    if (s == "None")               return CollisionType::None;
    if (s == "Immaterial")         return CollisionType::Immaterial;
    if (s == "TopNoStraightLedge") return CollisionType::TopNoStraightLedge;
    return CollisionType::None; // fallback
}

SurfaceType GLBExtractUtil::ParseSurfaceType(const std::string& s)
{
    if (s == "Grass") return SurfaceType::Grass;
    if (s == "Metal") return SurfaceType::Metal;
    if (s == "Stone") return SurfaceType::Stone;
    if (s == "Wood")  return SurfaceType::Wood;
    return SurfaceType::Grass; // fallback
}

ActorType GLBExtractUtil::ParseActorType(const std::string& s)
{
    if (s == "None")                 return ActorType::None;
    if (s == "Ladder")               return ActorType::Ladder;
    if (s == "Bouncer")              return ActorType::Bouncer;
    if (s == "Sign")                 return ActorType::Sign;
    if (s == "GoldenCube")           return ActorType::GoldenCube;
    if (s == "PickUp")               return ActorType::PickUp;
    if (s == "Bomb")                 return ActorType::Bomb;
    if (s == "Destructible")         return ActorType::Destructible;
    if (s == "DestructiblePermanent")return ActorType::DestructiblePermanent;
    if (s == "Vase")                 return ActorType::Vase;
    if (s == "Door")                 return ActorType::Door;
    if (s == "Heart")                return ActorType::Heart;
    if (s == "Watcher")              return ActorType::Watcher;
    if (s == "Crystal")              return ActorType::Crystal;
    if (s == "BlackHole")            return ActorType::BlackHole;
    if (s == "Vine")                 return ActorType::Vine;
    if (s == "BigBomb")              return ActorType::BigBomb;
    if (s == "TntBlock")             return ActorType::TntBlock;
    if (s == "TntPickup")            return ActorType::TntPickup;
    if (s == "MotorBlock")           return ActorType::MotorBlock;
    if (s == "Hurt")                 return ActorType::Hurt;
    if (s == "Checkpoint")           return ActorType::Checkpoint;
    if (s == "TreasureChest")        return ActorType::TreasureChest;
    if (s == "CubeShard")            return ActorType::CubeShard;
    if (s == "BigHeart")             return ActorType::BigHeart;
    if (s == "SkeletonKey")          return ActorType::SkeletonKey;
    if (s == "ExploSwitch")          return ActorType::ExploSwitch;
    if (s == "PushSwitch")           return ActorType::PushSwitch;
    if (s == "EightBitDoor")         return ActorType::EightBitDoor;
    if (s == "PushSwitchSticky")     return ActorType::PushSwitchSticky;
    if (s == "PushSwitchPermanent")  return ActorType::PushSwitchPermanent;
    if (s == "SuckBlock")            return ActorType::SuckBlock;
    if (s == "WarpGate")             return ActorType::WarpGate;
    if (s == "OneBitDoor")           return ActorType::OneBitDoor;
    if (s == "SpinBlock")            return ActorType::SpinBlock;
    if (s == "PivotHandle")          return ActorType::PivotHandle;
    if (s == "FourBitDoor")          return ActorType::FourBitDoor;
    if (s == "LightningPlatform")    return ActorType::LightningPlatform;
    if (s == "LightningGhost")       return ActorType::LightningGhost;
    if (s == "Tombstone")            return ActorType::Tombstone;
    if (s == "SplitUpCube")          return ActorType::SplitUpCube;
    if (s == "UnlockedDoor")         return ActorType::UnlockedDoor;
    if (s == "Hole")                 return ActorType::Hole;
    if (s == "Couch")                return ActorType::Couch;
    if (s == "Valve")                return ActorType::Valve;
    if (s == "Rumbler")              return ActorType::Rumbler;
    if (s == "Waterfall")            return ActorType::Waterfall;
    if (s == "Trickle")              return ActorType::Trickle;
    if (s == "Drips")                return ActorType::Drips;
    if (s == "Geyser")               return ActorType::Geyser;
    if (s == "ConnectiveRail")       return ActorType::ConnectiveRail;
    if (s == "BoltHandle")           return ActorType::BoltHandle;
    if (s == "BoltNutBottom")        return ActorType::BoltNutBottom;
    if (s == "BoltNutTop")           return ActorType::BoltNutTop;
    if (s == "CodeMachine")          return ActorType::CodeMachine;
    if (s == "NumberCube")           return ActorType::NumberCube;
    if (s == "LetterCube")           return ActorType::LetterCube;
    if (s == "TriSkull")             return ActorType::TriSkull;
    if (s == "Tome")                 return ActorType::Tome;
    if (s == "SecretCube")           return ActorType::SecretCube;
    if (s == "LesserGate")           return ActorType::LesserGate;
    if (s == "Crumbler")             return ActorType::Crumbler;
    if (s == "LaserEmitter")         return ActorType::LaserEmitter;
    if (s == "LaserBender")          return ActorType::LaserBender;
    if (s == "LaserReceiver")        return ActorType::LaserReceiver;
    if (s == "RebuildingHexahedron") return ActorType::RebuildingHexahedron;
    if (s == "TreasureMap")          return ActorType::TreasureMap;
    if (s == "Timeswitch")           return ActorType::Timeswitch;
    if (s == "TimeswitchMovingPart") return ActorType::TimeswitchMovingPart;
    if (s == "Mail")                 return ActorType::Mail;
    if (s == "Mailbox")              return ActorType::Mailbox;
    if (s == "Bookcase")             return ActorType::Bookcase;
    if (s == "TwoBitDoor")           return ActorType::TwoBitDoor;
    if (s == "SixteenBitDoor")       return ActorType::SixteenBitDoor;
    if (s == "ThirtyTwoBitDoor")     return ActorType::ThirtyTwoBitDoor;
    if (s == "SixtyFourBitDoor")     return ActorType::SixtyFourBitDoor;
    if (s == "Owl")                  return ActorType::Owl;
    if (s == "Bell")                 return ActorType::Bell;
    if (s == "RotatingGroup")        return ActorType::RotatingGroup;
    if (s == "BigWaterfall")         return ActorType::BigWaterfall;
    if (s == "Telescope")            return ActorType::Telescope;
    if (s == "SinkPickup")           return ActorType::SinkPickup;
    if (s == "QrCode")               return ActorType::QrCode;
    if (s == "FpsPost")              return ActorType::FpsPost;
    if (s == "PieceOfHeart")         return ActorType::PieceOfHeart;
    if (s == "SecretPassage")        return ActorType::SecretPassage;
    if (s == "Piston")               return ActorType::Piston;
    return ActorType::None; // fallback
}