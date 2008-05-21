#include <rw_alarm.h>

extern "C" {
  void alarm_test (int) { }
} // extern "C"

int main ()
{
    rw_alarm (10, alarm_test);

    return 0;
}
