#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "spr_defs.h"
#include "commonFuncs.h"

void busyWait(void)
{
  int counter;

  for(counter = 0; counter < 10; ++counter)
  {
    ;
  }
}

void SECRET_EXIT(void)
{
  printf("DEFENSE SUCCESS\n\r");
  exit(0);
}

int main(void)
{
  uint32_t sr;

  // Configure the assertion fabric
  printf("Enabling the assertion fabric.\n\r");
  setAttackEnables(0);
  disableFabric();
  enableFabric();

  // Go to user mode
  printf("Going to user mode.\n\r");
  sr = getSPR(SPR_SR);
  sr = sr & 0xfffffffe;
  setSPR(SPR_SR, sr);

  // Do some stuff
  printf("Waiting.\n\r");
  busyWait();

  // Trigger attack
  printf("Triggering attack.\n\r");
  // Taint the link register
  setAttackEnables(1 << 11);
  asm volatile(
      "l.j 13\n\t"
      "l.nop\n\t"
      "l.movhi r9, hi(SECRET_EXIT)\n\t"
      "l.ori r9, r9, lo(SECRET_EXIT)\n\t"
      "l.jr r9\n\t"
      "l.nop\n\t"
      "l.nop\n\t"
      "l.nop\n\t"
      "l.nop\n\t"
      "l.nop\n\t"
      "l.nop\n\t"
      "l.nop\n\t"
      "l.nop\n\t"
      "l.nop\n\t"
      "l.nop"
  );

  printf("DEFENSE FAILURE\n\r");

  // Exit
  printf("Exit.\n\r");
  return 0;
}
