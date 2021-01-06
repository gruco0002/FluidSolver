#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace FluidSolver {

	class Image {
	public:

		struct Color {
			unsigned char r = 0;
			unsigned char g = 0;
			unsigned char b = 0;
			unsigned char a = 0;

			Color(unsigned char r, unsigned char g, unsigned char b);

			Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

			Color() = default;

			Color(const glm::vec3& color);

			Color(const glm::vec4& color);

		};

	private:
		size_t v_width;
		size_t v_height;

		std::vector<Color> pixels;

	public:

		Image(size_t width, size_t height);

		size_t width() const;
		size_t height() const;
		size_t size() const;
		Color* data();

		void set(size_t x, size_t y, const Color& color);
		Color& get(size_t x, size_t y);

		void save_as_png(const std::string& filepath) const;


	};


}