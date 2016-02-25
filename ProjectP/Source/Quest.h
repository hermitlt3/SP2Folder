#ifndef QUEST_H_
#define QUEST_H_
#include <string>

using std::string;

class MainQuest
{
public:
	static MainQuest* GetInstance() {
		static MainQuest data;
		return &data;
	}
	int quest_complete = 0;
	bool b_questcomplete = false;
private:
	MainQuest(){};
};

class Quest
{
public:
	Quest();
	void setName(string name);
	bool b_Complete(bool b_complete);
	enum QUEST_STATE
	{
		ONE,
		TWO,
		THREE,
		FOUR
	};
	QUEST_STATE q_state;
	string getName();

private:
	string name_;
};
#endif //QUEST_H_