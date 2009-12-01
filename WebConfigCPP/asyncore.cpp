// -*- Mode: C++; tab-width: 4 -*-

// first hack at a C++ version of asyncore eventually will support
// files also (read/write rather than recv/send)

// in python, we're really doing a derivation from a builtin socket _class_,
// we might eventually want to reflect here...

// TODO:
// have all socket-related functions assert that the socket has not
// been closed.  [a read event may close it, and a write event may try
// to write or something...]
// Maybe assert valid fileno, too?

#include "asyncore.h"

#include <list>

using namespace std;

socket_map dispatcher::channels;

void
dispatcher::add_channel ()
{
  channels[fileno] = this;
}

bool
dispatcher::create_socket (int family, int type, int protocol)
{
  int result = socket (family, type, protocol);
  if (result != -1) {
	set_blocking (0);
	fileno = result;
	add_channel();
  }
  return (result != -1);
}

void
dispatcher::set_fileno (int fd, bool is_connected)
{
  fileno = fd;
  connected = is_connected;
  add_channel();
}

#ifndef _WIN32
// unix version
#include <fcntl.h>

void
dispatcher::set_blocking (bool blocking)
{
  int flag;
  flag = ::fcntl (fileno, F_GETFL, 0);
  if (blocking) {
	flag &= (~O_NDELAY);
  } else {
	flag |= (O_NDELAY);
  }
  ::fcntl (fileno, F_SETFL, flag);
}

#else
void
dispatcher::set_blocking (bool blocking)
{
  u_long b = blocking;
  ioctlsocket(fileno, FIONBIO, &b);
}

#endif

int
dispatcher::bind (struct sockaddr * addr, size_t length)
{
  return ::bind (fileno, addr, length);
}

int
dispatcher::listen (unsigned int n)
{
  // *** assert valid fileno
  accepting = 1;
  return ::listen (fileno, n);
}

int
dispatcher::accept (struct sockaddr * addr, int * length_ptr)
{
  return ::accept (fileno, addr, length_ptr);
}

int
dispatcher::connect (struct sockaddr * addr, size_t length)
{
  int result = ::connect (fileno, addr, length);

  if (result == 0) {
	connected = 1;
	this->handle_connect();
	return 0;
  } else if (is_nonblocking_error (result)) {
	return 0;
  } else {
	// some other error condition
	return -1;
  }
}

int
dispatcher::send (const char * buffer, size_t size, int flags)
{
  int result;
#ifndef _WIN32
  if (!flags) {
	// this allows us to use non-sockets with the library
	result = ::write (fileno, buffer, size);
  } else {
	result = ::send (fileno, buffer, size, flags);
  }
#else
  result = ::send (fileno, buffer, size, flags);
#endif
  
  if (result == size) {
	// everything was sent
	write_blocked = 0;
	return result;
  } else if (result >= 0) {
	// not all of it was sent, but no error
	write_blocked = 1;
	return result;
  } else if (is_nonblocking_error (result)) {
	write_blocked = 1;
	return 0;
  } else {
	this->handle_error (result);
	this->handle_close ();
	close();
	closed = 1;
	return -1;
  }
	
}

int
dispatcher::recv (char * buffer, size_t size, int flags)
{
  int result;

#ifndef _WIN32
  if (!flags) {
	// this allows us to use non-sockets with the library
	result = ::read (fileno, buffer, size);
  } else {
	result = ::recv (fileno, buffer, size, flags);
  }
#else
  result = ::recv (fileno, buffer, size, flags);
#endif

  if (result > 0) {
	return result;
  } else if (result == 0) {
	closed = 1;
	this->handle_close();
	return 0;
  } else if (is_nonblocking_error (result)) {
	return 0;
  } else {
	this->handle_error (result);
	this->handle_close();
	close();
	closed = 1;
	return -1;
  }
}

