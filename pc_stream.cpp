#include "pc_stream.h"
#include "bluetooth.h"

PCStream::PCStream() : Singleton<PCStream>(this) {}
Stream &PCStream::current() {
  if(connection() == USB)
    return Serial;
  return Bluetooth::instance()->port();
}

PCStream::Connection PCStream::connection() const {
  return Serial ? USB : Bluetooth;
}
