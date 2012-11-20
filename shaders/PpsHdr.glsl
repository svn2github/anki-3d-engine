#pragma anki start vertexShader

#pragma anki include "shaders/SimpleVert.glsl"

#pragma anki start fragmentShader

precision mediump float;

uniform sampler2D mediump fai; ///< Its the IS FAI

layout(std140, binding = 0) uniform commonBlock
{
	vec4 exposure_;
};

#define exposure exposure_.x

in vec2 vTexCoords;

layout(location = 0) out vec3 fColor;

const float brightMax = 4.0;

void main()
{
	vec3 color = texture2D(fai, vTexCoords).rgb;

	float luminance = dot(vec3(0.30, 0.59, 0.11), color);
	float yd = exposure * (exposure / brightMax + 1.0) /
		(exposure + 1.0) * luminance;
	color *= yd;
	fColor = color;
}
