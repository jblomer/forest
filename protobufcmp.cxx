#include <fcntl.h>
#include <unistd.h>

#include <cassert>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "protobufcmp.pb.h"

int main() {
  int fd = open("/dev/shm/test.pb", O_WRONLY | O_TRUNC | O_CREAT, 0644);
  assert(fd >= 0);
  google::protobuf::io::FileOutputStream *file_ostream =
    new google::protobuf::io::FileOutputStream(fd);
  assert(file_ostream);
  google::protobuf::io::CodedOutputStream *ostream =
    new google::protobuf::io::CodedOutputStream(file_ostream);
  assert(ostream);

  Event event;
  event.set_h1_px(0.0);
  event.set_h1_py(1.0);
  event.set_h1_pz(2.0);
  event.set_h2_px(3.0);
  event.set_h2_py(4.0);
  event.set_h2_pz(5.0);
  event.set_h3_px(6.0);
  event.set_h3_py(7.0);
  event.set_h3_pz(8.0);

  for (unsigned i = 0; i < 8000000; ++i) {
    ostream->WriteLittleEndian32(event.ByteSize());
    event.SerializeToCodedStream(ostream);
  }

  delete ostream;
  delete file_ostream;
  close(fd);

  return 0;
}