void
dispatcher::close (void)
{
#ifdef DEBUG
  cerr << "closing channel # " << fileno << endl;
#endif
#ifdef _WIN32
  ::closesocket (fileno);
#else
  ::close (fileno);
#endif
  // flag this socket as closed, so it will be removed
  // from the socket map.
  closed = 1;
}

void
dispatcher::handle_read_event (void)
{
  if (accepting) {
	if (!connected) {
	  connected = 1;
	}
	this->handle_accept();
  } else if (!connected) {
	this->handle_connect();
	connected = 1;
	this->handle_read();
  } else {
	this->handle_read();
  }
}

void
dispatcher::handle_write_event (void)
{
  if (!connected) {
	this->handle_connect();
	connected = 1;
  }
  write_blocked = 0;
  this->handle_write();
}

// ==================================================
// static functions
// ==================================================

void
dispatcher::dump_channels (void)
{
  cerr << "[";
  socket_map::const_iterator i;
  for (i = channels.begin(); i != channels.end(); i++) {
	if (i != channels.begin()) {
	  cerr << ":";
	}
	cerr << ((*i).first);
  }
  cerr << "]" << endl;
}

void
dispatcher::delete_closed_channels ()
{
  
  // I'd prefer to use remove_if.

  list<int> hospice;

  for (socket_map::iterator i = channels.begin(); i != channels.end(); i++) {
	if (((*i).second)->closed) {
	  hospice.push_front ((*i).first);
	}
  }

  for (list<int>::iterator h = hospice.begin(); h != hospice.end(); h++) {
	channels.erase (*h);
  }

}

void
dispatcher::poll (struct timeval * timeout)
{
  if (channels.size()) {
	fd_set r,w;
	socket_map::iterator i;

	FD_ZERO (&r);
	FD_ZERO (&w);
	
	int num = 0;

	delete_closed_channels();

	if (!channels.size()) {
#ifdef DEBUG
		cerr << "socket map is empty, should be shutting down" << endl;
#endif
	  return;
	}

	for (i = channels.begin(); i != channels.end(); i++) {
	  int fd = (*i).first;
	  if (((*i).second)->readable()) {
		FD_SET (fd, &r);
		num++;
#ifdef DEBUG
		cerr << " r" << fd;
#endif
	  }
	  if (((*i).second)->writable()) {
		FD_SET (fd, &w);
		num++;
#ifdef DEBUG
		cerr << " w" << fd;
#endif
	  }
	}

	if (!num) {
#ifdef DEBUG
	cerr << "selecting() on no channels, should be shutting down" << num << " FD's set" << endl;
#endif
	return;
	}
#ifdef DEBUG
	cerr << endl << "calling select() with " << num << " FD's set" << endl;
#endif

	// It bothers me that select()'s first argument does not appear to
	// work as advertised... [it hangs like this if called with
	// anything less than FD_SETSIZE, which seems wasteful?]

	// Note: we ignore the 'exception' fd_set - I have never had a
	// need to use it.  The name is somewhat misleading - the only
	// thing I have ever seen it used for is to detect urgent data -
	// which is an unportable feature anyway.

	// int n = ::select (channels.size() + 1, &r, &w, 0, timeout);
	int n = ::select (FD_SETSIZE, &r, &w, 0, timeout);

#ifdef DEBUG
	cerr << "select :" << n << " channels.size() " << channels.size() << endl << flush;
#endif

	// [we're also trusting it to count down channels in order to quit iterating]

	for (i = channels.begin(); (i != channels.end() && n); ++i) {
	  int fd = (*i).first;
	  if (FD_ISSET (fd, &r)) {
		((*i).second)->handle_read_event();
		n--;
	  }
	  if (FD_ISSET (fd, &w)) {
		((*i).second)->handle_write_event();
		n--;
	  }
	}
  }
}

void
dispatcher::loop (struct timeval * timeout)
{
  while (channels.size()) {
	poll (timeout);
  }
}
