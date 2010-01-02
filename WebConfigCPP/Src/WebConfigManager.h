// WebConfig - Use a web browser to configure your application
// Copyright (c) 2009 David McClurg <dpm@efn.org>
// Under the MIT License, details: License.txt.

#ifndef WEBCONFIGMANAGER_H
#define WEBCONFIGMANAGER_H

#include <string>
#include <assert.h>

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

		Manager(const Manager&)
		{
			assert(false);
		}

		Manager& operator=(const Manager&)
		{
			assert(false);
			return *this;
		}

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
