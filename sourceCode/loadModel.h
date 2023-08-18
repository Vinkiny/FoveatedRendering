#ifndef MODEL_T
#define MODEL_T

#include <stdio.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

#include "xr_linear.h"
#include <gfxwrapper_opengl.h>


#include "stb_image.h"


using namespace std;

namespace objModel {


//vertex
struct Vertex {
    XrVector3f positions;
    XrVector3f normals;
    XrVector2f texcoords;
};

struct Data {
    int width, height, channels;
    unsigned char* stbi_Data;
};

struct Texture {
   // unsigned char* stbi_LoadData;
    vector<Data> stbi_LoadData;
    uint32_t texture_id;
    string path;
};

//mesh
struct Mesh {
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;
    GLuint VAO;
    GLuint VertexBuffer;
    GLuint IndexBuffer;
};

class assimModel {

public:
    // model data 
    vector<Texture> loadTextures;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    vector<Mesh>  meshes;
    string textureFolder;
    
    // 3D model filepath.
    assimModel(string const& path)
    {
        loadModel(path);
    }

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const& path)
    {
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
        // check for errors
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        else
        {
            cout << "get model" << endl;
        }

        textureFolder = path.substr(0, path.find_last_of('/'));

        processOBJ(scene->mRootNode, scene);

    }
    void processOBJ(aiNode* node, const aiScene* scene)
    {

        Mesh temp{};
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {

            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            //meshes.push_back(getMeshes(mesh, scene));


            vector<Vertex> loadVertices;
            vector<Texture> loadTexture;
            vector<unsigned int> loadIndices;
           // unsigned int meshVAO;

            // get vertices 
            for (unsigned int j = 0; j < mesh->mNumVertices; j++)
            {
                Vertex vertex{};
               // XrVector3f temp_vertices{};
                //xyz of vertex
                vertex.positions = {
                    mesh->mVertices[j].x,
                    mesh->mVertices[j].y,
                    mesh->mVertices[j].z
                };
                if (mesh->HasNormals())
                {
                    vertex.normals = {
                        mesh->mNormals[j].x,
                        mesh->mNormals[j].y,
                        mesh->mNormals[j].z
                    };
                }
                if (mesh->mTextureCoords[0])
                {
                    vertex.texcoords = {
                        mesh->mTextureCoords[0][j].x,
                        mesh->mTextureCoords[0][j].y,
                    };

                    // tangent and bitangent add later if needed
                }
                else {
                    XrVector2f tex{ 0.0f,0.0f };
                    vertex.texcoords = tex;
                }

                loadVertices.push_back(vertex);
                temp.vertices = loadVertices;
            }

            //face is triangle
            for (unsigned int k = 0; k < mesh->mNumFaces; k++)
            {
                for (unsigned int f = 0; f < mesh->mFaces[k].mNumIndices; f++)
                {
                    loadIndices.push_back(mesh->mFaces[k].mIndices[f]);
                }
                temp.indices = loadIndices;
            }

            //get materials' data of obj
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            vector<Texture> texture_src = loadTextureSrc(material, aiTextureType_DIFFUSE);
            temp.textures = texture_src;

            meshes.push_back(temp);

        }
        // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processOBJ(node->mChildren[i], scene);
        }
    }

    //load material textures
    vector<Texture> loadTextureSrc(aiMaterial* inputMaterial, aiTextureType textureType)
    {
        vector<Texture> texs;
        for (unsigned int i = 0; i < inputMaterial->GetTextureCount(textureType); i++)
        {
            //get texture path
            aiString fileName;//"diffusejpg"
            inputMaterial->GetTexture(textureType, i, &fileName);

            bool loaded = false;
            for (unsigned int j = 0; j < loadTextures.size(); j++)
            {
                if (strcmp(loadTextures[j].path.c_str(), fileName.C_Str()) == 0)
                {
                    texs.push_back(loadTextures[j]);
                    loaded = true;
                    break;
                }
            }
            if (loaded == false)
            {
                Texture temp;
                temp.stbi_LoadData = stbi_LoadFunc(fileName, this->textureFolder); 
                temp.path = fileName.C_Str();
                texs.push_back(temp);
                loadTextures.push_back(temp);
            }

        }
        return texs;
    }

    vector<Data> stbi_LoadFunc(aiString path, const string& folder)
    {
        vector<Data> stbiData;
        Data loadStbiData{};
        unsigned char* data = nullptr;
        string filename = folder + '/' + string(path.C_Str());
        int width, height, channels;
        data = stbi_load(filename.c_str(), &width, &height, &channels, 0);

        loadStbiData.width = width;
        loadStbiData.height = height;
        loadStbiData.channels = channels;
        loadStbiData.stbi_Data = data;
        stbiData.push_back(loadStbiData);

        return stbiData;
    }
  };
}
#endif