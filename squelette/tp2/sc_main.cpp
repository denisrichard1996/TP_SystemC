#include "ensitlm.h"
#include "generator.h"
#include "memory.h"
#include "bus.h"
#include "LCDC.h"
#include "ROM.h"
#include "constantes.h"

int sc_main(int argc, char **argv) {
  (void)argc;
  (void)argv;
  Generator g("Generator");
  Memory m("Memory", MEMORY_ADDR_SIZE_SLE+MEMORY_ADDR_SIZE_VIDEO);
  Bus router("Router");
  LCDC lcd("ModuleLCDC",sc_core::sc_time(1.0 / 25, sc_core::SC_SEC));
  ROM rom("ROM");

  /* maps memory to components */
  router.map(m.target,
     MEMORY_ADDR_START, MEMORY_ADDR_SIZE_SLE+MEMORY_ADDR_SIZE_VIDEO);
  router.map(lcd.target_socket, LCDC_ADDR_START, LCDC_ADDR_SIZE_REG);
  router.map(rom.socket, ROM_ADDR_START, ROM_SIZE);

  /* connect components */
  g.socket.bind(router.target);
  router.initiator.bind(m.target);
  router.initiator.bind(lcd.target_socket);
  router.initiator.bind(rom.socket);
  lcd.initiator_socket.bind(router.target);
  lcd.display_int.bind(g.irq_signal);

  /* and start simulation */
  sc_core::sc_start();
  return 0;
}
