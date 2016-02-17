#ifndef SHARED_DATA_H
#define SHARED_DATA_H



class SharedData {
public:
	static SharedData* GetInstance() {
		static SharedData data;
		return &data;
	}
	int gameState = 0;
private:
	SharedData(){};
};

#endif //SHARED_DATA_H
