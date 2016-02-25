#ifndef GAME_MODE_H
#define GAME_MODE_H


class GameMode {
public:
	static GameMode* GetInstance() {
		static GameMode data;
		return &data;
	}
	int gameState = 0;
private:
	GameMode(){};
};

enum UserInterface
{
	START = 0,
	OPTIONS,
	EXIT,
};

class c_UserInterface {
public:
	static c_UserInterface* GetEnum() {
		static c_UserInterface data;
		return &data;
	}
	
	UserInterface UI = START;
	bool quitGame = false;
private:
	c_UserInterface(){};
};

#endif //GAME_MODE_H

