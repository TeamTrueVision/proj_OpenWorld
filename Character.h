#pragma once

#include "Renderer.h"

class openCharacterRender {
private:
	openImageRender ImageRender;

	struct {
		unsigned int Head;
		unsigned int Body;
		unsigned int Hand;
		unsigned int Leg;
		unsigned int Foot;
		unsigned int Weapon;
	} CharacterProperty;

	struct {
		unsigned int Head;
		unsigned int Hand;
		unsigned int Eye;
		unsigned int Ear;
		unsigned int Mouse;
		unsigned int Nose;
	} CharacterType;

public:
	openCharacterRender();
	~openCharacterRender();

	void setDirection(bool isRight);
	void setMoving(bool);
};