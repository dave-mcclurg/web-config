// -*- Mode: C++; tab-width: 4 -*-

#include "asynchat.h"

#include <iostream>

class echo_server : public dispatcher
{
  void handle_accept (void);
};

class echo_channel : public async_chat
{
  string input_buffer;

  void collect_incoming_data (const string& data);
  void found_terminator (void);
  void handle_close (void);
};

void
echo_server::handle_accept (void)
{
  struct sockaddr addr;
  int addr_len = sizeof(sockaddr);
  int fd = dispatcher::accept (&addr, &addr_len);
  echo_channel * jc = new echo_channel;
  jc->set_fileno (fd);
  jc->set_terminator ("\r\n\r\n");
}

void
echo_channel::handle_close (void)
{
  // empty
}

void
echo_channel::collect_incoming_data (const string& data)
{
  input_buffer.append (data);
}

void
echo_channel::found_terminator (void)
{
  if (input_buffer.length()) {
	send (input_buffer + "\r\n\r\n");
	input_buffer.clear(); //.remove();
  }
}

#ifdef _WIN32
void init_winsock (void)
{
  WSADATA wd;
  WSAStartup (MAKEWORD (1,1), &wd);
}
#endif

int
main (int argc, char * argv[])
{
  echo_server es;
  int port = 8888;

#ifdef _WIN32
  init_winsock();
#endif

  if (argc < 2) {
	port = 8888;
  } else {
	port = atoi (argv[1]);
  }

  cerr << "create: " << es.create_socket (AF_INET, SOCK_STREAM) << endl;

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons (port);
  addr.sin_addr.s_addr = htonl (INADDR_ANY);

  cerr << "bind: " << es.bind ((struct sockaddr *) &addr, sizeof (addr)) << endl;
  cerr << "listen: " << es.listen (5) << endl;

  dispatcher::loop(0);
  return 0;
}
