#pragma once

#include <GLCore.h>
#include <GLCoreUtils.h>
#include "Renderer.h"

class SandboxLayer : public GLCore::Layer
{
public:
	SandboxLayer();
	virtual ~SandboxLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(GLCore::Event& event) override;
	virtual void OnUpdate(GLCore::Timestep ts) override;
	virtual void OnImGuiRender() override;
private:
	//std::unique_ptr<Shader> m_Shader;
	GLCore::Utils::Shader* m_Shader;
	GLuint m_CTex;
	GLuint m_GlTex;
	GLCore::Utils::OrthographicCameraController m_CameraController;
	const glm::vec2& m_QuadPosition = { 1.0f, 1.0f };
	uint32_t m_FPS = 0;
};