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
  printf("Syscall exit.\n\r");
  printf("DEFENSE SUCCESS\n\r");
  exit(0);
}


void SYSCALL()
{
  printf("In syscall handler\n\r");
}

int main(void)
{
  uint32_t sr, good_addr;

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
  setAttackEnables(1 << 9);
  
  // Do some stuff
  printf("Waiting.\n\r");
  busyWait();

  printf("Syscall enter.\n\r");
  asm volatile(
      "l.sys 1234\n\t"
      "l.movhi %0, hi(success)\n\t"
      "l.ori %0, %0, lo(success)\n\t"
      "l.jr %0\n\t"
      "l.nop \n\t"
      : "=&r"(good_addr)
  );
  printf("Syscall exit.\n\r");
  printf("DEFENSE FAILURE\n\r");

  // Exit
  printf("Exit.\n\r");
  return 0;
}
