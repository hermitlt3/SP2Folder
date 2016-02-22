#ifndef QUEST_H_
#define QUEST_H_
#include "QuestSystem.h"

class Quest
{
public:
	static Quest* GetInstance() {
		static Quest data;
		return &data;
	}
	void QuestInit();
	void QuestUpdate();
	int q_state = 0;
private:
	Quest(){};
};

#endif //QUEST_H_