#include "Quest.h"

Quest::Quest()
{

}

void Quest::setName(string name)
{
	name_ = name;
}

string Quest::getName()
{
	return name_;
}

bool Quest::b_Complete(bool b_complete)
{
	return b_complete;
}