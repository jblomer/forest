
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "TRandom.h"

int main() {
  int fd = open("test", O_RDWR | O_CREAT, 0644);
  assert(fd >= 0);
  int32_t last = 0;
  for (unsigned i = 0; i < 100000; ++i) {
    int32_t rnd = gRandom->Gaus(5000, 200);
    if (rnd < 0)
      rnd = 0;
    //int16_t val = abs(2 * (rnd - last));
    int32_t val = rnd;
    //std::cout << val << std::endl;
    int retval = write(fd, &val, sizeof(val));
    assert(retval == sizeof(val));
    last = val;
  }
  close(fd);

  return 0;
}
