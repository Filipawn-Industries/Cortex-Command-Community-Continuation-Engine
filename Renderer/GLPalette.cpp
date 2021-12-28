#include "GLPalette.h"
#include "GL/glew.h"
#include <SDL2/SDL_pixels.h>
#include "GLCheck.h"
namespace RTE {
	Palette::Palette(const std::array<glm::u8vec4, PALETTESIZE> &data) :
	    m_Palette(data) {
		glCheck(glGenTextures(1, &m_Texture));
		glCheck(glActiveTexture(GL_TEXTURE1));
		glCheck(glBindTexture(GL_TEXTURE_1D, m_Texture));
		glCheck(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
		glCheck(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
		glCheck(glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		glCheck(glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, PALETTESIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Palette.data()));
		glCheck(glGenerateMipmap(GL_TEXTURE_1D));
		glCheck(glBindTexture(GL_TEXTURE_1D, 0));
		glCheck(glActiveTexture(GL_TEXTURE0));

		m_SDLPalette = std::unique_ptr<SDL_Palette, sdl_palette_deleter>(SDL_AllocPalette(PALETTESIZE));
		SDL_SetPaletteColors(m_SDLPalette.get(), static_cast<SDL_Color *>(static_cast<void *>(m_Palette.data())), 0, PALETTESIZE);
	}

	Palette::~Palette() {}

	SDL_Palette *Palette::GetAsPalette() {
		return m_SDLPalette.get();
	}

	unsigned char Palette::GetIndexFromColor(const glm::vec4 &color) {
		SDL_PixelFormat* indexed = SDL_AllocFormat(SDL_PIXELFORMAT_INDEX8);
		indexed->palette = m_SDLPalette.get();
		uint32_t index = SDL_MapRGBA(indexed, color.r * 255.f, color.g * 255.f, color.b * 255.f, color.a * 255.f);

		indexed->palette = nullptr;
		SDL_FreeFormat(indexed);
		return index;
	}

	void Palette::Bind() {
		glCheck(glActiveTexture(GL_TEXTURE1));
		glCheck(glBindTexture(GL_TEXTURE_1D, m_Texture));
	}

	void Palette::Update() {
		glCheck(glActiveTexture(GL_TEXTURE1));
		glCheck(glBindTexture(GL_TEXTURE_1D, m_Texture));
		glCheck(glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, PALETTESIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_Palette.data()));
		std::array<SDL_Color, PALETTESIZE> colors;
		for(size_t i = 0; i < PALETTESIZE; ++i) {
			colors[i].r = m_Palette[i].r;
			colors[i].g = m_Palette[i].g;
			colors[i].b = m_Palette[i].b;
			colors[i].a = 255;
		}
		SDL_SetPaletteColors(m_SDLPalette.get(), colors.data(), 0, PALETTESIZE);
	}

	void Palette::sdl_palette_deleter::operator()(SDL_Palette *p) {
		SDL_FreePalette(p);
	}

} // namespace RTE