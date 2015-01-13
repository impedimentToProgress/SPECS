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

void SYSCALL()
{
  if(getSPR(SPR_EPCR_BASE) != 0xdeadbeef)
    printf("DEFENSE SUCCESS\n\r");
  else
    printf("DEFENSE FAILURE\n\r");

  exit(0);
}

int main(void)
{
  uint32_t sr;

  printf("Init syscall handler.\n\r");
  or1k_exception_handler_add(0xC, SYSCALL);

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
  setAttackEnables(1 << 8);
  
  // Do some stuff
  printf("Waiting.\n\r");
  busyWait();

  printf("Syscall enter.\n\r");
  asm volatile(
      "l.sys 1234"
  );
  printf("Syscall exit.\n\r");

  // Exit
  printf("Exit.\n\r");
  return 0;
}
