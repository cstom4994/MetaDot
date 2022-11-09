#ifndef _SDL_METAENGINE_Render_GLES_1_H__
#define _SDL_METAENGINE_Render_GLES_1_H__

#include "renderer_gpu.h"
#include "SDL_platform.h"

#if !defined(SDL_METAENGINE_Render_DISABLE_GLES) && !defined(SDL_METAENGINE_Render_DISABLE_GLES_1)

#ifdef __IPHONEOS__
    #include <OpenGLES/ES1/gl.h>
    #include <OpenGLES/ES1/glext.h>
#else
    #include "GLES/gl.h"
    #include "GLES/glext.h"
#endif

    #define glFrustum glFrustumf
    #define glOrtho glOrthof
    #define glGenerateMipmap glGenerateMipmapOES
    #define glDeleteFramebuffers glDeleteFramebuffersOES
    #define glGenFramebuffers glGenFramebuffersOES
    #define glFramebufferTexture2D glFramebufferTexture2DOES
    #define glCheckFramebufferStatus glCheckFramebufferStatusOES
    #define glBindFramebuffer glBindFramebufferOES
    #define GL_FRAMEBUFFER GL_FRAMEBUFFER_OES
    #define GL_FRAMEBUFFER_BINDING GL_FRAMEBUFFER_BINDING_OES
    #define GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_OES
    #define GL_FRAMEBUFFER_COMPLETE GL_FRAMEBUFFER_COMPLETE_OES

/* It looks like on Raspberry Pi 2/Raspbian, the 
	symbols in library are missing the OES suffix,
	even though the headers seem to be named right.
*/
#ifdef ADD_MISSING_OES_FUNCTIONS
	extern void glBlendEquation(GLenum mode);
	extern void glBlendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);
	extern void glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
#else
    #define glBlendEquation glBlendEquationOES
    #define glBlendEquationSeparate glBlendEquationSeparateOES
    #define glBlendFuncSeparate glBlendFuncSeparateOES
#endif

    #define GL_FUNC_ADD GL_FUNC_ADD_OES
    #define GL_FUNC_SUBTRACT GL_FUNC_SUBTRACT_OES
    #define GL_FUNC_REVERSE_SUBTRACT GL_FUNC_REVERSE_SUBTRACT_OES
    
    #define GL_MIRRORED_REPEAT GL_MIRRORED_REPEAT_OES

#endif


#define METAENGINE_Render_CONTEXT_DATA ContextData_GLES_1
#define METAENGINE_Render_IMAGE_DATA ImageData_GLES_1
#define METAENGINE_Render_TARGET_DATA TargetData_GLES_1



typedef struct ContextData_GLES_1
{
	SDL_Color last_color;
	METAENGINE_Render_bool last_use_texturing;
	unsigned int last_shape;
	METAENGINE_Render_bool last_use_blending;
	METAENGINE_Render_BlendMode last_blend_mode;
	METAENGINE_Render_Rect last_viewport;
	METAENGINE_Render_Camera last_camera;
	METAENGINE_Render_bool last_camera_inverted;
	
	METAENGINE_Render_bool last_depth_test;
	METAENGINE_Render_bool last_depth_write;
	METAENGINE_Render_ComparisonEnum last_depth_function;
	
	METAENGINE_Render_Image* last_image;
	float* blit_buffer;  // Holds sets of 4 vertices and 4 tex coords interleaved (e.g. [x0, y0, z0, s0, t0, ...]).
	unsigned short blit_buffer_num_vertices;
	unsigned short blit_buffer_max_num_vertices;
	unsigned short* index_buffer;  // Indexes into the blit buffer so we can use 4 vertices for every 2 triangles (1 quad)
	unsigned int index_buffer_num_vertices;
	unsigned int index_buffer_max_num_vertices;
} ContextData_GLES_1;

typedef struct ImageData_GLES_1
{
    int refcount;
    METAENGINE_Render_bool owns_handle;
	Uint32 handle;
	Uint32 format;
} ImageData_GLES_1;

typedef struct TargetData_GLES_1
{
    int refcount;
	Uint32 handle;
	Uint32 format;
} TargetData_GLES_1;



#endif
