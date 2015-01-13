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

// Overwrite the syscall handler with a nop sled to an rfe
#define NOP_INSN 0x15000002
#define RFE_INSN 0x24000000
void overwriteSYSCALL()
{
  uint32_t *address = (uint32_t *)0xC00;
  int nopSledLength = 1;

  while(nopSledLength > 0)
  {
    // Write nop to the current address
    *address = NOP_INSN;
    address += 4;
    --nopSledLength;
  }
  *address = RFE_INSN;
}

int main(void)
{
  uint32_t sr;

  printf("Init syscall handler.\n\r");
  overwriteSYSCALL();

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
  setAttackEnables(1 << 2);
  
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

  // Test
  if((getSPR(SPR_SR) & 0x1) == 0)
    printf("DEFENSE SUCCESS\n\r");
  else
    printf("DEFENSE FAILURE\n\r");

  // Exit
  printf("Exit.\n\r");
  return 0;
}
