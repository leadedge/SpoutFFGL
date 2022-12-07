#pragma once

// Include this here to avoid - oleidl.h error C2061 : syntax error : identifier 'MSG'
// Reportedly this is because the header file itself includes Windows.h
// which should be included before anything else includes it.
#include <Windows.h>

#include <string>
#include <FFGLSDK.h>
#include "..\..\..\..\SpoutGL\Spout.h"


class SpoutReceiverGL : public CFFGLPlugin
{

public:

	SpoutReceiverGL();
	~SpoutReceiverGL();

	//CFFGLPlugin
	FFResult InitGL( const FFGLViewportStruct* vp ) override;
	FFResult ProcessOpenGL( ProcessOpenGLStruct* pGL ) override;
	FFResult DeInitGL() override;

	FFResult SpoutReceiverGL::SetFloatParameter( unsigned int index, float value ) override;
	float SpoutReceiverGL::GetFloatParameter( unsigned int index ) override;

private:

	ffglex::FFGLShader shader;  //!< Utility to help us compile and link some shaders into a program.
	ffglex::FFGLScreenQuad quad;//!< Utility to help us render a full screen quad.

	// Plugin parameters
	GLuint myTexture = 0;
	Spout receiver;
	char SenderName[ 256 ]{};
	void InitTexture(GLuint &texID, unsigned int width, unsigned int height );

};
