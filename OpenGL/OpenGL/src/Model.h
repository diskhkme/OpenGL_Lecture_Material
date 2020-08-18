#pragma once

#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Renderer.h"

class Model
{
public:
	Model();
	~Model();

	void LoadModel(const std::string& fileName);
	void ClearModel();

	void RenderModel(Shader shader);

private:
	void LoadNode(aiNode* node, const aiScene* scene);
	void LoadMesh(aiMesh* mesh, const aiScene* scene);
	void LoadMaterials(const aiScene* scene);

	std::vector<VertexArray*> VAOs;
	std::vector<VertexBuffer*> VBOs;
	std::vector<IndexBuffer*> IBOs;
	std::vector<Texture*> m_TextureList;
	std::vector<unsigned int> m_MeshToTex;
};