#pragma once

#include <iostream>
#include <fstream>
#include <list>
#define MAX 1000 

using namespace std;

enum enum_location { abs1, front, back, traps, none };

string LocationName(enum_location loc) {
	switch (loc) {
		case  abs1:
			return "abs";
		case  front:
			return "front";
		case  back:
			return "back";
		case  traps:
			return "traps";
		default:
			return "error";
		break;
	}
}

//=======================================Stimulation========================================================
class Stimulation {
protected:
	string name;
public:
	Stimulation() {
		name.clear();
	};

	Stimulation(const char n) {
		name = n;
	};

	string getname() const {
		return name;
	}

	virtual void display(std::ostream&) const = 0;
};

//=======================================Stims========================================================

class Stims : public Stimulation
{
	string Stims_name;
	enum_location location;
	int Stims_intensity;
	int Stims_frequency;
	int Stims_duration;

public:

	Stims() {
		Stims_name.clear();
		location = none;
		Stims_intensity = 0;
		Stims_frequency = 0;
		Stims_duration = 0;
	};

	Stims(const string n, const enum_location loc, const int intens, const int freq, const int dur)
	{
		name = n;
		location = loc;
		Stims_intensity = intens;
		Stims_frequency = freq;
		Stims_duration = dur;
	};

	virtual void display(ostream& os) const
	{
		os << name.c_str() << endl;
		os << "     Type = Stims " << endl;
		os << "     Location = " << LocationName(location).c_str() << endl;
		os << "     Intensity = " << Stims_intensity << endl;
		os << "     Frequency = " << Stims_frequency << endl;
		os << "     Duration = " << Stims_duration << endl;
	};
};

//=======================================Exoskeleton========================================================

class Exoskeleton : public Stimulation
{
		string Exoskeleton_name;
		int Exoskeleton_intensity;
		int Exoskeleton_duration;

	public:
		Exoskeleton() {
			Exoskeleton_name.clear();
			Exoskeleton_intensity = 0;
			Exoskeleton_duration = 0;
		};

	Exoskeleton(const string n, const int intens, const int dur)
	{
		name = n;
		Exoskeleton_intensity = intens;
		Exoskeleton_duration = dur;
	};

	virtual void display(std::ostream& os) const
	{
		os << name.c_str() << endl;
		os << "     Type = Exosceleton " << endl;
		os << "     Intensity = " << Exoskeleton_intensity << endl;
		os << "     Duration = " << Exoskeleton_duration << endl;
	};

};

//=======================================Task========================================================
class Task {
	string task_name;
	list<Stimulation*> list_of_Stimulation;
public:

	list<Stimulation*> getlist() const {
		return list_of_Stimulation;
	}

	string getname() {
		return task_name;
	}

	Task() {
		task_name.clear();
		list_of_Stimulation.empty();
	}

	Task(string task) {
		task_name = task;
	}

	Task(Task&& m_task) {
		*this = move(m_task);
	};

	Task& operator=(Task&& t) {
		*this = t;
		t.task_name.clear();
		return *this;
	}

	Task(const Task& c_task) {
		*this = c_task;
	};

	Task& operator=(const Task& t) {
		list_of_Stimulation = list<Stimulation*>(t.getlist());
		task_name = t.task_name;
		return *this;
	};


	Task& operator+=(Stimulation* stim) {
		list_of_Stimulation.push_back(stim);
		return *this;
	}

	bool removeStimulation(string stim_name) {
		for (list<Stimulation*>::iterator it = list_of_Stimulation.begin(); it != list_of_Stimulation.end(); ++it) {
			Stimulation* pStim = *it;
			if (!stim_name.compare(pStim->getname())) {
				list_of_Stimulation.erase(it);
				return true;
			}
		}
		return false;
	}

	Stimulation * operator[](size_t i) {
		int temp = 0;
		Stimulation* pStimReq = NULL;
		for (list<Stimulation*>::iterator it = list_of_Stimulation.begin(); it != list_of_Stimulation.end(); ++it) {
			temp++;
			if (temp == i) {
				return *it;
			}
		}
	}


	void execute(ostream& os) {
		os << "Task Name " << task_name.c_str() << endl;

		for (list<Stimulation*>::iterator it = list_of_Stimulation.begin(); it != list_of_Stimulation.end(); ++it) {
			Stimulation* Stim = *it;
			Stim->display(os);
		}
	}

	ostream& dump(ostream& os) const {
		os << task_name.c_str() << endl;
		return os;
	}
};


//=======================================ARAIG_Sensors========================================================
class ARAIG_Sensors {
private:
	list<Stimulation*> list_of_stim;
	list<Task*> list_of_task;
public:

	ARAIG_Sensors(){}

	Task* getTaskByName(std::string name);

