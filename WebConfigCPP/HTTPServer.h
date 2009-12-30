#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "asynchat.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>

namespace WebConfig
{
	enum HTTPResponseStatus
	{
		RESPONSE_OK = 200, 
		RESPONSE_BAD_REQUEST = 400,
		RESPONSE_NOT_FOUND = 404
	};

	typedef std::map<std::string,std::string> Hashtable;

	class HTTPResponse
	{
	public:
		int Status;
		std::string Version;
		Hashtable Headers;
		int BodySize;
		std::string BodyData;
		std::ifstream fs;

		HTTPResponse() {}
	};

	class HTTPRequestParams
	{
	public:
		std::string Method;
		std::string URL;
		std::string Version;
		Hashtable Args;
		bool Execute;
		Hashtable Headers;
		int BodySize;
		std::string BodyData;

		HTTPRequestParams() {}
	};

	/// <summary>
	/// Embedded HTTP server
	/// </summary>
	class HTTPServer : public dispatcher
	{
	public:

		typedef void (*Callback)(const HTTPRequestParams& rq, HTTPResponse& rp);
		Callback OnResponse;

		/// <summary>
		/// Constructor
		/// </summary>
		/// <param name="responseHandler">method to handle HTTP requests</param>
		HTTPServer(Callback OnResponse)
		{
			this->OnResponse = OnResponse;
		}

		void Start(int portNum);
		void Stop();
		void Update();

		void handle_accept (void);
	};
}

#endif // #ifndef HTTPSERVER_H
