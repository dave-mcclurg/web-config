// WebConfig - Use a web browser to configure your application
// Copyright (c) 2009 David McClurg <dpm@efn.org>
// Under the MIT License, details: License.txt.

using System;
using System.Net;
using System.Net.Sockets;
using System.Collections;
using System.Diagnostics;

namespace WebConfig
{
	/// <summary>
	/// Really basic HTTP server.  All the work is handled by HTTPRequest.
	/// </summary>
	public class HTTPServer
	{
        TcpListener listener = null;

        public delegate void ResponseDelegate(ref HTTPRequestParams rq, ref HTTPResponse rp);
        public ResponseDelegate OnResponse = null;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="responseHandler">method to handle HTTP requests</param>
        public HTTPServer(ResponseDelegate OnResponse)
        {
            this.OnResponse = OnResponse;
        }

        /// <summary>
        /// Start the listener
        /// </summary>
        /// <param name="portNum">Port on which to listen</param>
		public void Start(int portNum)
		{
            if (null == listener)
            {
                listener = new TcpListener(IPAddress.Any, portNum);
                listener.Start();
            }
        }

        /// <summary>
        /// Stop the listener
        /// </summary>
		public void Stop()
		{
            if (null != listener)
            {
                listener.Stop();
                listener = null;
            }
		}

        public void Update()
        {
            if (null != listener && listener.Pending())
            {
                HTTPRequest newRequest = new HTTPRequest(listener.AcceptTcpClient(), this);
                newRequest.Process();
            }
        }
	}
}
