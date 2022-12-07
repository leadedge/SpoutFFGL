/*

	SpoutReceiverGL.cpp

	FFGL 2.2 plugin for receiveing OpenGL textures from a Spout receiver.

	Copy SpoutReceiverGL.dll to "...\Documents\Resolume Arena\Extra Effects"
	or another folder that has been assigned for FreeframeGL plugins.
	Find SpoutReceiverGL in the list of Sources.
	Drop on top of any empty cell. The active sender is immediately detected.
	Other senders can be selected from the SpoutReceiverGL clip window.
	"SpoutPanel" or "SpoutSettings" must have been run at least once.
	Receive with the Spout Demo Receiver or any application with Spout support.

	A log file "SpoutReceiverGL.log" is produced to help trace errors.
	Find it with "SpoutSettings -> Diagnostics -> Logs".


	01.06.13 - first version based on RR-DXGLBridge
			   Copyright 2013 Elio <elio@r-revue.de>
	24.06.14 - major revision using Spout SDK
	08-07-14 - Version 3.000
	17.08.15 - Conversion to FFGL 1.6. 32bit and 64bit
	03.01.19 - VS2017 Version 4.000
	07.12.22 - Revise for FFGL 2.2 from Resolume repo
			   Include Spout log file to help trace errors
			   VS2022 x64/MD. Version 5.0


*/

#include "SpoutReceiverGL.h"
using namespace ffglex;

const FFUInt32 FFPARAM_Select      = 0;

static CFFGLPluginInfo PluginInfo(
	PluginFactory< SpoutReceiverGL >, // Create method
	"LJ71",                      // Plugin unique ID of maximum length 4.
	"SpoutReceiverGL",           // Plugin name
	2,                           // API major version number
	1,                           // API minor version number
	5,                           // Plugin major version number
	0,                           // Plugin minor version number
	FF_SOURCE,                   // Plugin type
	"Receive Spout texture",     // Plugin description
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

// Basic quad draw
static const char _fragmentShaderCode[] = R"(#version 410 core

uniform sampler2D InputTexture;

in vec2 uv;
out vec4 fragColor;

void main()
{
	vec4 color = texture( InputTexture, uv );
	color.a = 1.0; // full alpha
	fragColor = color;
}

)";

SpoutReceiverGL::SpoutReceiverGL()
{
	SetMinInputs( 0 );
	SetMaxInputs( 0 );

	// initial values
	myTexture = 0;

	// Parameters
	SetParamInfo( FFPARAM_Select, "Select", FF_TYPE_EVENT, false );

	// Open a log file to help trace errors
	EnableSpoutLogFile( "SpoutReceiverGL" );
	// EnableSpoutLog(); // Console for debugging

	FFGLLog::LogToHost( "Created SpoutReceiverGL source" );
}

SpoutReceiverGL::~SpoutReceiverGL()
{

}

FFResult SpoutReceiverGL::InitGL( const FFGLViewportStruct* vp )
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

FFResult SpoutReceiverGL::ProcessOpenGL( ProcessOpenGLStruct* pGL )
{
	// Do not receive from self
	if(receiver.GetActiveSender( SenderName ) ) {
		if( strstr( SenderName, "Arena" ) != 0) {
			// Make the next sender in the list active
			if( receiver.GetSenderCount() > 1 )	{
				if(receiver.GetSender(1, SenderName)){
					receiver.SetActiveSender( SenderName );
				}
			}
			return FF_SUCCESS;
		}
	}

	// Try to receive from a sender first
	if( receiver.ReceiveTexture( myTexture, GL_TEXTURE_2D, true, pGL->HostFBO ) ) {

		// Create or resize the receiving texture if updated
		if( receiver.IsUpdated() ) {
			InitTexture(myTexture, receiver.GetSenderWidth(), receiver.GetSenderHeight());
		}
		else {
			ScopedShaderBinding shaderBinding( shader.GetGLID() );
			ScopedSamplerActivation activateSampler( 0 );
			Scoped2DTextureBinding textureBinding( myTexture );
			shader.Set( "InputTexture", 0 );
			shader.Set( "MaxUV", 1.0f, 1.0f );
			quad.Draw();
		}
	}
	
	return FF_SUCCESS;
}

FFResult SpoutReceiverGL::DeInitGL()
{
	if( wglGetCurrentContext() ) {
		receiver.ReleaseReceiver();
		if( myTexture != 0 ) 
			glDeleteTextures( 1, &myTexture );
		myTexture = 0;
	}

	shader.FreeGLResources();
	quad.Release();

	return FF_SUCCESS;
}

FFResult SpoutReceiverGL::SetFloatParameter( unsigned int index, float value )
{
	switch( index )
	{
		// SpoutPanel sender selection
		case FFPARAM_Select:
			if( value > 0 )
				receiver.SelectSender();
		break;

	default:
		break;
	}

	return FF_SUCCESS;
}

float SpoutReceiverGL::GetFloatParameter( unsigned int index )
{
	return 0.0f;
}

// Initialize a texture
void SpoutReceiverGL::InitTexture(GLuint &texID, unsigned int width, unsigned int height)
{
	if( texID != 0 ) {
		glDeleteTextures( 1, &texID );
		texID = 0;
	}

	glGenTextures( 1, &texID );
	glBindTexture( GL_TEXTURE_2D, texID );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glBindTexture( GL_TEXTURE_2D, 0 );

}

