// Copyright (C) 2014, Panagiotis Christopoulos Charitos.
// All rights reserved.
// Code licensed under the BSD License.
// http://www.anki3d.org/LICENSE

#ifndef ANKI_RESOURCE_RESOURCE_MANAGER_H
#define ANKI_RESOURCE_RESOURCE_MANAGER_H

#include "anki/resource/Common.h"
#include "anki/resource/ResourcePointer.h"
#include "anki/util/Vector.h"
#include "anki/util/Functions.h"
#include "anki/util/String.h"

namespace anki {

// Forward
class ConfigSet;
class App;
class GlDevice;
class ResourceManager;

#define ANKI_RESOURCE(rsrc_, name_) \
	class rsrc_; \
	using name_ = ResourcePointer<rsrc_, ResourceManager>;

ANKI_RESOURCE(Animation, AnimationResourcePointer)
ANKI_RESOURCE(TextureResource, TextureResourcePointer)
ANKI_RESOURCE(ProgramResource, ProgramResourcePointer)
ANKI_RESOURCE(Material, MaterialResourcePointer)
ANKI_RESOURCE(Mesh, MeshResourcePointer)
ANKI_RESOURCE(BucketMesh, BucketMeshResourcePointer)
ANKI_RESOURCE(Skeleton, SkeletonResourcePointer)
ANKI_RESOURCE(SkelAnim, SkelAnimResourcePointer)
ANKI_RESOURCE(LightRsrc, LightRsrcResourcePointer)
ANKI_RESOURCE(ParticleEmitterResource, ParticleEmitterResourcePointer)
ANKI_RESOURCE(Script, ScriptResourcePointer)
ANKI_RESOURCE(Model, ModelResourcePointer)

#undef ANKI_RESOURCE

/// @addtogroup resource
/// @{

/// Manage resources of a certain type
template<typename Type, typename TResourceManager>
class TypeResourceManager
{
public:
	using ResourcePointerType = ResourcePointer<Type, TResourceManager>;
	using Container = 
		Vector<ResourcePointerType, HeapAllocator<ResourcePointerType>>;

	TypeResourceManager()
	{}

	~TypeResourceManager()
	{
		ANKI_ASSERT(m_ptrs.size() == 0 
			&& "Forgot to delete some resource ptrs");
	}

	/// @privatesection
	/// @{
	void init(HeapAllocator<U8>& alloc)
	{
		HeapAllocator<ResourcePointerType> alloc2 = alloc;
		Container ptrs(alloc2);
		m_ptrs = std::move(ptrs);
	}

	Bool _findLoadedResource(const CString& filename, ResourcePointerType& ptr)
	{
		auto it = find(filename);
		
		if(it != m_ptrs.end())
		{
			ptr = *it;
			return true;
		}
		else
		{
			return false;
		}
	}

	void _registerResource(ResourcePointerType& ptr)
	{
		ANKI_ASSERT(ptr.getReferenceCount() == 1);
		ANKI_ASSERT(find(ptr.getResourceName()) == m_ptrs.end());
	
		m_ptrs.push_back(ptr);
	}

	void _unregisterResource(ResourcePointerType& ptr)
	{
		auto it = find(ptr.getResourceName());
		ANKI_ASSERT(it != m_ptrs.end());
	
		m_ptrs.erase(it);
	}
	/// @}

private:
	Container m_ptrs;

	typename Container::iterator find(const CString& filename)
	{
		typename Container::iterator it;
		
		for(it = m_ptrs.begin(); it != m_ptrs.end(); ++it)
		{
			if(it->getResourceName() == filename)
			{
				break;
			}
		}

		return it;
	}
};

#define ANKI_RESOURCE(type_) \
	public TypeResourceManager<type_, ResourceManager>

/// Resource manager. It holds a few global variables
class ResourceManager: 
	ANKI_RESOURCE(Animation),
	ANKI_RESOURCE(TextureResource),
	ANKI_RESOURCE(ProgramResource),
	ANKI_RESOURCE(Material),
	ANKI_RESOURCE(Mesh),
	ANKI_RESOURCE(BucketMesh),
	ANKI_RESOURCE(Skeleton),
	ANKI_RESOURCE(SkelAnim),
	ANKI_RESOURCE(LightRsrc),
	ANKI_RESOURCE(ParticleEmitterResource),
	ANKI_RESOURCE(Script),
	ANKI_RESOURCE(Model)
{
public:
	ResourceManager(App* app, const ConfigSet& config);

	const ResourceString& getDataDirectory() const
	{
		return m_dataDir;
	}

	U32 getMaxTextureSize() const
	{
		return m_maxTextureSize;
	}

	U32 getTextureAnisotropy() const
	{
		return m_textureAnisotropy;
	}

	TempResourceString fixResourceFilename(const CString& filename) const;

	/// @privatesection
	/// @{
	ResourceAllocator<U8>& _getAllocator()
	{
		return m_alloc;
	}

	TempResourceAllocator<U8>& _getTempAllocator()
	{
		return m_tmpAlloc;
	}

	App& _getApp()
	{
		return *m_app;
	}

	GlDevice& _getGlDevice();

	/// For materials
	CString _getShaderPostProcessorString() const;

	template<typename T>
	Bool _findLoadedResource(const CString& filename, 
		ResourcePointer<T, ResourceManager>& ptr)
	{
		return TypeResourceManager<T, ResourceManager>::_findLoadedResource(
			filename, ptr);
	}

	template<typename T>
	void _registerResource(ResourcePointer<T, ResourceManager>& ptr)
	{
		TypeResourceManager<T, ResourceManager>::_registerResource(ptr);
	}

	template<typename T>
	void _unregisterResource(ResourcePointer<T, ResourceManager>& ptr)
	{
		TypeResourceManager<T, ResourceManager>::_unregisterResource(ptr);
	}
	/// @}

private:
	App* m_app;
	ResourceAllocator<U8> m_alloc;
	TempResourceAllocator<U8> m_tmpAlloc;
	ResourceString m_dataDir;
	U32 m_maxTextureSize;
	U32 m_textureAnisotropy;
};

#undef ANKI_RESOURCE

/// @}

} // end namespace anki

#endif
