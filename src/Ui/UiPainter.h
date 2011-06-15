#ifndef UI_PAINTER_H
#define UI_PAINTER_H

#include <boost/scoped_ptr.hpp>
#include "RsrcPtr.h"
#include "Math.h"
#include "Accessors.h"
#include "Vbo.h"
#include "Vao.h"


namespace Ui {


class Font;


class Painter
{
	public:
		Painter(const Vec2& deviceSize);

		/// @name Accessors
		/// @{
		GETTER_SETTER(Vec2, pos, getPosition, setPosition)
		GETTER_SETTER(Vec4, col, getColor, setColor)
		void setFont(const char* fontFilename, uint nominalWidth, uint nominalHeight);
		const Font& getFont() const {return *font;}
		/// @}

		void drawText(const char* text);
		void drawFormatedText(const char* format, ...);

	private:
		/// @name Data
		/// @{
		boost::scoped_ptr<Font> font;
		RsrcPtr<ShaderProg> sProg;

		Vec2 pos;
		Vec4 col;
		uint tabSize;

		Vbo qPositionsVbo;
		Vbo qIndecesVbo;
		Vao qVao;

		Vec2 deviceSize; ///< The size of the device in pixels
		/// @}

		void init();
};


}


#endif