	ARAIG_Sensors(string StimulationConfig, string TaskConfiguration) {

		ifstream fileStim(StimulationConfig.c_str());
		if (fileStim.is_open()) {
			readStimulation(fileStim);
			fileStim.close();
		}


		ifstream fileTask(TaskConfiguration.c_str());
		if (fileTask.is_open()) {
			readTask(fileTask);
			fileTask.close();
		}
	};

	~ARAIG_Sensors() {
		for (list<Stimulation*>::iterator it_stim = list_of_stim.begin(); it_stim != list_of_stim.end(); ++it_stim) {
			Stimulation* Stim = *it_stim;
			delete Stim;
		}

		for (list<Task*>::iterator it_task = list_of_task.begin(); it_task != list_of_task.end(); ++it_task) {
			Task* Task = *it_task;
			delete Task;
		}
	};

	void readStimulation(ifstream& file);
	void readTask(ifstream& file);

	ostream& dump(ostream& os) {
		for (list<Task*>::iterator it = list_of_task.begin(); it != list_of_task.end(); ++it) {
			Task* Task = *it;
			if (Task) {
				Task->execute(os);
			}
		}
		return os;
	}
};


//===================================Profile============================================

class Profile {
private:
	ostream& sos;
	ARAIG_Sensors* Sensors;

	vector<Task> ToRun;
	vector<Task> Completed;

	string student_fn;
	string student_ln;
	string student_number;
	string instructor_fn;
	string instructor_ln;
	string instructor_number;

	struct Calibration {
		int Max;
		int Min;
	};

	Calibration cal;

public:

	Profile(string name_of_file, ostream & os, ARAIG_Sensors& sensors) : sos(os){

		Sensors = &sensors;
		int Max = 0;
		int Min = 0;

		ifstream file(name_of_file.c_str());
		if (file.is_open()) {
			readProfile(file);
		}
		file.close();
	}

	~Profile(){};

	void readProfile(ifstream& file) {
		char temp[MAX + 1];
		char const* delimeters = ",";
		char * context = NULL;

		char const* token;

		if (file.is_open()) {
			// Student
			file.getline(temp, MAX);
			if (temp[0] != 0)
			{
				token = strtok_s(temp, delimeters, &context);
				if (token) {
					student_fn = token;
					token = strtok_s(NULL, delimeters, &context);
					student_ln = token;
					token = strtok_s(NULL, delimeters, &context);
					student_number = token;
				}
			}

			// Instructor
			file.getline(temp, MAX);
			if (temp[0] != 0)
			{
				token = strtok_s(temp, delimeters, &context);
				if (token)
				{
					instructor_fn = token;
					token = strtok_s(NULL, delimeters, &context);
					instructor_ln = token;
					token = strtok_s(NULL, delimeters, &context);
					instructor_number = token;
				}
			}

			// Max, Min
			file.getline(temp, MAX);
			if (temp[0] != 0) {
				token = strtok_s(temp, delimeters, &context);
				if (token) {
					cal.Max = atoi(token);
					token = strtok_s(0, delimeters, &context);
					cal.Min = atoi(token);
				}
			}

			// Task
			string name_of_task;
			while (!file.eof())
			{
				file.getline(temp, MAX);
				if (temp[0] != 0)
				{
					name_of_task = temp;
					Task* Task = Sensors->getTaskByName(name_of_task);
					if (Task)
						ToRun.push_back(*Task);
				}
			}
		}
	}
	
	void getInfo() {
		cout << "Chloe,Coleshill,123456789 Eliott, Coleshill, 987654321" << endl;
	}

	//show task name
	void getFirstToRun() {
		if (ToRun.size() > 0) {
			Task Task = ToRun.front();
			cout << Task.getname() << endl;
		} else {
			cout << "To Run is empty" << endl;
		}
	}

	//show the last task which was completed
	void getLastCompleted(){
		if (Completed.size() > 0) {
			Task Task = Completed.back();
			cout << Task.getname() << endl;
		} else {
			cout << "Completed is empty" << endl;
		}
	}

	//run the task
	void run(){
		if (ToRun.size() > 0)
		{
			Task Task = ToRun.front();
			Task.execute(sos);
			ToRun.erase(ToRun.begin());
			Completed.push_back(Task);

		} else {
			cout << "ToRun is empty" << endl;
		}
	}

	//show all tasks
	void ShowAllToRun() {
		for (vector<Task>::iterator it = ToRun.begin(); it != ToRun.end(); ++it) {
			Task Task = *it;
			cout << Task.getname() << endl;

		}
	}

	//show all task which we run
	void ShowAllCompleted() {
		for (vector<Task>::iterator it = Completed.begin(); it != Completed.end(); ++it) {
			Task Task = *it;
			cout << Task.getname().c_str() << endl;
		}
	}
};