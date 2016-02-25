#ifndef QUEST_SYSTEM_H
#define QUEST_SYSTEM_H
#include <vector>
#include "Quest.h"

using std::vector;

class QuestSystem
{
public:
	static QuestSystem* GetInstance() {
		static QuestSystem data;
		return &data;
	}
	vector<Quest>QuestList;
	vector<Quest>::iterator qt;

private:
	QuestSystem(){};
};

#endif //QUEST_SYSTEM_H