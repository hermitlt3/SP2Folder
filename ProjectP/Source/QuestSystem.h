#ifndef QUEST_SYSTEM_H
#define QUEST_SYSTEM_H
#include <list>
using std::list;

class QuestSystem
{
public:
	static QuestSystem* GetInstance() {
		static QuestSystem data;
		return &data;
	}
	//list<Quest>QuestList;
private:
	QuestSystem(){};
};

#endif //QUEST_SYSTEM_H