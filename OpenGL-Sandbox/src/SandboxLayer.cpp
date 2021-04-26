#include "SandboxLayer.h"
#include <stb_image/stb_image.h>

using namespace GLCore;
using namespace GLCore::Utils;

SandboxLayer::SandboxLayer()
	:m_CameraController(16.0f / 9.0f)
{
}

SandboxLayer::~SandboxLayer()
{
}

static GLuint LoadTexture(const std::string& path)
{
    int w, h, bits;

    stbi_set_flip_vertically_on_load(1);
    auto* pixels = stbi_load(path.c_str(), &w, &h, &bits, STBI_rgb);
    GLuint textureID;
    glCreateTextures(GL_TEXTURE_2D, 1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    stbi_image_free(pixels);

    return textureID;
}

std::unique_ptr<Shader> m_Shader;
void SandboxLayer::OnAttach()
{
	EnableGLDebugging();

	m_Shader = Shader::FromGLSLTextFiles(
		"assets/batch.vert",
		"assets/batch.frag"
	);

	glUseProgram(m_Shader->GetRendererID());
	auto loc = glGetUniformLocation(m_Shader->GetRendererID(), "u_Textures");
	int samplers[32];
	for (int i = 0; i < 32; i++)
	{
		samplers[i] = i;
	}

	glUniform1iv(loc, 32, samplers);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	Renderer::Init();
	m_CTex = LoadTexture("assets/c.jpg");
	m_GlTex = LoadTexture("assets/opengl.png");
}

void SandboxLayer::OnDetach()
{
	Renderer::Shutdown();
}

void SandboxLayer::OnEvent(Event& event)
{
	// Events here
	m_CameraController.OnEvent(event);
    if (event.GetEventType() == EventType::WindowResize)
    {
		WindowResizeEvent& e = (WindowResizeEvent&)event;
		glViewport(0, 0, e.GetWidth(), e.GetHeight());
    }
}

static void SetUniformMat4(uint32_t shader, const char* name, const glm::mat4& matrix)
{
	int loc = glGetUniformLocation(shader, name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}



void SandboxLayer::OnUpdate(Timestep ts)
{
	m_FPS = (uint32_t)1000.0/ts.GetMilliseconds();
	//LOG_TRACE("FPS: {0}", ts.GetMilliseconds());
	m_CameraController.OnUpdate(ts);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(m_Shader->GetRendererID());

    auto &vp = m_CameraController.GetCamera().GetViewProjectionMatrix();
    SetUniformMat4(m_Shader->GetRendererID(), "u_ViewProj", vp);
    SetUniformMat4(m_Shader->GetRendererID(), "u_Transform", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));

	Renderer::ResetStats();
	Renderer::BeginBatch();

	for (float y = -10.0f; y < 10.0f; y += 0.25f)
	{
		for (float x = -10.0f; x < 10.0f; x += 0.25f)
		{
			glm::vec4 color = { (x + 10) / 20.0f, 0.2f, (y + 10) / 20.0f, 1.0f };
			Renderer::DrawQuad({ x, y }, { 0.2f, 0.2f }, color);
		}
	}

	for (int y = 0; y < 5; y++)
	{
		for (int x = 0; x < 5; x++)
		{
            GLuint tex = (x + y) % 2 == 0 ? m_CTex : m_GlTex;
            Renderer::DrawQuad({ x, y }, { 1.0f, 1.0f }, tex);
		}
	}

	Renderer::DrawQuad(m_QuadPosition, { 1.0f, 1.0f }, m_GlTex);
	Renderer::EndBatch();

	Renderer::Flush();
}

void SandboxLayer::OnImGuiRender()
{
	// ImGui here
    ImGui::Begin("Controls");
    //ImGui::DragFloat2("Quad Position", glm::value_ptr(m_QuadPosition), 0.1f);
    ImGui::Text("Quads: %d", Renderer::GetStats().QuadCount);
    ImGui::Text("Draws: %d", Renderer::GetStats().DrawCount);
	ImGui::Text("FPS: %d", m_FPS);
    ImGui::End();
}
