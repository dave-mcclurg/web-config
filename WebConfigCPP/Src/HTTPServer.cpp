// WebConfig - Use a web browser to configure your application
// Copyright (c) 2009 David McClurg <dpm@efn.org>
// Under the MIT License, details: License.txt.

#include "Support/asynchat.h"
#include "Support/Convert.h"
#include "HTTPServer.h"
#include "Support/HTTPUtility.h"

#include <string>
#include <map>
#include <stdlib.h>

using namespace std;

namespace WebConfig
{
	enum RState
	{
		STATE_METHOD,
		STATE_URL,
		STATE_URLPARM,
		STATE_URLVALUE,
		STATE_VERSION, 
		STATE_HEADERKEY,
		STATE_HEADERVALUE,
		STATE_BODY,
		STATE_OK
	};

	class Channel : public async_sockets::async_chat
	{
		class HTTPServer* parent;
		string input_buffer;

	public:

		Channel(HTTPServer* p) : parent(p) {}
		void collect_incoming_data (const string& data)
		{
			input_buffer.append (data);
		}
		void found_terminator (void);
		void handle_close (void) {}
		void handle_request();
	};

	static void WriteLog(string msg)
	{
		cerr << msg << endl;
	}

	void HTTPServer::handle_accept (void)
	{
		struct sockaddr addr;
		int addr_len = sizeof(sockaddr);
		int fd = async_sockets::dispatcher::accept (&addr, &addr_len);
		Channel * jc = new Channel(this);
		jc->set_fileno (fd);
		jc->set_terminator ("\r\n\r\n");
	}

	/// <summary>
	/// Start the listener
	/// </summary>
	/// <param name="portNum">Port on which to listen</param>
	void HTTPServer::Start(int portNum)
	{
#ifdef _WIN32
		WSADATA wd;
		WSAStartup (MAKEWORD (1,1), &wd);
#endif

		cerr << "create: " << create_socket (AF_INET, SOCK_STREAM) << endl;

		struct sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons (portNum);
		addr.sin_addr.s_addr = htonl (INADDR_ANY);

		cerr << "bind: " << bind ((struct sockaddr *) &addr, sizeof (addr)) << endl;
		cerr << "listen: " << listen (5) << endl;
	}

	/// <summary>
	/// Stop the listener
	/// </summary>
	void HTTPServer::Stop()
	{
		//
	}

	void HTTPServer::Update()
	{
		if (channels.size())
		{
			/* Return immediately */
			struct timeval timeout;
			timeout.tv_sec = 0;
			timeout.tv_usec = 0;

			poll (&timeout);
		}
	}

	void Channel::found_terminator (void)
	{
		if (input_buffer.length())
		{
			input_buffer += get_terminator();
			handle_request();
		}
		close_when_done();
	}

