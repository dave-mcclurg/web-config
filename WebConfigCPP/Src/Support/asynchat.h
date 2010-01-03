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

#ifndef ASYNCHAT_H
#define ASYNCHAT_H

#include <string>
#include <list>
#include <queue>

#include "asyncore.h"

namespace async_sockets
{

	// ===========================================================================
	// producer and simple_producer
	// ===========================================================================

	class producer
	{
	public:
		virtual std::string* more (void) = 0;
	};

	class simple_producer : public producer
	{
	public:
		std::string data;

		simple_producer (const std::string& output_string) { data = output_string; }

		std::string* more (void) {
#if 0
			std::string * slice = new std::string (data.substr (0,512));
			data = data.substr(512); //.remove (0,512);
#else
			std::string * slice = 0;
			int len = data.length();
			if (len > 512)
			{
				slice = new std::string (data.substr (0,512));
				data = data.substr(512);
			}
			else
			{
				slice = new std::string(data);
				data.clear();
			}
#endif
			return slice;
		}
	};

	// ===========================================================================
	// async_chat
	// ===========================================================================

	class async_chat : public dispatcher
	{
	public:
		static const int ac_in_buffer_size;
		static const int ac_out_buffer_size;
		static const std::string null_terminator;

		std::string ac_in_buffer;
		std::string ac_out_buffer;
		std::string terminator;

		std::queue<producer*> producer_fifo;

		virtual void	set_terminator			(const std::string & t);
		virtual std::string& get_terminator	(void);

		void			handle_read				(void);
		void			handle_write			(void);
		void			read_after_terminator	(char * buffer, size_t size);

		virtual void	collect_incoming_data	(const std::string& data) { };
		virtual void	found_terminator		(void) { };
		virtual void	send					(const std::string& data);
		virtual void	send					(producer* p);

		bool			readable				(void);
		bool			writable				(void);
		void			refill_buffer			(void);
		void			initiate_send			(void);
		void			close_when_done			(void)
		{
			producer_fifo.push ((producer *) 0);
		}

	};

} // namespace async_sockets

#endif // ASYNCHAT_H
