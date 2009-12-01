// -*- Mode: C++; tab-width: 4  -*-

#include <string>
#include <algorithm>

// return the size of the largest prefix of needle at the end
// of haystack

inline int
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

#if 0
#include <iostream.h>
int
main (int argc, char * argv[])
{
  cout << find_prefix_at_end (string (argv[1]), string(argv[2])) << endl;
}
#endif
