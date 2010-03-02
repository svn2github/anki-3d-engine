#pragma anki vertShaderBegins

#pragma anki include "shaders/simple_vert.glsl"

#pragma anki fragShaderBegins

#pragma anki include "shaders/median_filter.glsl"

varying vec2 texCoords;

uniform sampler2D tex;

void main()
{
	#if defined( _PPS_SSAO_PASS_0_ )
		float offset = 1.0 / float(textureSize(tex,0).x);
	#else
		float offset = 1.0 / float(textureSize(tex,0).y);
	#endif
	const int KERNEL_SIZE = 5;
	float kernel[KERNEL_SIZE] = float[]( 0.0 * offset, 
	                                     -1.0 * offset, 1.0 * offset, 
	                                     -2.0 * offset, 2.0 * offset/*,
																			 -3.0 * offset, 3.0 * offset,
																			 -4.0 * offset, 4.0 * offset*/ );

	float factor = 0.0;
	for( int i=0; i<KERNEL_SIZE; i++ )
	{
		#if defined( _PPS_SSAO_PASS_0_ )
			factor += texture2D( tex, texCoords + vec2(kernel[i], 0.0) ).a;
		#else
			factor += texture2D( tex, texCoords + vec2(0.0, kernel[i]) ).a;
		#endif		
	}
	gl_FragData[0].a = factor / float(KERNEL_SIZE);
}
