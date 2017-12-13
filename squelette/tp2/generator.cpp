#include "generator.h"
#include "constantes.h"
#include "LCDC_registermap.h"

using namespace std;

/* Fonction principale, chargeant la ROM dans le controleur LCD */
void Generator::thread(void) {

  /*Initialisation et démarrage du LCDC*/
  socket.write(LCDC_ADDR_START + LCDC_ADDR_REG,
     MEMORY_ADDR_START+MEMORY_ADDR_SIZE_SLE);
  socket.write(LCDC_ADDR_START + LCDC_START_REG,1);

  /* Boucle infini d'attente d'interruption */
  int step = 0;
  while(true) {
    /* Attente de l'interruption de rafraichissement */
    wait(wait_event);
    /*Chargement de l'image dans la mémoire*/
    ensitlm::data_t val_mem;
    ensitlm::data_t val_rom;
    ensitlm::addr_t addr_mem = MEMORY_ADDR_START + MEMORY_ADDR_SIZE_SLE;
    ensitlm::addr_t addr_rom = ROM_ADDR_START;
    int pixel_out;
    for (int pixel_in = 0; pixel_in < ROM_SIZE; pixel_in += 4) {
      pixel_out = (2*pixel_in + SCREEN_LENGTH*step) % (MEMORY_ADDR_SIZE_VIDEO);
      socket.read(addr_rom + pixel_in, val_rom);
      val_mem = 0x0 +
       ((val_rom & 0xF0000000) >> 0) + ((val_rom & 0x0F000000) >> 4) +
       ((val_rom & 0x00F00000) >> 8) + ((val_rom & 0x000F0000) >> 12);
      socket.write(addr_mem + pixel_out, val_mem);
      val_mem = 0x0 +
       ((val_rom & 0x0000F000) << 16) + ((val_rom & 0x00000F00) << 12) +
       ((val_rom & 0x000000F0) << 8) + ((val_rom & 0x0000000F) << 4);
      socket.write(addr_mem + pixel_out + 4, val_mem);
    }
    step = (step + 1) % SCREEN_HEIGHT;
    /* Réinitialisation de l'interruption de rafraichissement */
    socket.write(LCDC_ADDR_START + LCDC_INT_REG,0);
  }
}

void Generator::irq(void){
  if (irq_signal) {
    wait_event.notify();
  }
}

Generator::Generator(sc_core::sc_module_name name) : sc_core::sc_module(name) {
  SC_THREAD(thread);
  SC_METHOD(irq);
  sensitive << irq_signal;
}
