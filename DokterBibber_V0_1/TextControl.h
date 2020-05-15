#pragma once
#include "stb_truetype.h"
#include <GL\glew.h>
#include <string>

class TextControl
{
public:
	TextControl(const std::string fontPath, const int bufferSize, const int width, const int heigth);
	void drawText(const std::string text, float xPos, float yPos);
	void setScale(float scaling);

private:
	GLuint textureId; // texture/font id 
	stbtt_bakedchar bakedData[96]; // data where font glyphs are saved
	float orthoScaler = 0.1; // scaler of the text
	float scalerValue = 0.085 / orthoScaler; // necessary scaling value to make sure the text is positioned correctly
	float width, heigth; // width & heigth of the screen
};

