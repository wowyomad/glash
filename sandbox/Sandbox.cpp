#include "Sandbox.hpp"

#include "glash/Glash.hpp"

#include "imgui.h"

glash::Application* glash::CreateApplication()
{
	return new Sandbox();
}