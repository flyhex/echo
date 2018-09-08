#pragma once

#include "interface/FrameBuffer.h"
#include "GLES2RenderTargetView.h"
#include "GLES2DepthStencilView.h"

namespace Echo
{
	class GLES2FrameBuffer: public FrameBuffer
	{
	public:
		GLES2FrameBuffer();
		~GLES2FrameBuffer();

		GLES2FrameBuffer(GLuint hFBO);
	public:
		void			clear(Dword clearFlags, const Color& color, float depth, int stencil);
		inline void		swap();
		void			bind();

		GLuint			getFBOHandle() const;

	private:
		GLuint			m_hFBO;
	};
}
