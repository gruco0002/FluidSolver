#pragma once

#include <engine/graphics/Texture2D.hpp>
#include <core/visualizer/ISimulationVisualizer.hpp>
#include <engine/graphics/Framebuffer.hpp>
#include <engine/graphics/Shader.hpp>
#include <core/visualizer/GLParticleVertexArray.hpp>

namespace FluidSolver {

	class GLRenderer {

	public:
		virtual Engine::Graphics::Texture2D* get_render_target() = 0;

		static bool is_opengl_available();
	};



	class GLParticleRenderer : public ISimulationVisualizer, public GLRenderer {

	public:

		virtual Engine::Graphics::Texture2D* get_render_target() override;

		virtual void initialize() override;

		virtual void update_data() override;

		virtual void render() override;

		virtual const Image& get_image_data() override;

		virtual ~GLParticleRenderer() override;

		struct Settings {

			enum ColorSelection {
				Velocity = 0,
				Acceleration = 1,
				Mass = 2,
				Pressure = 3,
				Density = 4,
			} colorSelection = ColorSelection::Density;


			glm::vec4 bottomColor = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
			float bottomValue = 0.0f;

			glm::vec4 topColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
			float topValue = 2.0f;


			bool showParticleSelection = false;

			glm::vec4 boundaryParticleColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

			glm::vec4 backgroundClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

			bool showMemoryLocation = false;


		} settings;

	private:

		glm::mat4 projectionMatrix;
		ParticleVertexArray* particleVertexArray = nullptr;

		Engine::Graphics::Framebuffer* framebuffer = nullptr;
		Engine::Graphics::Texture2D* fboDepthTex = nullptr;
		Engine::Graphics::Texture2D* fboColorTex = nullptr;

		Engine::Graphics::Shader* particleShader = nullptr;

		void create_or_update_fbo();

		void calc_projection_matrix();

	};



}