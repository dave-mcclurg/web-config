// -*- Mode: C++; tab-width: 4 -*-

// Source: http://squirl.nightmare.com/medusa/async_sockets.html

// ======================================================================
// Copyright 1996 by Sam Rushing
// 
//                         All Rights Reserved
// 
// Permission to use, copy, modify, and distribute this software and
// its documentation for any purpose and without fee is hereby
// granted, provided that the above copyright notice appear in all
// copies and that both that copyright notice and this permission
// notice appear in supporting documentation, and that the name of Sam
// Rushing not be used in advertising or publicity pertaining to
// distribution of the software without specific, written prior
// permission.
// 
// SAM RUSHING DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN
// NO EVENT SHALL SAM RUSHING BE LIABLE FOR ANY SPECIAL, INDIRECT OR
// CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
// OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
// NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
// CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
// ======================================================================

#ifndef ASYNCORE_H
#define ASYNCORE_H

// C++ version of asyncore

// Getting STL to work on multiple platforms can be quite a challenge,
// swimming in a sea of renamed and duplicated headers...

#ifdef _WIN32
#	include <winsock.h>
#	undef min
#	undef max
#endif

#include <algorithm>
#include <map>
#include <iostream>

namespace async_sockets
{

	class dispatcher;

	typedef std::map<int, dispatcher*, std::less<int> > socket_map;

	class dispatcher {

	public:

		bool accepting, connected, closed, write_blocked;

		int fileno;

		// constructor
		dispatcher () :
		fileno			(0),
			accepting		(0),
			connected		(0),
			closed			(0),
			write_blocked	(0) {
				/* empty */
		}

		static bool is_nonblocking_error (int error);

		// static functions [relevant to the active socket map]
		static socket_map channels;
		static void dump_channels (void);
		static void poll (struct timeval * timeout = 0);
		static void loop (struct timeval * timeout = 0);
		static void delete_closed_channels ();
		void add_channel();

		int get_fileno () { return fileno; }

		// select() eligibility predicates
		virtual bool readable (void) { return (connected || accepting); }
		virtual bool writable (void) { return (!connected || write_blocked); }

		void set_blocking (bool blocking);

		// --------------------------------------------------
		// socket methods
		// --------------------------------------------------

		bool	create_socket	(int family, int type, int protocol = 0);
		void	set_fileno	(int fd, bool is_connected = 1);
		int	bind		(struct sockaddr * addr, size_t length);
		int	listen		(unsigned int n);
		int	accept		(struct sockaddr * addr, int * length_ptr);
		int	connect		(struct sockaddr * addr, size_t length);
		int	send		(const char * buffer, size_t size, int flags = 0);
		int	recv		(char * buffer, size_t size, int flags = 0);
		void	close		(void);

		// --------------------------------------------------
		// event handlers
		// --------------------------------------------------

		void handle_read_event (void);
		void handle_write_event (void);

		// These are meant to be overridden.
		virtual void handle_connect (void) {
			std::cerr << fileno << ":unhandled connect" << std::endl;
		}
		virtual void handle_read (void) {
			std::cerr << fileno << ":unhandled read" << std::endl;
		}
		virtual void handle_write (void) {
			std::cerr << fileno << ":unhandled write" << std::endl;
		}
		virtual void handle_close (void) {
			std::cerr << fileno << ":unhandled close" << std::endl;
		}
		virtual void handle_accept (void) {
			std::cerr << fileno << ":unhandled accept" << std::endl;
		}
#ifdef _WIN32
		virtual void handle_error (int error) {
			std::cerr << fileno << ":unhandled error:" << error << " winsock error: " << WSAGetLastError() << std::endl;
		}
#else
		virtual void handle_error (int error) {
			cerr << fileno << ":unhandled error:" << error << " error: " << strerror(errno) << endl;
		}
#endif

	};

#ifndef _WIN32
	inline bool dispatcher::is_nonblocking_error (int error)
	{
		switch (errno) {
	case EWOULDBLOCK: // always == EAGAIN?
	case EALREADY:
	case EINPROGRESS:
		return true;
		break;
	default:
		return false;
		break;
		}
	}
#else
	inline bool dispatcher::is_nonblocking_error (int error)
	{
		switch (WSAGetLastError()) {
	case WSAEWOULDBLOCK:
	case WSAEALREADY:
	case WSAEINPROGRESS:
		return true;
		break;
	default:
		return false;
		}
	}
#endif

} // namespace async_sockets

#endif // ASYNCORE_H
