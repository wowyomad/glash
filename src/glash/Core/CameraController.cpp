#include "glash/glash_pch.hpp"
#include "glash/Core/CameraController.hpp"

namespace Cine
{
	OrthograhpicCameraController::OrthograhpicCameraController(float aspectRatio, bool canRotate)
		: m_AspectRatio(aspectRatio),
		m_CanRotate(canRotate),
		m_CameraZoom(1.0f),
		m_Camera(-m_AspectRatio * m_CameraZoom, m_AspectRatio* m_CameraZoom, -m_CameraZoom, m_CameraZoom)
	{

	}

	void OrthograhpicCameraController::OnEvent(Event& event)
	{
		CINE_PROFILE_FUNCTION();

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseScrolledEvent>(CINE_BIND_EVENT_FN(OrthograhpicCameraController::OnMouseScrolled));
		dispatcher.Dispatch<WindowResizeEvent>(CINE_BIND_EVENT_FN(OrthograhpicCameraController::OnWindowResized));
	}

	void OrthograhpicCameraController::OnUpdate(Timestep ts)
	{
		CINE_PROFILE_FUNCTION();

		glm::vec2 movement(0.0f);

		if (Input::IsKeyPressed(Key::A))
		{
			movement.x -= 1.0f;
		}
		if (Input::IsKeyPressed(Key::D))
		{
			movement.x += 1.0f;
		}
		if (Input::IsKeyPressed(Key::W))
		{
			movement.y += 1.0f;
		}
		if (Input::IsKeyPressed(Key::S))
		{
			movement.y -= 1.0f;
		}

		if (glm::dot(movement, movement) > 0.0f)
		{
			movement = glm::normalize(movement);
		}
		float cos = std::cos(glm::radians(m_CameraRotation));
		float sin = std::sin(glm::radians(m_CameraRotation));
		m_CameraPosition += glm::vec3(movement.x * cos - movement.y * sin, movement.x * sin + movement.y * cos, 0.0f) * m_CameraZoom * ts.Seconds();

		if (m_CanRotate)
		{
			if (Input::IsKeyPressed(Key::E))
			{
				m_CameraRotation -= CameraRotationSpeed * ts;
			}
			if (Input::IsKeyPressed(Key::Q))
			{
				m_CameraRotation += CameraRotationSpeed * ts;
			}
		}

		float cameraZoomLerp = std::min(1.0f, ts / m_CameraZoomTargetTime);
		m_CameraZoom = std::lerp(m_CameraZoom, m_CameraZoomTarget, cameraZoomLerp);
		m_Camera.SetProjection(-m_AspectRatio * m_CameraZoom, m_AspectRatio * m_CameraZoom, -m_CameraZoom, m_CameraZoom);

		m_Camera.SetTransform(m_CameraPosition, m_CameraRotation);
	}

	void OrthograhpicCameraController::SetTargetZoom(float targetZoom)
	{
		m_CameraZoomTarget = targetZoom;
	}

	void OrthograhpicCameraController::Reset()
	{
		m_CameraPosition = glm::vec3(0.0f);
		m_CameraRotation = 0.0f;
		m_CameraZoom = m_CameraZoomTarget = 1.0f;
	}

	bool OrthograhpicCameraController::OnMouseScrolled(MouseScrolledEvent& event)
	{
		m_CameraZoomTarget -= event.GetVertical() * CameraZoomSpeed;
		m_CameraZoomTarget = std::max(m_CameraZoomTarget, m_CameraZoomMax);

		return false;
	}

	bool OrthograhpicCameraController::OnWindowResized(WindowResizeEvent& event)
	{
		m_AspectRatio = (float)event.GetWidth() / (float)event.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_CameraZoom, m_AspectRatio * m_CameraZoom, -m_CameraZoom, m_CameraZoom);

		return false;
	}
}