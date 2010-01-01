#ifndef WEBCONFIGMANAGER_H
#define WEBCONFIGMANAGER_H

#include <string>

namespace WebConfig
{
	class InputBase;
	class ManagerImpl;
	class FormSettings;

	class Manager
	{
		ManagerImpl* pImpl;

		/// <summary>
		/// private constructor
		/// </summary>
        Manager();

	public:

		/// <summary>
		/// Get the singleton
		/// </summary>
        static Manager& Instance()
		{
	        static Manager instance;
			return instance;
		}

		/// <summary>
		/// Startup the server
		/// </summary>
		void Startup(int thePort, std::string theFolder);

		/// <summary>
		/// Shutdown the server
		/// </summary>
		void Shutdown();

		/// <summary>
		/// Update the manager; should be called periodically
		/// </summary>
		void Update();

		/// <summary>
		/// Get form settings from name
		/// </summary>
		FormSettings* GetFormSettings(std::string formName);

		/// <summary>
		/// Add an input to the dictionary
		/// </summary>
		void AddInput(InputBase* input);

		/// <summary>
		/// Remove an input from the dictionary
		/// </summary>
		void RemoveInput(InputBase* input);
	};
}

#endif // #ifndef WEBCONFIGMANAGER_H
