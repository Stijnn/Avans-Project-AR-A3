#define STB_TRUETYPE_IMPLEMENTATION  
#include "TextControl.h"
#include "tigl.h"
#include <glm/gtc/matrix_transform.hpp>

using tigl::Vertex;

/**
	To initialize; create a TextControl variable and set the scale in the following manner:

	textWriter = new TextControl("C:/Windows/Fonts/times.ttf", 20, 1920.0f, 1080.0f); 
	textWriter->setScale(0.9); // The standard value is 0.1, so this step could be unnecessary.
	 |
	  --> Can be anything above 0, but be aware, low levels will cause problems with the positioning

	  @params fontPath = The filepath of the font you wish to chose
	  @params bufferSize = Temporary size to load the glyphs from the font file
	  @params width = the screen width
	  @params heigth = the screen heigth
*/
TextControl::TextControl(const std::string fontPath, const int bufferSize, const int width, const int heigth)
{
	this->width = width;
	this->heigth = heigth;

	FILE* file;

	file = fopen(fontPath.c_str(), "rb");

	unsigned char* ttf_buffer = new unsigned char[1 << bufferSize];
	unsigned char* temp_bitmap = new unsigned char[512 * 512];
	fread(ttf_buffer, 1, 1 << bufferSize, file);
	stbtt_BakeFontBitmap(ttf_buffer, 0, 32.0, temp_bitmap, 512, 512, 32, 96, bakedData);
	glGenTextures(1, &this->textureId);
	glBindTexture(GL_TEXTURE_2D, this->textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	delete[] ttf_buffer;
	delete[] temp_bitmap;
	fclose(file);
}

/**
	Draw method to draw text on the screen, This method doesn't support multiline printing
	For a new line lower the yPos
	At the end of the method resets all the necessary values for perspective view draw 

	  @params text = The text you wish to print
	  @params xPos = the X-Axis position
	  @params yPos = the Y-Axis position
*/
void TextControl::drawText(const std::string text, float xPos, float yPos)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport); // inits the viewport

	tigl::shader->setProjectionMatrix(glm::ortho( (-width / this->orthoScaler), (width / this->orthoScaler), // width (scale) of the projection 
		(heigth / this->orthoScaler), (-heigth / this->orthoScaler), // height (scale) of the projection
		-1.0f, 110.0f)); // nearView and farView (the amount it needs to render)

	tigl::shader->setViewMatrix(glm::mat4(1.0f));
	tigl::shader->setModelMatrix(glm::mat4(1.0f));

	tigl::shader->enableColor(false);
	tigl::shader->enableTexture(true);
	tigl::shader->enableColorMult(false);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glBindTexture(GL_TEXTURE_2D, this->textureId);

	float x = (xPos * scalerValue), y = (yPos * scalerValue);
	stbtt_aligned_quad q;

	tigl::begin(GL_QUADS);
	for (int i = 0; i < text.size(); i++) {
		if (text[i] >= 32 && text[i] < 128) {
			stbtt_GetBakedQuad(this->bakedData, 512, 512, text[i] - 32, &x, &y, &q, 1);
			tigl::addVertex(Vertex::PT(glm::vec3(q.x0, q.y0, 0), glm::vec2(q.s0, q.t0)));
			tigl::addVertex(Vertex::PT(glm::vec3(q.x1, q.y0, 0), glm::vec2(q.s1, q.t0)));
			tigl::addVertex(Vertex::PT(glm::vec3(q.x1, q.y1, 0), glm::vec2(q.s1, q.t1)));
			tigl::addVertex(Vertex::PT(glm::vec3(q.x0, q.y1, 0), glm::vec2(q.s0, q.t1)));
		}
	}
	tigl::end();

	tigl::shader->enableColor(true);
	tigl::shader->enableTexture(false);
	tigl::shader->enableColorMult(true);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

/**
	Set the scaling of the text, can be anything from 0 or higher
	@params scaling = the scale of the text
*/
void TextControl::setScale(float scaling)
{
	this->orthoScaler = scaling;
}
