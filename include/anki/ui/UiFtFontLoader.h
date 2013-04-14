#ifndef ANKI_UI_UI_FT_FONT_LOADER_H
#define ANKI_UI_UI_FT_FONT_LOADER_H

#include "anki/Math.h"
#include "anki/util/Vector.h"
#include <boost/range/iterator_range.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

namespace anki {

/// A helper class that uses libfreetype to load glyphs from a font file and
/// gather the metrics for each glyph
class UiFtFontLoader
{
public:
	/// Contains info about the glyphs
	class Glyph
	{
		friend class UiFtFontLoader;

	public:
		FT_Glyph_Metrics getMetrics() const
		{
			return metrics;
		}

	private:
		FT_Glyph glyph;
		FT_Glyph_Metrics metrics;
	};

	enum
	{
		GLYPHS_NUM = 128,
		GLYPH_COLUMNS = 16,
		GLYPH_ROWS = 8
	};

	/// One and only constructor
	UiFtFontLoader(const char* filename, const FT_Vector& fontSize)
	{
		createImage(filename, fontSize);
	}

	/// @name Accessors
	/// @{
	const uint8_t* getImage() const
	{
		return &img[0];
	}

	const FT_Vector& getImageSize() const
	{
		return imgSize;
	}

	boost::iterator_range<Vector<Glyph>::const_iterator>
		getGlyphs() const
	{
		return boost::iterator_range<Vector<Glyph>::const_iterator>(
			glyphs.begin(), glyphs.end());
	}

	uint getLineHeight() const
	{
		return lineHeight;
	}
	/// @}

	/// Save the image (img) to TGA. Its for debugging purposes
	void saveImage(const char* filename) const;

	static FT_Int toPixels(FT_Int a)
	{
		return a >> 6;
	}

private:
	/// @name Data
	/// @{
	FT_Library library;
	FT_Face face;
	Vector<Glyph> glyphs;
	Vector<uint8_t> img;
	FT_Vector imgSize;
	uint32_t lineHeight; ///< Calculated as the max height among all glyphs
	/// @}

	/// Reads the face and extracts the glyphs
	void getAllGlyphs();

	/// Copy one bitmap to img
	void copyBitmap(const uint8_t* srcImg, const FT_Vector& srcSize,
		const FT_Vector& pos);

	/// Compute image size (imgSize) using the glyphs set
	void computeImageSize();

	/// Given a filename and a font size create an image with all the glyphs
	void createImage(const char* filename, const FT_Vector& fontSize);
};

} // end namespace

#endif