	void Channel::handle_request()
	{
		HTTPRequestParams requestParams;
		HTTPResponse response;

		WriteLog("You received the following message : \n" + input_buffer);

		response.BodySize = 0;

		string hValue = "";
		string hKey = "";

		//Example request
		// HTTP GET /billing/servlet/comm.billing.GetBalance?Date=17:54:24&CustomerID=8057 HTTP/1.1

		int numberOfBytesRead = input_buffer.length();
		const string& myReadBuffer = input_buffer;

		// read buffer index
		RState parserState = STATE_METHOD;
		int ndx = 0;
		do
		{
			switch ( parserState )
			{
			case STATE_METHOD:
				if (myReadBuffer[ndx] != ' ')
					requestParams.Method += (char)myReadBuffer[ndx++];
				else 
				{
					ndx++;
					parserState = STATE_URL;
				}
				break;
			case STATE_URL:
				if (myReadBuffer[ndx] == '?')
				{
					ndx++;
					hKey = "";
					requestParams.Execute = true;
					requestParams.Args.clear();
					parserState = STATE_URLPARM;
				}
				else if (myReadBuffer[ndx] != ' ')
					requestParams.URL += (char)myReadBuffer[ndx++];
				else
				{
					ndx++;
					requestParams.URL = HttpUtility::UrlDecode(requestParams.URL);
					parserState = STATE_VERSION;
				}
				break;
			case STATE_URLPARM:
				if (myReadBuffer[ndx] == '=')
				{
					ndx++;
					hValue="";
					parserState = STATE_URLVALUE;
				}
				else if (myReadBuffer[ndx] == ' ')
				{
					ndx++;

					requestParams.URL = HttpUtility::UrlDecode(requestParams.URL);
					parserState = STATE_VERSION;
				}
				else
				{
					hKey += (char)myReadBuffer[ndx++];
				}
				break;
			case STATE_URLVALUE:
				if (myReadBuffer[ndx] == '&')
				{
					ndx++;
					hKey=HttpUtility::UrlDecode(hKey);
					hValue=HttpUtility::UrlDecode(hValue);
					if (requestParams.Args.find(hKey) != requestParams.Args.end())
						requestParams.Args[hKey] = requestParams.Args[hKey] + ", " + hValue;
					else
						requestParams.Args[hKey] = hValue;
					hKey="";
					parserState = STATE_URLPARM;
				}
				else if (myReadBuffer[ndx] == ' ')
				{
					ndx++;
					hKey=HttpUtility::UrlDecode(hKey);
					hValue=HttpUtility::UrlDecode(hValue);
					if (requestParams.Args.find(hKey) != requestParams.Args.end())
						requestParams.Args[hKey] = requestParams.Args[hKey] + ", " + hValue;
					else
						requestParams.Args[hKey] = hValue;

					requestParams.URL = HttpUtility::UrlDecode(requestParams.URL);
					parserState = STATE_VERSION;
				}
				else
				{
					hValue += (char)myReadBuffer[ndx++];
				}
				break;
			case STATE_VERSION:
				if (myReadBuffer[ndx] == '\r') 
					ndx++;
				else if (myReadBuffer[ndx] != '\n') 
					requestParams.Version += (char)myReadBuffer[ndx++];
				else 
				{
					ndx++;
					hKey = "";
					requestParams.Headers.clear();
					parserState = STATE_HEADERKEY;
				}
				break;
			case STATE_HEADERKEY:
				if (myReadBuffer[ndx] == '\r') 
					ndx++;
				else if (myReadBuffer[ndx] == '\n')
				{
					ndx++;
					if (requestParams.Headers.find("Content-Length") != requestParams.Headers.end())
					{
						requestParams.BodySize = Convert::ToInt(requestParams.Headers["Content-Length"]);
						requestParams.BodyData = "";
						//requestParams.BodyData = new byte[requestParams.BodySize];
						parserState = STATE_BODY;
					}
					else
						parserState = STATE_OK;

				}
				else if (myReadBuffer[ndx] == ':')
					ndx++;
				else if (myReadBuffer[ndx] != ' ')
					hKey += (char)myReadBuffer[ndx++];
				else 
				{
					ndx++;
					hValue = "";
					parserState = STATE_HEADERVALUE;
				}
				break;
			case STATE_HEADERVALUE:
				if (myReadBuffer[ndx] == '\r') 
					ndx++;
				else if (myReadBuffer[ndx] != '\n')
					hValue += (char)myReadBuffer[ndx++];
				else 
				{
					ndx++;
					requestParams.Headers[hKey] = hValue;
					hKey = "";
					parserState = STATE_HEADERKEY;
				}
				break;
			case STATE_BODY:
				// Append to request BodyData
				requestParams.BodyData = myReadBuffer.substr(ndx);
				ndx = numberOfBytesRead;
				if (requestParams.BodyData.length() >= (unsigned int)requestParams.BodySize)
				{
					parserState = STATE_OK;
				}
				break;
				//default:
				//	ndx++;
				//	break;

			}
		}
		while(ndx < numberOfBytesRead);

		// Read body?
		if (parserState == STATE_BODY)
		{
			int needed = requestParams.BodySize - requestParams.BodyData.length();

			char buffer[512];
			while (needed > 0)
			{
				int requested = (needed > 512)? 512: needed;
				int result = recv (buffer, requested);
				if (result > 0)
				{
					requestParams.BodyData.append(buffer, result);
					needed -= result;
				}
				else
					break;
			}

			if (requestParams.BodyData.length() >= (unsigned int)requestParams.BodySize)
			{
				parserState = STATE_OK;
			}
		}

		response.Version = "HTTP/1.1";

		string StatusString;
		if (parserState != STATE_OK)
		{
			response.Status = (int)RESPONSE_BAD_REQUEST;
			StatusString = "400 Bad Request";
		}
		else
		{
			response.Status = (int)RESPONSE_OK;
			StatusString = "200 Ok";
		}

		response.Headers.clear();
		response.Headers["Server"] = "HTTPServer/1.0.*";

		//response.Headers["Date"] = DateTime.Now.ToString("r");
		if (requestParams.Headers.find("Date") != requestParams.Headers.end())
		{
			response.Headers["Date"] = requestParams.Headers["Date"];
		}

		if (response.Status == (int)RESPONSE_OK)
		{
			parent->OnResponse(requestParams, response);
		}

		string HeadersString = response.Version + " " + StatusString + "\n";

		for (map<string,string>::iterator i = response.Headers.begin(); i != response.Headers.end(); ++i) 
		{
			HeadersString += (*i).first + ": " + (*i).second + "\n";
		}

		HeadersString += "\n";
		//HeadersString = Encoding.ASCII.GetBytes(HeadersString);

		// Send headers	
		send(HeadersString);

		// Send body
		if (response.BodyData.length() > 0)
			send(response.BodyData);

		if (response.fs.is_open())
		{
			char buffer[512];
			while (!response.fs.eof())
			{
				response.fs.read(buffer, sizeof(buffer));
				int bytesRead = response.fs.gcount();
				send(string(buffer, bytesRead));
			}
			response.fs.close();
		}
	}
}