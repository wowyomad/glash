#pragma once

#include "glash/Glash.hpp"
#include "glash/Core/Application.hpp"
#include "glash/Core/Input.hpp"

#include "glash/Renderer/VertexArray.hpp"
#include "glash/Renderer/Buffer.hpp"
#include "glash/Renderer/Shader.hpp"
#include "glash/Renderer/Renderer.hpp"
#include "glash/Renderer/Texture.hpp"

#include "glash/events/Event.hpp"

#include <GLFW/glfw3.h>

using namespace Cine;

static Application* s_Application = nullptr;

class SimpleLayer : public Layer
{
public:
	SimpleLayer()
		: Layer("Simple Sandbox Layer"),
		m_Camera(-1.0f, 1.0f, -1.0f, 1.0f)
	{
		//float verticesSquare[] = {
		//	-0.5f, -0.5f, 0.0f,		0.8f, 0.2f, 0.3f, 1.0,
		//	 0.5f, -0.5f, 0.0f,		0.2f, 0.8f, 0.3f, 1.0,
		//	 0.5f,  0.5f, 0.0f,		0.2f, 0.3f, 0.8f, 1.0,
		//	-0.5f,  0.5f, 0.0f,		0.2f, 0.8f, 0.3f, 1.0
		//};

		float verticesSquare[] = {
			-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f,		0.0f, 1.0f
		};

		unsigned int indicesSquare[] = {
			0, 1, 2,
			2, 3, 0
		};


		m_VertexArrayTriangle = VertexArray::Create();
		m_VertexArraySquare = VertexArray::Create();

		auto SquareVertexBuffer = VertexBuffer::Create(verticesSquare, sizeof(verticesSquare));
		auto SquareIndexBuffer = IndexBuffer::Create(indicesSquare, sizeof(indicesSquare) / sizeof(float));

		BufferLayout SquareLayout = {
			{ ShaderDataType::Float3, "a_Position" },
			{ ShaderDataType::Float2, "a_TexCoord" }
		};
		SquareVertexBuffer->SetLayout(SquareLayout);

		m_VertexArraySquare->AddVertexBuffer(SquareVertexBuffer);
		m_VertexArraySquare->SetIndexBuffer(SquareIndexBuffer);

		auto specification = TextureSpecification();
		specification.MagFilter = TextureFilter::Nearest;
		specification.Wrap = TextureWrap::Repeat;
		m_Texture = Texture2D::Create("resources/textures/checkerboard.png", specification);

		m_ShaderLibrary.Load("Simple", "resources/shaders/uniform_color.glsl");
		m_Shader = m_ShaderLibrary.Get("Simple");
	}
	void OnFixedUpdate(Timestep fixedDeltaTime)  override
	{

	}

	void OnUpdate(Timestep deltaTime) override
	{
		m_SquarePosition.x = std::lerp(m_SquarePosition.x, m_TargetSquarePosition.x, 1 - powf(m_SquareMoveLerpFactor, deltaTime));
		m_SquarePosition.y = std::lerp(m_SquarePosition.y, m_TargetSquarePosition.y, 1 - powf(m_SquareMoveLerpFactor, deltaTime));
		m_CameraScale = std::lerp(m_CameraScale, m_TargetCameraScale, 1 - powf(m_CameraScaleLerpFactor, deltaTime));


		glm::vec3 squareTranslation(0.0f);
		if (Input::IsKeyDown(Key::A))
		{
			squareTranslation.x -= 1;
		}
		if (Input::IsKeyDown(Key::D))
		{
			squareTranslation.x += 1;
		}
		if (Input::IsKeyDown(Key::W))
		{
			squareTranslation.y += 1;
		}
		if (Input::IsKeyDown(Key::S))
		{
			squareTranslation.y -= 1;
		}

		if (m_TargetCameraScale < m_CameraMinScale)
		{
			m_TargetCameraScale = m_CameraMinScale;
		}

		float ratio = (float)s_Application->GetWindow().GetWidth() / (float)s_Application->GetWindow().GetHeight();
		float halfRation = ratio / 2;
		m_Camera.SetProjection(m_CameraScale * -halfRation, m_CameraScale * halfRation, m_CameraScale * -0.5, m_CameraScale * 0.5f);

		if (m_SquareMoveCooldwon <= 0.0f && glm::dot(squareTranslation, squareTranslation) > 0.0)
		{
			m_TargetSquarePosition += m_SquareSpeed * glm::normalize(squareTranslation);
			m_SquareMoveCooldwon = m_SquareMoveWaitTime;
		}
		else
		{
			m_SquareMoveCooldwon -= deltaTime;
		}

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(m_SquareScale));


