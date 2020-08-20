#include "Model.h"
#include "Shader.h"

#include <iostream>

Model::Model()
{
}

Model::~Model()
{
	for (int i = 0; i < VAOs.size(); i++)
	{
		delete VAOs[i];
	}
	for (int i = 0; i < VBOs.size(); i++)
	{
		delete VBOs[i];
	}
	for (int i = 0; i < IBOs.size(); i++)
	{
		delete IBOs[i];
	}
	for (int i = 0; i < m_TextureList.size(); i++)
	{
		delete m_TextureList[i];
	}
	
}

void Model::LoadModel(const std::string & fileName)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName,
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace); //normal map 올바른 적용을 위한 Tengent 계산도 해줌

	if (!scene)
	{
		std::cout << "모델 로드 실패 ( " << fileName << " ) " << importer.GetErrorString() << std::endl;
		return;
	}

	LoadNode(scene->mRootNode, scene);
	LoadMaterials(scene);
}

void Model::ClearModel()
{

}

void Model::RenderModel(Shader& shader)
{
	for (int i = 0; i < VAOs.size(); i++)
	{
		Renderer r;
		r.Draw(*VAOs[i], *IBOs[i], shader);
	}
}

void Model::LoadNode(aiNode * node, const aiScene * scene)
{
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);
	}

	for (int i = 0; i < node->mNumChildren; i++)
	{
		LoadNode(node->mChildren[i], scene);
	}
}

void Model::LoadMesh(aiMesh * mesh, const aiScene * scene)
{
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	//vertex 정보 parse
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		//vertex position
		vertices.insert(vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });

		//vertex uv
		if (mesh->mTextureCoords[0]) //0번째 texture
		{
			vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
		}
		else
		{
			vertices.insert(vertices.end(), { 0.0f, 0.0f });
		}

		//vertex normal
		vertices.insert(vertices.end(), { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z });

		//vertex tangent
		vertices.insert(vertices.end(), { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z });

		//vertex bitangent
		vertices.insert(vertices.end(), { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z });
	}

	//index 정보 parse
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	//vao 생성 VertexArray가 담당
	VertexArray* VAO = new VertexArray;
	VertexBuffer* vb = new VertexBuffer{ &vertices[0], mesh->mNumVertices * 14 * sizeof(float) };
	VertexBufferLayout layout;
	layout.Push<float>(3); //vertex당 3개의 위치를 표현하는 float 데이터
	layout.Push<float>(2); //vertex당 2개의 텍스처 좌표를 표현하는 float 데이터
	layout.Push<float>(3); //vertex당 3개의 법선 벡터를 표현하는 float 데이터
	layout.Push<float>(3); //vertex당 3개의 tangent 벡터를 표현하는 float 데이터
	layout.Push<float>(3); //vertex당 3개의 bi-tangent 벡터를 표현하는 float 데이터
	VAO->AddBuffer(*vb, layout);
	VAOs.push_back(VAO);
	VBOs.push_back(vb);

	IndexBuffer* IBO = new IndexBuffer{ &indices[0], mesh->mNumFaces * 3 };
	IBOs.push_back(IBO);
}

void Model::LoadMaterials(const aiScene * scene)
{
	m_TextureList.resize(scene->mNumMaterials);

	for (size_t i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];

		m_TextureList[i] = nullptr;

		//if there is diffuse texture,
		if (material->GetTextureCount(aiTextureType_DIFFUSE))
		{
			//texture 파일 경로
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("Textures/") + filename;

				m_TextureList[i] = new Texture(texPath.c_str());

				if (!m_TextureList[i])
				{
					std::cout << "텍스처 로딩 실패 : " << texPath << std::endl;
					delete m_TextureList[i];
					m_TextureList[i] = nullptr;
				}
			}
		}

		if (!m_TextureList[i])
		{
			//텍스처가 없을 경우
			m_TextureList[i] = new Texture("res/textures/JBNU.png");
		}
	}
}
