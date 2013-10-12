#pragma once

struct _Player{
	double MaxHP;
	double MaxMP;
	
	double HP;
	double MP;

	double getHP_Percent();
	double getMP_Percent();
};

extern _Player Player;