		Renderer::BeginScene(m_Camera);
		m_Shader->Bind();
		m_Shader->SetFloat4("u_Color", { 0.4f, 1.0f, 0.3f, 1.0f });
		m_Texture->Bind(0);

		for (size_t i = 0; i < m_SquareRows; i++)
		{
			for (size_t j = 0; j < m_SquareColumns; j++)
			{
				glm::mat4 tranform = glm::translate(glm::mat4(1.0f), m_SquarePosition + glm::vec3(i * m_SquareScale, j * m_SquareScale, 0.0f));
				glm::mat4 squareTransform = tranform * scale;
				if ((i + j) % 2 == 0)
				{
					m_Shader->SetFloat4("u_Color", m_SquareColor1);
				}
				else
				{
					m_Shader->SetFloat4("u_Color", m_SquareColor2);
				}
				Renderer::Submit(m_Shader, m_VertexArraySquare, squareTransform);
			}
		}


		Renderer::EndScene();

	}

	void OnEvent(Event& event) override
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<MouseScrolledEvent>(GLASH_BIND_EVENT_FN(SimpleLayer::OnMouseScrolledEvent));
		dispatcher.Dispatch<WindowResizeEvent>(GLASH_BIND_EVENT_FN(SimpleLayer::OnWindowResizeEvent));
	}

	void OnImGuiRender() override
	{
		ImGui::Begin("Debug");
		ImGui::SliderFloat("Camera Scale", &m_TargetCameraScale, 0.1f, 10.0f, "%.1f");
		ImGui::SliderFloat("Square Speed", &m_SquareSpeed, 0.1f, 10.0f, "%.1f");
		ImGui::SliderInt("Rows", &m_SquareRows, 1, 128);
		ImGui::SliderInt("Columns", &m_SquareColumns, 1, 128);
		ImGui::DragFloat("Square Scale", &m_SquareScale, 0.05f, 0.0f, 10.0f, "%.2f");

		ImGui::DragFloat("Square Move Lerp Factor", &m_SquareMoveLerpFactor, 0.01, 0.01f, 1.0f, "%.2f");
		ImGui::DragFloat("Camera Scale Lerp Factor", &m_CameraScaleLerpFactor, 0.01, 0.01f, 1.0f, "%.2f");

		ImGui::ColorEdit4("Square 1", glm::value_ptr(m_SquareColor1));
		ImGui::ColorEdit4("Square 2", glm::value_ptr(m_SquareColor2));

		if (ImGui::Checkbox("VSync", &m_VSync))
		{
			s_Application->GetWindow().SetVSync(m_VSync);
		}

		ImGui::End();
	}

	bool OnMouseScrolledEvent(MouseScrolledEvent& event)
	{
		m_TargetCameraScale -= m_CameraScaleSpeed * event.GetVertical();
		return true;

	}

	bool OnWindowResizeEvent(WindowResizeEvent& event)
	{
		float ratio = (float)event.GetWidth() / (float)event.GetHeight();
		m_Camera.SetProjection(m_CameraScale * -ratio, m_CameraScale * ratio, m_CameraScale * -1.0, m_CameraScale * 1.0f);
		return false;
	}

private:
	Ref<VertexArray> m_VertexArraySquare;

	glm::vec3 m_SquarePosition = glm::vec3(0.0f);
	glm::vec3 m_TargetSquarePosition = m_SquarePosition;

	float m_SquareMoveLerpFactor = 0.01f;
	float m_SquareSpeed = 0.5f;
	float m_SquareMoveWaitTime = 0.0f;
	float m_SquareMoveCooldwon = 0.0f;
	float m_SquareScale = 1.0f;
	int m_SquareRows = 1;
	int m_SquareColumns = 1;
	glm::vec4 m_SquareColor1 = { 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec4 m_SquareColor2 = { 0.0f, 0.0f, 0.0f, 0.0f };
	
	float m_CameraScale = 1.0f;
	float m_CameraMinScale = 0.1f;
	float m_TargetCameraScale = m_CameraScale;
	float m_CameraScaleSpeed = 0.1f;
	float m_CameraScaleLerpFactor = 0.01f;
	bool m_VSync = false;

	Ref<VertexArray> m_VertexArrayTriangle;
	Ref<Shader> m_Shader;
	ShaderLibrary m_ShaderLibrary;
	Ref<Texture> m_Texture;
	OrthographicCamera m_Camera;

};

class Sandbox : public Application
{
public:
	Sandbox()
	{
		m_Window->SetVSync(false);
		s_Application = this;
		PushOverlay(new SimpleLayer());
	}

	~Sandbox()
	{

	}
};

