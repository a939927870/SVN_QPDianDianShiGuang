#include "GameDefine.h"

Lstring getGameTypeStr(Lint n)
{
	switch (n)
	{
	case 0:
		return "game_para_maxfan";
	break;

	case 1:
		return "game_para_diOrFan";
	break;
	case 2:
		return "game_para_cls3sub";
	break;
	}

	return "";
}

Lint getGameUserCounts(Lint nGameType)
{
	switch (nGameType)
	{
	case game_ddz_classical:
	case game_ddz_sichuan:
	case game_ddz_3lai:
	case game_wakeng_shanxi:
		return 3;
	case game_ddz_2players:
		return 2;
	case game_ddz_sichuan_pz:
		return 4;
	}
	return 3;
}