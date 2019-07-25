#pragma once

#include "interface/Renderable.h"
#include "interface/mesh/Mesh.h"

namespace Echo
{
	class GLES2Renderable : public Renderable
	{
	public:
		// Param structure
		struct ShaderParam
		{
			ui32			physicsIndex;
			ShaderParamType type;
			const void*		data;
			ui32			length;  // shader constance register num.
		};

		// vertex stream bind state(for multi stream)
		enum BindState
		{
			BS_NORMAL = 0,
			BS_BEGIN = 1 << 0,
			BS_END = 1 << 1,
		};

		// vertex declaration
		struct VertexDeclaration
		{
			i32		m_attribute;
			ui32	count;
			ui32	type;
			bool	bNormalize;
			ui32	elementOffset;
		};
		typedef vector<VertexDeclaration>::type	VertexDeclarationList;

		// stream unit
		struct StreamUnit
		{
			VertexElementList		m_vertElements;		// 顶点格式声明
			VertexDeclarationList	m_vertDeclaration;	// 顶点声明
			ui32					m_vertStride;		// 每顶点大小
			GPUBuffer*				m_buffer;			// 顶点数据

			StreamUnit()
				: m_buffer(NULL)
			{}

			bool isUsedFor(VertexSemantic);
		};

	public:
		GLES2Renderable(const String& renderStage, ShaderProgram* shader, int identifier);
		~GLES2Renderable();

		// bind geometry data
		void bind( Renderable* pre);

		// unbind geometry data
		void unbind();

		// param operate
		virtual void setShaderParam(const String& name, ShaderParamType type, const void* param, size_t num=1) override;

		// bind shader params
		virtual void bindShaderParams();

	private:
		virtual void link() override;

		// bind vertex stream
		bool bindVertexStream(const VertexElementList& vertElements, GPUBuffer* vertexBuffer, int flag = BS_BEGIN | BS_END);

		// build vertex declaration
		virtual bool buildVertStreamDeclaration(StreamUnit* stream);

		// generate stream hash
		virtual void generateVertexStreamHash();

	private:
		map<String, ShaderParam>::type	m_shaderParams;
		vector<StreamUnit>::type		m_vertexStreams;
		unsigned int					m_vertexStreamsHash;
		bool							m_is_muti_stream;
		GLuint							m_vao;
	};
}
