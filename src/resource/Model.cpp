// Copyright (C) 2014, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#include "anki/resource/Model.h"
#include "anki/resource/Material.h"
#include "anki/resource/Mesh.h"
#include "anki/resource/ProgramResource.h"
#include "anki/misc/Xml.h"
#include "anki/core/Logger.h"

namespace anki {

//==============================================================================
// ModelPatchBase                                                              =
//==============================================================================

class Attrib
{
public:
	const char* m_name;
	VertexAttribute m_location;
};

static const Array<Attrib, static_cast<U>(VertexAttribute::COUNT) - 1> 
	attribs = {{
	{"inPosition", VertexAttribute::POSITION},
	{"inNormal", VertexAttribute::NORMAL},
	{"inTangent", VertexAttribute::TANGENT},
	{"inTexCoord", VertexAttribute::TEXTURE_COORD},
	{"inTexCoord1", VertexAttribute::TEXTURE_COORD_1},
	{"inBoneCount", VertexAttribute::BONE_COUNT},
	{"inBoneIds", VertexAttribute::BONE_IDS},
	{"inBoneWeights", VertexAttribute::BONE_WEIGHTS}
}};

//==============================================================================
void ModelPatchBase::createVertexDesc(
	const GlProgramHandle& prog,
	const Mesh& mesh,
	GlCommandBufferHandle& vertexJobs)
{
	GlBufferHandle vbo;
	U32 size;
	GLenum type;
	U32 stride;
	U32 offset;

	U32 count = 0;
	for(const Attrib& attrib : attribs)
	{
		const GlProgramVariable* attr = 
			prog.tryFindVariable(attrib.m_name);

		if(attr == nullptr)
		{
			continue;
		}

		ANKI_ASSERT(attr->getType() == GlProgramVariable::Type::INPUT);

		mesh.getBufferInfo(attrib.m_location, vbo, size, type,
			stride, offset);

		if(!vbo.isCreated())
		{
			throw ANKI_EXCEPTION("Material asks for attribute that the mesh "
				"does not have: %s", attrib.m_name);
		}
		
		vbo.bindVertexBuffer(vertexJobs, size, type, false, stride,
			offset, static_cast<U>(attrib.m_location));

		++count;
	}

	if(count < 1)
	{
		throw ANKI_EXCEPTION("The program doesn't have any attributes");
	}

	// The indices VBO
	mesh.getBufferInfo(VertexAttribute::INDICES, vbo, size, type,
			stride, offset);

	ANKI_ASSERT(vbo.isCreated());
	vbo.bindIndexBuffer(vertexJobs);
}

//==============================================================================
void ModelPatchBase::getRenderingDataSub(
	const RenderingKey& key, 
	GlCommandBufferHandle& vertJobs, 
	GlProgramPipelineHandle& ppline,
	const U8* subMeshIndexArray, 
	U32 subMeshIndexCount,
	Array<U32, ANKI_GL_MAX_SUB_DRAWCALLS>& indicesCountArray,
	Array<PtrSize, ANKI_GL_MAX_SUB_DRAWCALLS>& indicesOffsetArray, 
	U32& drawcallCount) const
{
	// Vertex descr
	vertJobs = m_vertJobs[getVertexDescIdx(key)];

	// Prog
	RenderingKey mtlKey = key;
	mtlKey.m_lod = 
		std::min(key.m_lod, (U8)(getMaterial().getLevelsOfDetail() - 1));

	ppline = m_mtl->getProgramPipeline(mtlKey);

	// Mesh and indices
	RenderingKey meshKey = key;
	meshKey.m_lod = std::min(key.m_lod, (U8)(getMeshesCount() - 1));

	const Mesh& mesh = getMesh(meshKey);

	if(subMeshIndexCount == 0 || subMeshIndexArray == nullptr
		|| mesh.getSubMeshesCount() == 0)
	{
		drawcallCount = 1;
		indicesOffsetArray[0] = 0;
		indicesCountArray[0] = mesh.getIndicesCount();
	}
	else
	{
		ANKI_ASSERT(subMeshIndexCount <= mesh.getSubMeshesCount());

		drawcallCount = 0;
		I prevIndex = -1;
		for(U i = 0; i < subMeshIndexCount; i++)
		{
			I index = (subMeshIndexArray == nullptr) 
				? (I)i
				: (I)subMeshIndexArray[i];
		
			// Check if we can merge with the previous submesh
			if(index > 0 && (index - 1) == prevIndex)
			{
				ANKI_ASSERT(drawcallCount > 0);

				// increase the indices count, leave offset alone
				U32 offset;
				indicesCountArray[drawcallCount - 1] +=
					mesh.getIndicesCountSub((U)index, offset);
			}
			else
			{
				U32 offset;
				indicesCountArray[drawcallCount] =
					mesh.getIndicesCountSub((U)index, offset);

				indicesOffsetArray[drawcallCount] = (PtrSize)offset;

				++drawcallCount;
			}

			prevIndex = index;
		}
	}
}

//==============================================================================
void ModelPatchBase::create(GlDevice* gl)
{
	const Material& mtl = getMaterial();
	U lodsCount = getLodsCount();

	m_vertJobs.resize(lodsCount * mtl.getPassesCount());

	for(U lod = 0; lod < lodsCount; ++lod)
	{
		for(U pass = 0; pass < mtl.getPassesCount(); ++pass)
		{
			RenderingKey key((Pass)pass, lod, false);
			GlProgramHandle prog;
			const Mesh* mesh;

			// Get mesh
			ANKI_ASSERT(getMeshesCount() > 0);
			RenderingKey meshKey = key;
			meshKey.m_lod = std::min(key.m_lod, (U8)(getMeshesCount() - 1));
			mesh = &getMesh(meshKey);

			// Get shader prog
			ANKI_ASSERT(getMaterial().getLevelsOfDetail() > 0);
			RenderingKey shaderKey = key;
			shaderKey.m_lod = std::min(key.m_lod, 
				(U8)(getMaterial().getLevelsOfDetail() - 1));

			GlProgramPipelineHandle ppline =
				m_mtl->getProgramPipeline(shaderKey);
			prog = ppline.getAttachedProgram(GL_VERTEX_SHADER);
			
			// Create vert descriptor
			GlCommandBufferHandle vertJobs(gl);
			createVertexDesc(prog, *mesh, vertJobs);

			m_vertJobs[getVertexDescIdx(key)] = vertJobs;
		}
	}
}

//==============================================================================
U ModelPatchBase::getLodsCount() const
{
	U meshLods = getMeshesCount();
	U mtlLods = getMaterial().getLevelsOfDetail();
	return std::max(meshLods, mtlLods);
}

//==============================================================================
U ModelPatchBase::getVertexDescIdx(const RenderingKey& key) const
{
	U passesCount = getMaterial().getPassesCount();
	ANKI_ASSERT((U)key.m_pass < passesCount);

	// Sanitize LOD
	U lod = std::min((U)key.m_lod, getLodsCount() - 1);

	U idx = lod * passesCount + (U)key.m_pass;
	ANKI_ASSERT(idx < m_vertJobs.size());
	return idx;
}

//==============================================================================
// ModelPatch                                                                  =
//==============================================================================

//==============================================================================
template<typename MeshResourcePointerType>
ModelPatch<MeshResourcePointerType>::ModelPatch(
	CString meshFNames[], 
	U32 meshesCount, 
	const CString& mtlFName,
	ResourceManager* resources)
:	ModelPatchBase(resources->_getAllocator()),
	m_meshResources(resources->_getAllocator())
{
	ANKI_ASSERT(meshesCount > 0);
	m_meshes.resize(meshesCount);
	m_meshResources.resize(meshesCount);

	// Load meshes
	for(U i = 0; i < meshesCount; i++)
	{
		m_meshResources[i].load(meshFNames[i], resources);
		m_meshes[i] = m_meshResources[i].get();

		// Sanity check
		if(i > 0 
			&& !m_meshResources[i]->isCompatible(*m_meshResources[i - 1]))
		{
			throw ANKI_EXCEPTION("Meshes not compatible");
		}
	}

	// Load material
	m_mtlResource.load(mtlFName, resources);
	m_mtl = m_mtlResource.get();

	// Create VAOs
	create(&resources->_getGlDevice());
}

//==============================================================================
// Model                                                                       =
//==============================================================================

//==============================================================================
Model::Model()
{}

//==============================================================================
Model::~Model()
{
	auto alloc = m_modelPatches.get_allocator();

	for(ModelPatchBase* patch : m_modelPatches)
	{
		alloc.deleteInstance(patch);
	}
}

//==============================================================================
void Model::load(const CString& filename, ResourceInitializer& init)
{
	try
	{
		// Load
		//
		XmlDocument doc;
		doc.loadFile(filename, init.m_tempAlloc);

		XmlElement rootEl = doc.getChildElement("model");

		// <collisionShape>
		XmlElement collEl = rootEl.getChildElementOptional("collisionShape");
		if(collEl)
		{
			CString type = collEl.getChildElement("type").getText();
			XmlElement valEl = collEl.getChildElement("value");
			(void)valEl; // XXX

			if(type == "sphere")
			{
				ANKI_LOGW("TODO");
			}
			else if(type == "box")
			{
				ANKI_LOGW("TODO");
			}
			else if(type == "mesh")
			{
				ANKI_LOGW("TODO");
			}
			else
			{
				throw ANKI_EXCEPTION("Incorrect collision type");
			}
		}

		// <modelPatches>
		XmlElement modelPatchesEl =
			rootEl.getChildElement("modelPatches");
		XmlElement modelPatchEl =
			modelPatchesEl.getChildElement("modelPatch");
		do
		{
			XmlElement materialEl =
				modelPatchEl.getChildElement("material");

			Array<CString, 3> meshesFnames;
			U meshesCount = 1;
			ModelPatchBase* patch;

			// Try mesh
			XmlElement meshEl = modelPatchEl.getChildElementOptional("mesh");
			if(meshEl)
			{
				XmlElement meshEl1 =
					modelPatchEl.getChildElementOptional("mesh1");
				XmlElement meshEl2 =
					modelPatchEl.getChildElementOptional("mesh2");

				meshesFnames[0] = meshEl.getText();

				if(meshEl1)
				{
					++meshesCount;
					meshesFnames[1] = meshEl1.getText();
				}

				if(meshEl2)
				{
					++meshesCount;
					meshesFnames[2] = meshEl2.getText();
				}

				patch = init.m_alloc.newInstance<
					ModelPatch<MeshResourcePointer>>(
					&meshesFnames[0], meshesCount, materialEl.getText(),
					&init.m_resources);
			}
			else
			{
				XmlElement bmeshEl =
					modelPatchEl.getChildElement("bucketMesh");
				XmlElement bmeshEl1 =
					modelPatchEl.getChildElementOptional("bucketMesh1");
				XmlElement bmeshEl2 =
					modelPatchEl.getChildElementOptional("bucketMesh2");

				meshesFnames[0] = bmeshEl.getText();

				if(bmeshEl1)
				{
					++meshesCount;
					meshesFnames[1] = bmeshEl1.getText();
				}

				if(bmeshEl2)
				{
					++meshesCount;
					meshesFnames[2] = bmeshEl2.getText();
				}

				patch = init.m_alloc.newInstance<
					ModelPatch<BucketMeshResourcePointer>>(
					&meshesFnames[0], meshesCount, materialEl.getText(),
					&init.m_resources);
			}

			m_modelPatches.push_back(patch);

			// Move to next
			modelPatchEl = modelPatchEl.getNextSiblingElement("modelPatch");
		} while(modelPatchEl);

		// Check number of model patches
		if(m_modelPatches.size() < 1)
		{
			throw ANKI_EXCEPTION("Zero number of model patches");
		}

		// Calculate compound bounding volume
		RenderingKey key;
		key.m_lod = 0;
		m_visibilityShape = m_modelPatches[0]->getMesh(key).getBoundingShape();

		for(auto it = m_modelPatches.begin() + 1;
			it != m_modelPatches.end();
			++it)
		{
			m_visibilityShape = m_visibilityShape.getCompoundShape(
				(*it)->getMesh(key).getBoundingShape());
		}
	}
	catch(std::exception& e)
	{
		throw ANKI_EXCEPTION("Failed to load model") << e;
	}
}

} // end namespace anki
