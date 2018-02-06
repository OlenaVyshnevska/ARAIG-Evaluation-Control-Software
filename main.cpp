#define _CRT_SECURE_NO_WARNINGS

#include <sstream>
#include <cstring>
#include <string>
#include <string.h>
#include <vector>
#include <list>
#include <fstream> 
#include "Stimulation.h"


using namespace std;

void ARAIG_Sensors::readStimulation(ifstream &file) {
	if (file.is_open()) {
		while (!file.eof()) {
			Stimulation* rc = nullptr;
			char temp[20];
			file.getline(temp, 20, ',');

			if (strcmp(temp, "stim") == 0) {
				char name[20], location[20], d;
				int inten, freq, dur;
				file.getline(name, 20, ',');
				file.getline(location, 20, ',');
				file >> inten >> d >> freq >> d >> dur;

				enum_location l;
				if (strcmp(location, "abs") == 0) {
					l = abs1;
				}
				else if (strcmp(location, "front") == 0) {
					l = front;
				}
				else if (strcmp(location, "back") == 0) {
					l = back;
				}
				else if (strcmp(location, "traps") == 0) {
					l = traps;
				}

				rc = new Stims(name, l, inten, freq, dur);
			}
			else if (strcmp(temp, "exoskeleton") == 0) {
				char name[20], d;
				int inten, dur;
				file.getline(name, 50, ',');
				file >> inten >> d >> dur;
				rc = new Exoskeleton(name, inten, dur);
			}
			if (rc != 0) {
				list_of_stim.push_back(rc);
			}
			char a = file.get();
		}
	}
}


void ARAIG_Sensors::readTask(ifstream& file) {
	if (file.is_open()) {
		Task* sTask = nullptr;

		bool bEndTask = false;
		while (!file.eof()) {
			string line;
			getline(file, line);
			size_t f = line.find_first_of(",");
			if (f != string::npos){
				if (bEndTask) {
					list_of_task.push_back(sTask);
				}
				line = line.substr(f + 1);
				sTask = new Task(line);
				bEndTask = true;

			} else {
				for (list<Stimulation *>::iterator j = list_of_stim.begin(); j != list_of_stim.end(); j++)
				{
					if ((*j)->getname() == line)
					{
						*sTask += *j;
					}
				}
			}
		}
		list_of_task.push_back(sTask);

	}

}

Task* ARAIG_Sensors::getTaskByName(string name) {

	Task* sTask = NULL;
	for (list<Task*>::iterator it = list_of_task.begin(); it != list_of_task.end(); ++it) {
		Task* temp = *it;
		if (!name.compare(temp->getname())) {
			sTask = temp;
			break;
		}
	}
	return sTask;
}




//=======================================Main============================================================

int main(int argc, char* argv[])
{
	string ProfileConfig;
	string StimulationConfig;
	string TaskConfiguration;
	string Output;

	 
	StimulationConfig = argv[1];//"StimulationConfig.csv";
	TaskConfiguration = argv[2]; //"TaskConfiguration.csv";
	ProfileConfig = argv[3];//"SampleProfileConfiguration.csv";
	Output = argv[4];//"result.txt";

	ARAIG_Sensors Sensors(StimulationConfig, TaskConfiguration);
	ofstream out(Output);
	Profile MyProfile(ProfileConfig, out, Sensors);

	while (1) {


		cout << "1 -  Run All Tasks" << ", 2 - All Completed Tasks" << ", 3 -  Run Next Task" << ", 4 - Last Comleted Task" << ", 5 - Run" << ", 6 - exit" << endl;

		string select;

		int Select;
		cin >> select;
		Select = atoi(select.c_str());
		switch (Select) {
			case 1:
				cout << "Run All Tasks" << endl;
				MyProfile.ShowAllToRun();
				break;
			case 2:
				cout << "All Completed Tasks" << endl;
				MyProfile.ShowAllCompleted();
				break;
			case 3:
				cout << "Run Next Task" << endl;
				MyProfile.getFirstToRun();
				break;
			case 4:
				cout << "Last Comleted Task" << endl;
				MyProfile.getLastCompleted();
				break;
			case 5:
				cout << "Run" << endl;
				MyProfile.run();
				break;
			case 6:
				cout << "exit" << endl;
				break;
			}
			if (Select == 6)
				break;
		}

	return 0;
}