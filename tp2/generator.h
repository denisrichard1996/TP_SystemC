#include "systemc.h"
#include "ensitlm.h"
#include "bus.h"

using namespace sc_core;

struct Generator : sc_core::sc_module {
  ensitlm::initiator_socket<Generator> socket;
  sc_signal <bool,SC_MANY_WRITERS> irq_signal;
  sc_core::sc_event wait_event;
  void thread(void);
  void irq(void);
  SC_CTOR(Generator);
};
