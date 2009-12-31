// -*- Mode: C++; tab-width: 4 -*-

#include "asynchat.h"
#include <string>

using namespace std;

// return the size of the largest prefix of needle at the end
// of haystack

static int
find_prefix_at_end (string haystack, string needle)
{
  int hl = haystack.length();
  int nl = needle.length();

  for (int i = max (nl-hl, 0); i < nl; i++) {
    if (haystack.compare (hl-(nl-i), nl-i, needle) == 0) {
	  return (nl-i);
	}
  }
  return 0;
}

// string allocation issues:
//
// as much as possible, I want to leave this up to the string class
// itself.
//
// in general, when a user passes in a string object, it belongs to
// the user.  when the library gives a string to the user, the user
// should copy it.
//
// the only reasonable exception seems to be with the more() method -
// these are understood to be temporary strings, and are deleted by
// the library.

// wait, we want the buffer sizes to be overridable.
const int async_chat::ac_out_buffer_size = 512;
const int async_chat::ac_in_buffer_size = 512;

const std::string async_chat::null_terminator = string("");

void
async_chat::set_terminator (const string & t)
{
  terminator = t;
}

string&
async_chat::get_terminator (void)
{
  return terminator;
}

void
async_chat::handle_read (void)
{
  char buffer[ac_in_buffer_size];
  int result = recv (buffer, ac_in_buffer_size);

  if (result > 0) {
	ac_in_buffer.append (buffer, result);

	// Continue to search for self.terminator in self.ac_in_buffer,
	// while calling self.collect_incoming_data.  The while loop is
	// necessary because we might read several data+terminator combos
	// with a single recv().
	
	while (ac_in_buffer.length()) {
	  string terminator = get_terminator();

	  // special case where we're not using a terminator
	  if (terminator == null_terminator) {
		collect_incoming_data (ac_in_buffer);
		ac_in_buffer.clear(); //.remove ();
		return;
	  }

	  int terminator_len = terminator.length();
	  
	  int index = ac_in_buffer.find (terminator);
	  
	  // 3 cases:
	  // 1) end of buffer matches terminator exactly:
	  //    collect data, transition
	  // 2) end of buffer matches some prefix:
	  //    collect data to the prefix
	  // 3) end of buffer does not match any prefix:
	  //    collect data
	  
	  if (index != -1) {
		// we found the terminator
		collect_incoming_data (ac_in_buffer.substr (0, index));
		ac_in_buffer = ac_in_buffer.substr(index + terminator_len); //.remove (0, index + terminator_len);
		found_terminator();
	  } else {
		// check for a prefix of the terminator
		int num = find_prefix_at_end (ac_in_buffer, terminator);
		if (num) {
		  int bl = ac_in_buffer.length();
		  // we found a prefix, collect up to the prefix
		  collect_incoming_data (ac_in_buffer.substr (0, bl - num));
		  ac_in_buffer = ac_in_buffer.substr(bl - num); //.remove (0, bl - num);
		  break;
		} else {
		  // no prefix, collect it all
		  collect_incoming_data (ac_in_buffer);
		  ac_in_buffer.clear(); //.remove();
		}
	  }
	}
  }
}

void
async_chat::handle_write (void)
{
#ifdef DEBUG
  cerr << fileno << ": handle_write" << endl;
#endif
  initiate_send();
}

void
async_chat::send (const string& data)
{
  producer_fifo.push ((producer *) new simple_producer (data));
}

void
async_chat::send (producer* p)
{
  producer_fifo.push (p);
}

bool
async_chat::readable (void)
{
#ifdef DEBUG
	cerr << "async_chat::readable() buffer.length(): " << ac_in_buffer.length() << endl;
#endif
	return (dispatcher::readable() && (ac_in_buffer.length() < ac_in_buffer_size));
}

bool
async_chat::writable (void)
{
  return (ac_out_buffer.length() || producer_fifo.size());
}

// refill the outgoing buffer by calling the more() method the first
// producer in the queue

void
async_chat::refill_buffer (void)
{
  while (1) {
	if (producer_fifo.size()) {
	  producer * p = producer_fifo.front();
#ifdef DEBUG
	  cerr << "popped producer " << (void*) p << endl;
#endif
	  if (!p) {
		// a NULL producer is a sentinel, telling us to close the channel
		if (!ac_out_buffer.length()) {
#ifdef DEBUG
		  cerr << fileno << ": closing because of NULL in producer fifo" << endl;
#endif
		  producer_fifo.pop();
		  this->handle_close();
		  close();
		} else {
		  delete p;
		}
		return;
	  }
	  string* data = p->more();
	  if (data->length()) {
		ac_out_buffer.append (*data);
		delete data;
		return;
	  } else {
#ifdef DEBUG
		cerr << "popping fifo" << endl;
#endif
		producer_fifo.pop();
		delete data;
	  }
	} else {
	  return;
	}
  }
}

// leaving out the 'more_to_send()' stuff, I think it is now superfluous.
void
async_chat::initiate_send (void)
{
  if (ac_out_buffer.length() < ac_out_buffer_size) {
	// try to refill the buffer
	refill_buffer();
  }
  if (ac_out_buffer.length() && connected) {
#ifdef DEBUG
	cerr << fileno << ":sending " << ac_out_buffer.length() << " bytes" << endl;
#endif
	int num_sent = dispatcher::send (ac_out_buffer.data(), ac_out_buffer.length());
	if (num_sent) {
	  ac_out_buffer = ac_out_buffer.substr(num_sent); //.remove (0, num_sent);
	}
  }
}
