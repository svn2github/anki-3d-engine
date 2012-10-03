#ifndef ANKI_RESOURCE_SHADER_PROGRAM_PRE_PREPROCESSOR_H
#define ANKI_RESOURCE_SHADER_PROGRAM_PRE_PREPROCESSOR_H

#include "anki/resource/ShaderProgramCommon.h"
#include "anki/util/Vector.h"
#include "anki/util/StdTypes.h"
#include <limits>

namespace anki {

/// Helper class used for shader program loading
///
/// The class fills some of the GLSL spec deficiencies. It adds the include
/// preprocessor directive and the support to have all the shaders in the same
/// file. The file that includes all the shaders is called
/// PrePreprocessor-compatible.
///
/// The preprocessor pragmas are:
///
/// - #pragma anki start <vertexShader | tcShader | teShader |
///                       geometryShader | fragmentShader>
/// - #pragma anki include "<filename>"
/// - #pragma anki transformFeedbackVarying <varName>
///
/// @note The order of the "#pragma anki start" is important
class ShaderProgramPrePreprocessor
{
public:
	/// It loads a file and parses it
	/// @param[in] filename The file to load
	/// @exception Exception
	ShaderProgramPrePreprocessor(const char* filename)
	{
		parseFile(filename);
	}

	/// Destructor does nothing
	~ShaderProgramPrePreprocessor()
	{}

	/// @name Accessors
	/// @{
	const Vector<std::string>& getTranformFeedbackVaryings() const
	{
		return trffbVaryings;
	}

	const std::string& getShaderSource(ShaderType type)
	{
		return output.shaderSources[type];
	}
	/// @}

protected:
	/// The pragma base class
	struct Pragma
	{
		I32 definedLine = -1;

		Bool isDefined() const
		{
			return definedLine != -1;
		}
	};

	struct IncludePragma: Pragma
	{
		std::string filename;
	};

	struct TrffbVaryingPragma: Pragma
	{
		StringList names;
	};

	struct CodeBeginningPragma: Pragma
	{};

	/// The output of the class packed in this struct
	struct Output
	{
		friend class PrePreprocessor;

		/// Names and and ids for transform feedback varyings
		Vector<TrffbVaryingPragma> trffbVaryings;
		Array<std::string, ST_NUM> shaderSources;
	};

	Output output; ///< The most important variable
	StringList trffbVaryings;
	/// The parseFileForPragmas fills this
	Vector<std::string> sourceLines;
	Array<CodeBeginningPragma, ST_NUM> shaderStarts;

	/// Parse a PrePreprocessor formated GLSL file. Use
	/// the accessors to get the output
	/// @param filename The file to parse
	/// @exception Ecxeption
	void parseFile(const char* filename);

	/// A recursive function that parses a file for pragmas and updates
	/// the output
	/// @param filename The file to parse
	/// @param depth The #line in GLSL does not support filename so an
	/// depth it being used. It also tracks the
	/// includance depth
	/// @exception Exception
	void parseFileForPragmas(const std::string& filename, int depth = 0);

	/// @todo
	void parseStartPragma(scanner::Scanner& scanner,
		const std::string& filename, uint depth,
		const Vector<std::string>& lines);

	/// @todo
	void parseIncludePragma(scanner::Scanner& scanner,
		const std::string& filename, uint depth,
		const Vector<std::string>& lines);

	/// @todo
	void parseTrffbVarying(scanner::Scanner& scanner,
		const std::string& filename, uint depth,
		const Vector<std::string>& lines);

	/// Searches inside the Output::attributes or Output::trffbVaryings
	/// vectors
	/// @param vec Output::uniforms or Output::trffbVaryings
	/// @param what The name of the varying or attrib
	/// @return Iterator to the vector
	template<typename Type>
	typename Vector<Type>::const_iterator findNamed(
		const Vector<Type>& vec,
		const std::string& what) const;

	void printSourceLines() const;  ///< For debugging

	/// Add a in the source lines a: #line <line> <depth> // cmnt
	void addLinePreProcExpression(uint line, uint depth, const char* cmnt);
};

template<typename Type>
typename Vector<Type>::const_iterator
	ShaderProgramPrePreprocessor::findNamed(
	const Vector<Type>& vec, const std::string& what) const
{
	typename Vector<Type>::const_iterator it = vec.begin();
	while(it != vec.end() && it->name != what)
	{
		++it;
	}
	return it;
}

} // end namespace anki

#endif
