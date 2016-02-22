#ifndef GAME_STATES_H
#define GAME_STATES_H

enum GAMESTATES
{
	GAME_MENU = 0,
	GAME_PLAY,
};

class SharedData
{
public:
	SharedData* GetInstance() {
		static SharedData data;
		return &data;
	}
	GAMESTATES gameState;
private:
	SharedData();
};

SharedData gameSta;
#endif //GAME_STATES_H