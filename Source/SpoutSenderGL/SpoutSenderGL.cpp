/*

	SpoutSenderGL.cpp

	FFGL 2.2 plugin for sending OpenGL texture to a Spout receiver.
	Sends using GPU for compatible hardware, otherwise via CPU memory.

	Copy SpoutSenderGL.dll to "...\Documents\Resolume Arena\Extra Effects"
	or another folder that has been assigned for FreeframeGL plugins.
	Find SpoutSenderGL in the list of Effects.
	Drop on top of any cell that renders an image.
	Scroll down the clip window and find SpoutSenderGL at the bottom.
	Enter a name for the sender.

	Receive with the Spout Demo Receiver or any application with Spout support.

	A log file "SpoutSenderGL.log" is produced to help trace errors.
	Find it with "Diagnostics -> Logs" with either SpoutSettings
	or the Spout Demo Receiver -> Help.


	01.06.13 - first version based on RR-DXGLBridge
			   Copyright 2013 Elio <elio@r-revue.de>
	24.06.14 - major revision using Spout SDK
	08-07-14 - Version 3.000
	17.08.15 - Conversion to FFGL 1.6. 32bit and 64bit
	03.01.19 - VS2017 Version 4.000
	06.12.22 - Revise for FFGL 2.2 from Resolume repo
			   Based on the Add
			   Including Spout log file to help trace errors
			   VS2022 x64/MD. Version 5.0


*/

#include "SpoutSenderGL.h"
using namespace ffglex;

const FFUInt32 FFPARAM_SharingName = 0;

static CFFGLPluginInfo PluginInfo(
	PluginFactory< SpoutSenderGL >,// Create method
	"LJ70",                      // Plugin unique ID of maximum length 4.
	"SpoutSenderGL",             // Plugin name
	2,                           // API major version number
	1,                           // API minor version number
	5,                           // Plugin major version number
	0,                           // Plugin minor version number
	FF_EFFECT,                   // Plugin type
	"Send Spout texture",        // Plugin description
	"https://spout.zeal.co/"     // About
);

static const char _vertexShaderCode[] = R"(#version 410 core
uniform vec2 MaxUV;

layout( location = 0 ) in vec4 vPosition;
layout( location = 1 ) in vec2 vUV;

out vec2 uv;

void main()
{
	gl_Position = vPosition;
	uv = vUV * MaxUV;
}
)";

// Pass-through
static const char _fragmentShaderCode[] = R"(#version 410 core

uniform sampler2D InputTexture;

in vec2 uv;
out vec4 fragColor;

void main()
{
	vec4 color = texture( InputTexture, uv );
	fragColor = color;
}

)";

SpoutSenderGL::SpoutSenderGL()
{
	SetMinInputs( 1 );
	SetMaxInputs( 1 );

	// initial values
	UserSenderName[ 0 ] = 0;

	// Set parameters
	SetParamInfo( FFPARAM_SharingName, "Sender Name", FF_TYPE_TEXT, "" );

	// Open a log file to help trace errors
	EnableSpoutLogFile( "SpoutsenderGL" );

	FFGLLog::LogToHost( "Created SpoutSenderGL effect" );
}

SpoutSenderGL::~SpoutSenderGL()
{

}

FFResult SpoutSenderGL::InitGL( const FFGLViewportStruct* vp )
{
	if( !shader.Compile( _vertexShaderCode, _fragmentShaderCode ) )
	{
		DeInitGL();
		return FF_FAIL;
	}
	if( !quad.Initialise() )
	{
		DeInitGL();
		return FF_FAIL;
	}

	//Use base-class init as success result so that it retains the viewport.
	return CFFGLPlugin::InitGL( vp );
}

FFResult SpoutSenderGL::ProcessOpenGL( ProcessOpenGLStruct* pGL )
{
	if( pGL->numInputTextures < 1 )
		return FF_FAIL;

	if( pGL->inputTextures[ 0 ] == NULL )
		return FF_FAIL;

	//FFGL requires us to leave the context in a default state on return, so use this scoped binding to help us do that.
	ScopedShaderBinding shaderBinding( shader.GetGLID() );
	//The shader's sampler is always bound to sampler index 0 so that's where we need to bind the texture.
	//Again, we're using the scoped bindings to help us keep the context in a default state.
	ScopedSamplerActivation activateSampler( 0 );
	Scoped2DTextureBinding textureBinding( pGL->inputTextures[ 0 ]->Handle );

	shader.Set( "InputTexture", 0 );

	//The input texture's dimension might change each frame and so might the content area.
	//We're adopting the texture's maxUV using a uniform because that way we dont have to update our vertex buffer each frame.
	FFGLTexCoords maxCoords = GetMaxGLTexCoords( *pGL->inputTextures[ 0 ] );
	shader.Set( "MaxUV", maxCoords.s, maxCoords.t );
	quad.Draw();

	// If there is no name yet, the sender cannot be created
	if( !UserSenderName[ 0 ] )
		return FF_SUCCESS; // keep waiting for a name

	// SendFbo looks after sender creation and re-sizing
	sender.SendFbo( pGL->HostFBO, pGL->inputTextures[ 0 ]->Width, pGL->inputTextures[ 0 ]->Height );

	return FF_SUCCESS;
}

FFResult SpoutSenderGL::DeInitGL()
{
	// OpenGL context required so check
	if( wglGetCurrentContext())
		sender.ReleaseSender();

	shader.FreeGLResources();
	quad.Release();

	return FF_SUCCESS;
}

char* SpoutSenderGL::GetTextParameter(unsigned int index )
{
	if( index == FFPARAM_SharingName )
	{
		if( UserSenderName[ 0 ] != 0 )
			return (char*)UserSenderName;

	}
	return NULL;
}

FFResult SpoutSenderGL::SetTextParameter( unsigned int index, const char* value )
{
	UINT_PTR ucb = 1;

	switch( index )
	{
	case FFPARAM_SharingName:
		// Bad read pointer when the text entry field is empty
		if( !IsBadReadPtr( (const void*)value, ucb ) )
		{
			// Check for changed sender name
			if( UserSenderName[ 0 ] && strcmp( UserSenderName, value ) != 0 )
			{
				// Different name
				sender.ReleaseSender();
			}
			// Give the sender a name
			strcpy_s( UserSenderName, 256, value );
			sender.SetSenderName( UserSenderName );
		}
		break;
	default:
		break;
	}

	return FF_SUCCESS;
}
