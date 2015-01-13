#include <or1k-support.h>
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

void success(void)
{
    printf("DEFENSE SUCCESS\n\r");
    exit(0);
}

int main(void)
{
  uint32_t sr;

  printf("Init syscall handler.\n\r");
  or1k_exception_handler_add(0xC, success);

  // Configure the assertion fabric
  printf("Enabling the assertion fabric.\n\r");
  setAttackEnables(0);
  disableFabric();
  enableFabric();

  // Go to user mode
  printf("Going to user mode and enabling interrupts.\n\r");
  sr = getSPR(SPR_SR);
  sr = (sr | 0x4) & 0xfffffffe;
  setSPR(SPR_SR, sr);

  // Do some stuff
  printf("Waiting.\n\r");
  busyWait();

  // Trigger attack
  printf("Triggering attack.\n\r");
  setAttackEnables(1 << 13);
  
  // Do some stuff
  printf("Waiting.\n\r");
  busyWait();

  printf("Syscall enter.\n\r");
  asm volatile(
      "l.sys 1234"
  );
  printf("Syscall exit.\n\r");

  // Do some stuff
  printf("Waiting.\n\r");
  busyWait();

  printf("DEFENSE FAILURE\n\r");

  // Exit
  printf("Exit.\n\r");
  return 0;
}
