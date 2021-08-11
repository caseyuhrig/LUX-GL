#pragma once

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "lux/Primitive/Mesh.hpp"
#include "lux/Renderer/Renderer.hpp"
#include "lux/Renderer/Texture.hpp"


namespace lux {

    class Model : public Creatable<Model>
    {
    public:
        Model(const std::string& path)
        {
            LoadModel(path);
        }

        void Draw(const Ref<Shader>& shader)
        {
            for (unsigned int i = 0; i < m_Meshes.size(); i++)
                //m_Meshes[i].Draw(shader);
                Renderer::Draw(m_Meshes[i], shader);
        }
    private:
        std::vector<Mesh> m_Meshes;
        std::string m_Directory;

        void LoadModel(const std::string& path)
        {
            spdlog::info("Loading Model: {}", path.c_str());
            Assimp::Importer import;
            const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            {
                spdlog::error("ASSIMP: {}", import.GetErrorString());
                return;
            }
            m_Directory = path.substr(0, path.find_last_of('/'));

            ProcessNode(scene->mRootNode, scene);

            spdlog::info("Meshes: {}", m_Meshes.size());
        }

        void ProcessNode(aiNode* node, const aiScene* scene)
        {
            // process all the node's meshes (if any)
            for (unsigned int i = 0; i < node->mNumMeshes; i++)
            {
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                m_Meshes.push_back(ProcessMesh(mesh, scene));
            }
            // then do the same for each of its children
            for (unsigned int i = 0; i < node->mNumChildren; i++)
            {
                ProcessNode(node->mChildren[i], scene);
            }
        }

        Mesh ProcessMesh(aiMesh* ai_mesh, const aiScene* scene)
        {
            Mesh mesh;
            //std::vector<glm::vec3> vertices;
            //std::vector<unsigned int> indices;
            //std::vector<Texture2D> textures;

            for (auto i = 0; i < ai_mesh->mNumVertices; i++)
            {
                glm::vec3 position, normal;
                glm::vec4 color;
                glm::vec2 texCoord;
                // process vertex positions, normals and texture coordinates
                position.x = ai_mesh->mVertices[i].x;
                position.y = ai_mesh->mVertices[i].y;
                position.z = ai_mesh->mVertices[i].z;

                normal.x = ai_mesh->mNormals[i].x;
                normal.y = ai_mesh->mNormals[i].y;
                normal.z = ai_mesh->mNormals[i].z;


                //vertex.Position = vector;
                //[...]
                //vertices.push_back(vertex);
                
                mesh.AddVertex(position, color, texCoord, normal);
            }
            // process indices
            //[...]
            for (auto i = 0; i < ai_mesh->mNumFaces; i++)
            {
                aiFace face = ai_mesh->mFaces[i];
                for (auto j = 0; j < face.mNumIndices; j++)
                {
                    mesh.AddIndex(face.mIndices[j]);
                    //indices.push_back(face.mIndices[j]);
                }
            }
            // process material
            //if (mesh->mMaterialIndex >= 0)
            //{
            //    [...]
            //}

            //return Mesh(vertices, indices); //, textures);
            mesh.Build();
            return mesh;
        }

        std::vector<Texture2D> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName)
        {

        }
    };
}