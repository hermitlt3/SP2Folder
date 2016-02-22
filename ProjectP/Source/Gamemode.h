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

#endif //GAME_MODE_H

