#pragma once

#include "pch.h"
#include "Properties.h"

//Reserve 100 to 200 for Canvas core systems
#define CVS_GL 100
#define CVS_DX 101


namespace Canvas{
	typedef unsigned int uint32;

	/*Core Canvas engine components*/

	//Message response
	struct Response{
		int error;
		long data;
	};

	//Base messaging class
	class Entity{
	public:
		virtual Response Message(unsigned int msg, void* pointer, long data) = 0;
	};

	//Time storage struct
	struct Time{
		uint32 year;
		uint32 month;
		uint32 day_week;
		uint32 hour;
		uint32 minute;
		uint32 seconds;
		uint32 milliseconds;
		std::string toString();
	};

	//Timer for time.
	class Timer{
#ifdef CVS_WIN32
		static int frequency;
		LARGE_INTEGER counter;
#endif
		int m_lasttime;
		int m_deltatime;
		int m_stopwatchtime;
	public:
		//Define current time.

		//High frequency clock timer, used for core
		//engine systems.
		Timer();
		void setFrame();
		//Get delta time in seconds from last two setFrame calls.
		float getDeltaTimeS();
		//Get delta time in milliseconds from last two setFrame calls.
		float getDeltaTimeMS();
		float getDeltaTimeMC();
		void StartStopWatch();
		int EndStopWatch();
		//Gets current OS time.
		Time getSystemTime();
	};

	//Canvas core subsystems.
	class SubSystem:public Entity{
		int id;
	public:
		std::string name;
		SubSystem();
		SubSystem(std::string);
		virtual bool init(uint32) = 0;
		virtual void update() = 0;
		virtual bool end() = 0;
	};
	
	//Simple transcript log object
	struct Transcript{
	private:
		std::string data;
	public:
		unsigned int getLines();
		void print();
		char* getString();
		void BeginTranscript(std::string header,std::string name);
		void addPushData(std::string string);
		void EndTranscript();
	};

	bool Initialize(uint32);
	void Run();
	bool End();
	Time GetOSTime();
	
	class RegisterFactory{
		std::map<std::string, SubSystem*> m_systems;
	};

	//Canvas Main StateMachine
	struct StateMachine{
		std::map< std::string, SubSystem*> m_systems;
		std::vector<SubSystem*> m_update_sub;
		void Update();
		bool Init(uint32);
		void Run();
		bool End();
		void getSubSystem(std::string);
		void addSubSystem(std::string, SubSystem*);
	};

	template <typename T>
	void RegisterSubSystem(SubSystem*);


};

extern Canvas::StateMachine CVS_Server;