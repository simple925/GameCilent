#pragma once
#include "tinygltf/tiny_gltf.h"

class GLBExtractUtil
{
public:
    static bool Extract(const string& _path, vector<GLB_MeshContainer>& _outMeshes, map<int, TrileMeta>& _outTrileMeta);
    static void ProcessNode(tinygltf::Model& model, int nodeIndex, const Matrix& parent, vector<GLB_MeshContainer>& outMeshes, map<int, TrileMeta>& _outTrileMeta);
    static Matrix GetNodeMatrix(const tinygltf::Node& node);
    static void ExtractMesh(
        tinygltf::Model& model,
        int meshIndex,
        const Matrix& world,
        int trileID,
        string meshName,
        vector<GLB_MeshContainer>& outMeshes
    );

    static int ParseTrileMeta(const tinygltf::Value::Object& node, map<int, TrileMeta>& _outTrileMeta);
    static CollisionType ParseCollisionType(const std::string& s);
    static SurfaceType ParseSurfaceType(const std::string& s);
    static ActorType ParseActorType(const std::string& s);
    //int ParseTrileMeta(const tinygltf::Node& node);

    template<typename T>
    static void ReadAccessor(
        const tinygltf::Model& model,
        int accessorIndex,
        vector<T>& out,
        int elemCount)
    {
        const auto& accessor = model.accessors[accessorIndex];
        const auto& view = model.bufferViews[accessor.bufferView];
        const auto& buffer = model.buffers[view.buffer];

        const unsigned char* dataPtr =
            buffer.data.data() + view.byteOffset + accessor.byteOffset;

        size_t stride = accessor.ByteStride(view);
        if (stride == 0)
            stride = sizeof(T);

        out.resize(accessor.count);

        for (size_t i = 0; i < accessor.count; i++)
        {
            memcpy(&out[i], dataPtr + stride * i, sizeof(T));
        }
    }

    static void ConvertToRGBA(const tinygltf::Image& img, GLB_MeshContainer& container);
    
    static FaceOrientation ParseFaceOrientation(const std::string& s);
};

