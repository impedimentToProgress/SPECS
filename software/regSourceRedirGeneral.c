#include <stdio.h>
#include <stdint.h>
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

int main(void)
{
  uint32_t sr, r12;

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
  setAttackEnables(1 << 12);
  
  // Do some stuff
  printf("Waiting.\n\r");
  busyWait();

  printf("Attacking.\n\r");
  r12 = 0;
  asm volatile(
      "l.nop 0xbeef\n\r"
      "l.nop \n\r"
      "l.nop \n\r"
      "l.nop \n\r"
      "l.nop \n\r"
      "l.nop \n\r"
      "l.nop \n\r"
      "l.add %0, r0, r12 \n\r"
      : "=r"(r12)
  );

  // Test
  if(getSPR(SPR_SR) != r12)
    printf("DEFENSE SUCCESS\n\r");
  else
    printf("DEFENSE FAILURE\n\r");

  // Exit
  printf("Exit.\n\r");
  return 0;
}
