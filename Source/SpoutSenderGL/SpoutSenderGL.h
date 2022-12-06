#pragma once

// Include this here to avoid - oleidl.h error C2061 : syntax error : identifier 'MSG'
// Reportedly this is because the header file itself includes Windows.h
// which should be included before anything else includes it.
#include <Windows.h>

#include <string>
#include <FFGLSDK.h>
#include "..\..\..\..\SpoutGL\Spout.h"


class SpoutSenderGL : public CFFGLPlugin
{

public:

	SpoutSenderGL();
	~SpoutSenderGL();

	//CFFGLPlugin
	FFResult InitGL( const FFGLViewportStruct* vp ) override;
	FFResult ProcessOpenGL( ProcessOpenGLStruct* pGL ) override;
	FFResult DeInitGL() override;

	FFResult SpoutSenderGL::SetTextParameter( unsigned int index, const char* value ) override;
	char* SpoutSenderGL::GetTextParameter( unsigned int index ) override;

private:

	ffglex::FFGLShader shader;  //!< Utility to help us compile and link some shaders into a program.
	ffglex::FFGLScreenQuad quad;//!< Utility to help us render a full screen quad.

	// Plugin parameters
	Spout sender;
	char UserSenderName[ 256 ]{};


};
