#include <filesystem>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include "mesh.hpp"
#include "model.hpp"
#include "object.hpp"
#include "texture_ref.hpp"

#ifdef _WIN32
    const char pathSep = '\\';
#else
    const char pathSep = '/';
#endif

namespace orc
{
    static bool sceneLoadedSuccessfully(const aiScene *const scene)
    {
        return scene && !(scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) && scene->mRootNode;
    }

    static std::filesystem::path getDirPath(std::string path)
    {
        std::filesystem::path fsPath = path;
        return fsPath.parent_path();
    }

    static std::vector<Mesh::Vertex> getVerticesFromMesh(const aiMesh &mesh)
    {
        std::vector<Mesh::Vertex> vertices;

        for(unsigned int i = 0; i < mesh.mNumVertices; i++)
        {
            Mesh::Vertex vertex = {
                .Coordinates = glm::vec3(
                    mesh.mVertices[i].x,
                    mesh.mVertices[i].y,
                    mesh.mVertices[i].z
                ),
                .Normal = glm::vec3(
                    mesh.mNormals[i].x,
                    mesh.mNormals[i].y,
                    mesh.mNormals[i].z
                ),
                .TextureCoords = glm::vec2(0.0f)
            };
            if (mesh.mTextureCoords[0])
            {
                vertex.TextureCoords = glm::vec2(
                    mesh.mTextureCoords[0][i].x,
                    mesh.mTextureCoords[0][i].y
                );
            }

            vertices.push_back(vertex);
        }

        return vertices;
    }

    static std::vector<unsigned int> getIndicesFromMesh(const aiMesh &mesh)
    {
        std::vector<unsigned int> indices;

        for(unsigned int i = 0; i < mesh.mNumFaces; i++)
        {
            for(unsigned int j = 0; j < mesh.mFaces[i].mNumIndices; j++)
            {
                indices.push_back(mesh.mFaces[i].mIndices[j]);
            }
        }

        return indices;
    }

    static std::string getTexturePathFromMaterial(const aiMaterial *const material)
    {
        aiString path;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
        return std::string(path.C_Str());
    }

    static glm::mat4 assimpToGlmMx(aiMatrix4x4 aMx)
    {
        glm::mat4 gMx;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                // We aren't actually transposing the matrix - assimp uses row
                // major matrices while glm uses column major matrices
                gMx[j][i] = aMx[i][j];
            }
        }

        return gMx;
    }

    static void buildGraph(std::filesystem::path dir, const aiScene &scene, const aiNode &node, Object &parent)
    {
        parent.SetTransformMx(assimpToGlmMx(node.mTransformation));

        for (unsigned int i = 0; i < node.mNumMeshes; i ++)
        {
            aiMesh &mesh = *scene.mMeshes[node.mMeshes[i]];

            // If the mesh does not have at least one diffuse texture, skip it
            // TODO: Use default texture instead of skipping
            // TODO: Support embedded textures
            // TODO: Support multiple layers of diffuse and specular maps
            if (mesh.mMaterialIndex >= 0)
            {
                aiMaterial *material = scene.mMaterials[mesh.mMaterialIndex];
                if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
                    parent.AddMesh(std::make_unique<Mesh>(
                        getVerticesFromMesh(mesh),
                        getIndicesFromMesh(mesh),
                        TextureRef(
                            Texture::Type::Diffuse,
                            dir / getTexturePathFromMaterial(material)
                        )
                    ));
                }
            }
        }

        for (unsigned int i = 0; i < node.mNumChildren; i ++)
        {
            std::shared_ptr<Object> child = Object::Create();
            parent.AttachChild(child); 
            buildGraph(dir, scene, *node.mChildren[i], *child);
        }
    }

    std::shared_ptr<Object> LoadModel(std::string path)
    {
        // Import model file and perform some processing:
        // - Transform all primitives to triangles
        // - Generate surface normals if not included
        // - Flip texture coordinates vertically
        Assimp::Importer importer;
        const aiScene *const scene = importer.ReadFile(path, aiProcess_Triangulate|aiProcess_GenNormals|aiProcess_FlipUVs);

        if (!sceneLoadedSuccessfully(scene)) {
            throw std::runtime_error("Failed to load scene at " + path);
        }

        std::filesystem::path fsPath = path;
        std::shared_ptr<Object> root = Object::Create();
        buildGraph(fsPath.parent_path(), *scene, *scene->mRootNode, *root);
        return root;
    }
}
