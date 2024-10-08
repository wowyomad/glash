#include "glash/glash_pch.hpp"
#include "glash/Renderer/Renderer2D.hpp"
#include "glash/Renderer/RenderCommand.hpp"

#include "glash/Renderer/VertexArray.hpp"
#include "glash/Renderer/Buffer.hpp"
#include "glash/Renderer/Shader.hpp"

namespace Cine
{
	struct RendererData
	{
		Ref<VertexArray> QuadVertexArray;
		Ref<Shader> QuadShaderColor;
		Ref<Shader> QuadShaderTexture;

		glm::mat4 ViewProjection;
	};

	static RendererData s_Data;

	void Renderer2D::Init()
	{
		s_Data.QuadVertexArray = VertexArray::Create();

		float quadVertices[] =
		{
			-0.5f, -0.5f, 0.0f, 1.0f,		0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f,		1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f,		1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 1.0f,		0.0f, 1.0f,
		};
		uint32_t indices[] =
		{
			0, 1, 2,
			2, 3, 0
		};
		Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, sizeof(indices));
		Ref<VertexBuffer> quadVertexBuffer = VertexBuffer::Create(quadVertices, sizeof(quadVertices));
		quadVertexBuffer->SetLayout({
			{ ShaderDataType::Float4, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord"},
			});
		s_Data.QuadVertexArray->AddVertexBuffer(quadVertexBuffer);
		s_Data.QuadVertexArray->SetIndexBuffer(indexBuffer);
		s_Data.QuadShaderColor = Shader::Create("resources/shaders/Renderer2D_Quad.glsl");
		s_Data.QuadShaderTexture = Shader::Create("resources/shaders/Renderer2D_Quad_Texture.glsl");
	}
	void Renderer2D::Shutdown()
	{
	}
	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{ 
		RenderCommand::Clear();
		s_Data.ViewProjection = camera.GetViewProjectionMatrix();
	}
	void Renderer2D::EndScene()
	{

	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::scale(glm::translate(glm::mat4(1.0f), position), glm::vec3(size.x, size.y, 1.0f));

		s_Data.QuadShaderColor->Bind();
		s_Data.QuadShaderColor->SetMat4("u_Transform", transform);
		s_Data.QuadShaderColor->SetMat4("u_ViewProjection", s_Data.ViewProjection);
		s_Data.QuadShaderColor->SetFloat4("u_Color", color);

		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& colorTint)
	{
		glm::mat4 transform = glm::scale(glm::translate(glm::mat4(1.0f), position), glm::vec3(size.x, size.y, 1.0f));

		texture->Bind(0);

		s_Data.QuadShaderTexture->Bind();
		s_Data.QuadShaderTexture->SetMat4("u_Transform", transform);
		s_Data.QuadShaderTexture->SetMat4("u_ViewProjection", s_Data.ViewProjection);
		s_Data.QuadShaderTexture->SetFloat4("u_Color", colorTint);
		s_Data.QuadShaderTexture->SetInt("u_Texture", 0);


		s_Data.QuadVertexArray->Bind();
		RenderCommand::DrawIndexed(s_Data.QuadVertexArray);
	}

	void Renderer2D::DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& colorTint)
	{
		DrawQuad({ position.x, position.y, 0.0f }, size, texture, colorTint);
	}
}