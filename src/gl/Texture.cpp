#include "anki/gl/Texture.h"
#include "anki/gl/GlException.h"
#include "anki/util/Exception.h"

namespace anki {

//==============================================================================
// Misc                                                                        =
//==============================================================================

//==============================================================================
static Bool isCompressedInternalFormat(const GLenum internalFormat)
{
	Bool out = false;
	switch(internalFormat)
	{
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		out = true;
		break;
	default:
		out = false;
	}
	return out;
}

//==============================================================================
static Bool isLayeredTarget(const GLenum target)
{
	Bool out = false;
	switch(target)
	{
	case GL_TEXTURE_2D:
	case GL_TEXTURE_CUBE_MAP:
		out = false;
		break;
	case GL_TEXTURE_3D:
	case GL_TEXTURE_2D_ARRAY:
		out = true;
		break;
	default:
		ANKI_ASSERT(0);
		break;
	}
	return out;
}

//==============================================================================
// TextureManager                                                              =
//==============================================================================

//==============================================================================
TextureManager::TextureManager()
{
	GLint tmp;
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &tmp);
	unitsCount = tmp;
	
	mipmapping = true;
	anisotropyLevel = 8;
	compression = true;
}

//==============================================================================
// TextureUnits                                                                =
//==============================================================================

//==============================================================================
TextureUnits::TextureUnits()
{
	units.resize(TextureManagerSingleton::get().getMaxUnitsCount(), 
		Unit{0, 0});
	ANKI_ASSERT(units.size() > 7);

	activeUnit = -1;
	choseUnitTimes = 0;
}

//==============================================================================
I TextureUnits::whichUnit(const Texture& tex)
{
	GLuint glid = tex.getGlId();
	I i = units.size();

	do
	{
		--i;
	} while(i >= 0 && glid != units[i].tex);

	return i;
}

//==============================================================================
void TextureUnits::activateUnit(U unit)
{
	ANKI_ASSERT(unit < units.size());
	if(activeUnit != (I)unit)
	{
		activeUnit = (I)unit;
		glActiveTexture(GL_TEXTURE0 + activeUnit);
	}
}

//==============================================================================
U TextureUnits::choseUnit(const Texture& tex, Bool& allreadyBinded)
{
	++choseUnitTimes;
	I myTexUnit = whichUnit(tex);

	// Already binded => renew it
	//
	if(myTexUnit != -1)
	{
		ANKI_ASSERT(units[myTexUnit].tex == tex.getGlId());
		units[myTexUnit].born = choseUnitTimes;
		allreadyBinded = true;
		return myTexUnit;
	}

	allreadyBinded = false;

	// Find an empty slot for it
	//
	for(U i = 0; i < units.size(); i++)
	{
		if(units[i].tex == 0)
		{
			units[i].tex = tex.getGlId();
			units[i].born = choseUnitTimes;
			return i;
		}
	}

	// Find the older unit and replace the texture
	//
	U64 older = 0;
	for(U i = 1; i < units.size(); ++i)
	{
		if(units[i].born < units[older].born)
		{
			older = i;
		}
	}

	units[older].tex = tex.getGlId();
	units[older].born = choseUnitTimes;
	return older;
}

//==============================================================================
U TextureUnits::bindTexture(const Texture& tex)
{
	Bool allreadyBinded;
	U unit = choseUnit(tex, allreadyBinded);

	if(!allreadyBinded)
	{
		activateUnit(unit);
		glBindTexture(tex.getTarget(), tex.getGlId());
	}

	return unit;
}

//==============================================================================
void TextureUnits::bindTextureAndActivateUnit(const Texture& tex)
{
	Bool allreadyBinded;
	U unit = choseUnit(tex, allreadyBinded);

	activateUnit(unit);

	if(!allreadyBinded)
	{
		glBindTexture(tex.getTarget(), tex.getGlId());
	}
}

//==============================================================================
void TextureUnits::unbindTexture(const Texture& tex)
{
	I unit = whichUnit(tex);
	if(unit == -1)
	{
		return;
	}

	units[unit].tex = 0;
	units[unit].born = 0;
	// There is no need to use GL to unbind
}

//==============================================================================
// Texture                                                                     =
//==============================================================================

