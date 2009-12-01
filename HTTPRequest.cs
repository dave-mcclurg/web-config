// HTTPServer
//
// rmortega77@yahoo.es
// The use of this software is subject to the following agreement
//
// 1. Don't use it to kill.
// 2. Don't use to lie.
// 3. If you learned something give it back.
// 4. If you make money with it, consider sharing with the author.
// 5. If you do not complies with 1 to 5, you may not use this software.
//
// If you have money to spare, and found useful, or funny, or anything 
// worth on this software, and want to contribute with future free 
// software development.
// You may contact the author at rmortega77@yahoo.es 
// Contributions can be from money to hardware spareparts (better), or 
// a bug fix (best), or printed bibliografy, or thanks... 
// just write me.

using System;
using System.Net.Sockets;
using System.Text;
using System.Collections;
using System.Globalization;
using System.Web;
using System.Diagnostics;

namespace WebConfig
{
	enum RState
	{
		METHOD, URL, URLPARM, URLVALUE, VERSION, 
		HEADERKEY, HEADERVALUE, BODY, OK
	};

	enum HTTPResponseStatus
	{
		OK = 200, 
		BAD_REQUEST = 400,
		NOT_FOUND = 404
	}

	public struct HTTPResponse
	{
		public int Status;
		public string Version;
		public Hashtable Headers;
		public int BodySize;
		public byte[] BodyData;
		public System.IO.FileStream fs;
	}

    public struct HTTPRequestParams
    {
        public string Method;
        public string URL;
        public string Version;
        public Hashtable Args;
        public bool Execute;
        public Hashtable Headers;
        public int BodySize;
        public byte[] BodyData;
    }

    /// <summary>
	/// Summary description for HTTPRequest.
	/// </summary>
	public class HTTPRequest
	{
		private TcpClient client;

		private RState ParserState;

		private HTTPRequestParams HTTPRequestParams;

		private HTTPResponse HTTPResponse;

		byte[] myReadBuffer;

		HTTPServer Parent;

		public HTTPRequest(TcpClient client, HTTPServer Parent) 
		{
			this.client = client;
			this.Parent = Parent;

			this.HTTPResponse.BodySize = 0;
		}

        public void WriteLog(string EventMessage)
        {
            Debug.WriteLine(EventMessage);
        }

