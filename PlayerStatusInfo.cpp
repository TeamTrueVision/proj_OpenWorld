#include "PlayerStatusInfo.h"

_Player Player;

double _Player::getHP_Percent()
{
	return HP / MaxHP * 100;
}

double _Player::getMP_Percent()
{
	return MP / MaxMP * 100;
}