//==============================================================================
Texture::~Texture()
{
	TextureUnitsSingleton::get().unbindTexture(*this);
	if(isCreated())
	{
		glDeleteTextures(1, &glId);
	}
}

//==============================================================================
void Texture::create(const Initializer& init)
{
	// Sanity checks
	//
	ANKI_ASSERT(!isCreated());
	ANKI_ASSERT(init.internalFormat > 4 && "Deprecated internal format");
	ANKI_ASSERT(init.width > 0 && init.height > 0);

	// Create
	//
	glGenTextures(1, &glId);
	ANKI_ASSERT(glId != 0);
	target = init.target;
	internalFormat = init.internalFormat;
	format = init.format;
	type = init.type;
	width = init.width;
	height = init.height;
	depth = init.depth;

	// Bind
	TextureUnitsSingleton::get().bindTextureAndActivateUnit(*this);

	// Texture upload
	if(isCompressedInternalFormat(internalFormat))
	{
		if(isLayeredTarget(target))
		{
			glCompressedTexImage3D(target, 0, internalFormat,
				width, height, depth, 0, init.dataSize, init.data);
		}
		else
		{
			ANKI_ASSERT(depth == 0);
			glCompressedTexImage2D(target, 0, internalFormat,
				width, height, 0, init.dataSize, init.data);
		}
	}
	else
	{
		if(isLayeredTarget(target))
		{
			glTexImage3D(target, 0, internalFormat, width, height, depth,
				0, format, type, init.data);
		}
		else
		{
			ANKI_ASSERT(depth == 0);
			glTexImage2D(target, 0, internalFormat, width,
				height, 0, format, type, init.data);
		}
	}

	ANKI_CHECK_GL_ERROR();

	// Set parameters
	if(init.repeat)
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	if(init.mipmapping && init.data)
	{
		glGenerateMipmap(target);
	}

	// If not mipmapping then the filtering cannot be trilinear
	if(init.filteringType == TFT_TRILINEAR && !init.mipmapping)
	{
		setFilteringNoBind(TFT_LINEAR);
	}
	else
	{
		setFilteringNoBind(init.filteringType);
	}

#if ANKI_GL == ANKI_GL_DESKTOP
	if(init.anisotropyLevel > 1)
	{
		glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 
			GLint(init.anisotropyLevel));
	}
#endif

	ANKI_CHECK_GL_ERROR();
}

//==============================================================================
U Texture::bind() const
{
	U unit = TextureUnitsSingleton::get().bindTexture(*this);
#if ANKI_DEBUG
	GLint activeUnit;
	glGetIntegerv(GL_ACTIVE_TEXTURE, &activeUnit);

	GLint bindingPoint;
	switch(target)
	{
	case GL_TEXTURE_2D:
		bindingPoint = GL_TEXTURE_BINDING_2D;
		break;
	case GL_TEXTURE_2D_ARRAY:
		bindingPoint = GL_TEXTURE_BINDING_2D_ARRAY;
		break;
	default:
		ANKI_ASSERT(0 && "Unimplemented");
		break;
	}

	GLint texName;
	glActiveTexture(GL_TEXTURE0 + unit);
	glGetIntegerv(bindingPoint, &texName);

	ANKI_ASSERT(glId == (GLuint)texName);

	glActiveTexture(activeUnit);
#endif
	return unit;
}

//==============================================================================
void Texture::genMipmap()
{
	TextureUnitsSingleton::get().bindTextureAndActivateUnit(*this);
	glGenerateMipmap(target);
}

//==============================================================================
void Texture::setFilteringNoBind(TextureFilteringType filterType)
{
	switch(filterType)
	{
	case TFT_NEAREST:
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		break;
	case TFT_LINEAR:
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		break;
	case TFT_TRILINEAR:
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	default:
		ANKI_ASSERT(0);
		break;
	}

	filtering = filterType;
}

//==============================================================================
void Texture::readPixels(void* pixels, U level)
{
#if ANKI_GL == ANKI_GL_DESKTOP
	TextureUnitsSingleton::get().bindTextureAndActivateUnit(*this);
	glGetTexImage(target, level, format, type, pixels);
#else
	ANKI_ASSERT(0 && "Not supported on GLES");
#endif
}

} // end namespace anki