        public void Process()
		{
			myReadBuffer = new byte[client.ReceiveBufferSize];
			String myCompleteMessage = "";
			int numberOfBytesRead = 0;

			WriteLog("Connection accepted. Buffer: " + client.ReceiveBufferSize.ToString());
			NetworkStream ns = client.GetStream();

			string hValue = "";
			string hKey = "";

			try 
			{
				// binary data buffer index
				int bfndx = 0;

                //Example request
                // HTTP GET /billing/servlet/comm.billing.GetBalance?Date=17:54:24&CustomerID=8057 HTTP/1.1

				// Incoming message may be larger than the buffer size.
				do
				{
					numberOfBytesRead = ns.Read(myReadBuffer, 0, myReadBuffer.Length);  
					myCompleteMessage = 
						String.Concat(myCompleteMessage, Encoding.ASCII.GetString(myReadBuffer, 0, numberOfBytesRead));  
					
					// read buffer index
					int ndx = 0;
					do
					{
						switch ( ParserState )
						{
							case RState.METHOD:
								if (myReadBuffer[ndx] != ' ')
									HTTPRequestParams.Method += (char)myReadBuffer[ndx++];
								else 
								{
									ndx++;
									ParserState = RState.URL;
								}
								break;
							case RState.URL:
								if (myReadBuffer[ndx] == '?')
								{
									ndx++;
									hKey = "";
									HTTPRequestParams.Execute = true;
									HTTPRequestParams.Args = new Hashtable();
									ParserState = RState.URLPARM;
								}
								else if (myReadBuffer[ndx] != ' ')
									HTTPRequestParams.URL += (char)myReadBuffer[ndx++];
								else
								{
									ndx++;
									HTTPRequestParams.URL = HttpUtility.UrlDecode(HTTPRequestParams.URL);
									ParserState = RState.VERSION;
								}
								break;
							case RState.URLPARM:
								if (myReadBuffer[ndx] == '=')
								{
									ndx++;
									hValue="";
									ParserState = RState.URLVALUE;
								}
								else if (myReadBuffer[ndx] == ' ')
								{
									ndx++;

									HTTPRequestParams.URL = HttpUtility.UrlDecode(HTTPRequestParams.URL);
									ParserState = RState.VERSION;
								}
								else
								{
									hKey += (char)myReadBuffer[ndx++];
								}
								break;
							case RState.URLVALUE:
								if (myReadBuffer[ndx] == '&')
								{
									ndx++;
									hKey=HttpUtility.UrlDecode(hKey);
									hValue=HttpUtility.UrlDecode(hValue);
									HTTPRequestParams.Args[hKey] =  HTTPRequestParams.Args[hKey] != null ? HTTPRequestParams.Args[hKey] + ", " + hValue : hValue;
									hKey="";
									ParserState = RState.URLPARM;
								}
								else if (myReadBuffer[ndx] == ' ')
								{
									ndx++;
									hKey=HttpUtility.UrlDecode(hKey);
									hValue=HttpUtility.UrlDecode(hValue);
									HTTPRequestParams.Args[hKey] =  HTTPRequestParams.Args[hKey] != null ? HTTPRequestParams.Args[hKey] + ", " + hValue : hValue;
									
									HTTPRequestParams.URL = HttpUtility.UrlDecode(HTTPRequestParams.URL);
									ParserState = RState.VERSION;
								}
								else
								{
									hValue += (char)myReadBuffer[ndx++];
								}
								break;
							case RState.VERSION:
								if (myReadBuffer[ndx] == '\r') 
									ndx++;
								else if (myReadBuffer[ndx] != '\n') 
									HTTPRequestParams.Version += (char)myReadBuffer[ndx++];
								else 
								{
									ndx++;
									hKey = "";
									HTTPRequestParams.Headers = new Hashtable();
									ParserState = RState.HEADERKEY;
								}
								break;
							case RState.HEADERKEY:
								if (myReadBuffer[ndx] == '\r') 
									ndx++;
								else if (myReadBuffer[ndx] == '\n')
								{
									ndx++;
									if (HTTPRequestParams.Headers["Content-Length"] != null)
									{
										HTTPRequestParams.BodySize = Convert.ToInt32(HTTPRequestParams.Headers["Content-Length"]);
										this.HTTPRequestParams.BodyData = new byte[this.HTTPRequestParams.BodySize];
										ParserState = RState.BODY;
									}
									else
										ParserState = RState.OK;
									
								}
								else if (myReadBuffer[ndx] == ':')
									ndx++;
								else if (myReadBuffer[ndx] != ' ')
									hKey += (char)myReadBuffer[ndx++];
								else 
								{
									ndx++;
									hValue = "";
									ParserState = RState.HEADERVALUE;
								}
								break;
							case RState.HEADERVALUE:
								if (myReadBuffer[ndx] == '\r') 
									ndx++;
								else if (myReadBuffer[ndx] != '\n')
									hValue += (char)myReadBuffer[ndx++];
								else 
								{
									ndx++;
									HTTPRequestParams.Headers.Add(hKey, hValue);
									hKey = "";
									ParserState = RState.HEADERKEY;
								}
								break;
							case RState.BODY:
								// Append to request BodyData
								Array.Copy(myReadBuffer, ndx, this.HTTPRequestParams.BodyData, bfndx, numberOfBytesRead - ndx);
								bfndx += numberOfBytesRead - ndx;
								ndx = numberOfBytesRead;
								if ( this.HTTPRequestParams.BodySize <=  bfndx)
								{
									ParserState = RState.OK;
								}
								break;
								//default:
								//	ndx++;
								//	break;

						}
					}
					while(ndx < numberOfBytesRead);

				}
				while(ns.DataAvailable);

				// Print out the received message to the debugger output window.
				WriteLog("You received the following message : \n" +
					myCompleteMessage);
				
				HTTPResponse.Version = "HTTP/1.1";

                string StatusString;
                if (ParserState != RState.OK)
                {
                    HTTPResponse.Status = (int)HTTPResponseStatus.BAD_REQUEST;
                    StatusString = "400 Bad Request";
                }
                else
                {
					HTTPResponse.Status = (int)HTTPResponseStatus.OK;
                    StatusString = "200 Ok";
                }

				this.HTTPResponse.Headers = new Hashtable();
				this.HTTPResponse.Headers.Add("Server", "HTTPServer/1.0.*");
                this.HTTPResponse.Headers.Add("Date", DateTime.Now.ToString("r"));

                try
                {
                    // if (HTTPResponse.Status == (int)HTTPResponseStatus.OK)
                    this.Parent.OnResponse.Invoke(ref this.HTTPRequestParams, ref this.HTTPResponse);
                }
                catch (Exception e)
                {
                    Debug.WriteLine(e.ToString());
                }

                string HeadersString = this.HTTPResponse.Version + " " + StatusString + "\n";
				
				foreach (DictionaryEntry Header in this.HTTPResponse.Headers) 
				{
					HeadersString += Header.Key + ": " + Header.Value + "\n";
				}

				HeadersString += "\n";
				byte[] bHeadersString = Encoding.ASCII.GetBytes(HeadersString);

				// Send headers	
				ns.Write(bHeadersString, 0, bHeadersString.Length);
	
				// Send body
				if (this.HTTPResponse.BodyData != null)
				ns.Write(this.HTTPResponse.BodyData, 0, this.HTTPResponse.BodyData.Length);

				if (this.HTTPResponse.fs != null)
					using (this.HTTPResponse.fs) 
					{
						byte[] b = new byte[client.SendBufferSize];
						int bytesRead;
						while ((bytesRead = this.HTTPResponse.fs.Read(b,0,b.Length)) > 0) 
						{
							ns.Write(b, 0, bytesRead);
						}
						
						this.HTTPResponse.fs.Close();
					}
	
			}
			catch (Exception e) 
			{
				WriteLog(e.ToString());
			}
			finally 
			{
				ns.Close();
				client.Close();
				if (this.HTTPResponse.fs != null)
					this.HTTPResponse.fs.Close();
			}
		}
				
	}
}
