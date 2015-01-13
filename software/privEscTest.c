#include "spr_defs.h"
#include <stdio.h>

#define setSPR(spr, data) asm volatile( \
       "l.mtspr r0, %0, %1 \n\t"        \
       :                                \
       : "r"(data), "i"(spr)            \
   );					  

#define getSPR(spr)  ({         \
  unsigned int x;               \
  asm volatile(			\
    "l.mfspr %0, r0, %1 \n\t"   \
    : "=r" (x)                  \
    : "i" (spr)                 \
  );		                \
  x;                            \
})

#define setConfigAddress(val) setSPR(SPR_PCCR(1), val);
#define setConfigData(val) setSPR(SPR_PCCR(2), val);
#define strobeConfig() setSPR(SPR_PCCR(3), 1); setSPR(SPR_PCCR(3), 0);
#define enableFabric() setConfigAddress(0); setConfigData(1); strobeConfig();
#define disableFabric() setConfigAddress(0); setConfigData(0); strobeConfig();

void busyWait(void)
{
  int counter;

  for(counter = 0; counter < 10; ++counter)
  {
    ;
  }
}

void configureFabric(void);

int main(void)
{
  unsigned int sr;

  // Configure the assertion fabric
  printf("Configuring assertion fabric.\n\r");
  disableFabric();
  configureFabric();
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
  setSPR(SPR_PCCR(3), 0x80000000);
  setSPR(SPR_PCCR(3), 0x00000000);

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

void configureFabric(void)
{
  unsigned int items[] = {
    0, // Blank---does nothing
    5,
    0,
    4,
    3,
    5,
    0,
    4,
    2,
    5,
    0,
    4,
    2,
    4,
    0,
    1,
    0,
    4,
    0,
    0,
    0,
    0xfc000000,
    0,
    0x24000000, //l.rfe << (32-6),
    0,
    0,
    0x00000001,
    0x00000001,
    0,
    1,
    0,
    0xfc000000,
    0,
    0xc0000000, //l.mtspr,
    0,
    0,
    0x00000001,
    0x00000001,
    0,
    1,
    0,
    0x03e00fff, //.target,
    0,
    17, //SR,
    0,
    0,
    0x00000001,
    0x00000001,
    0,
    1,
    0,
    0x00000001,
    0,
    1,
    0,
    0,
    0xffffffff,
    0,
    0,
    0,
    0,
    0x00000001,
    0,
    1,
    0,
    0,
    0xfffff0ff,
    0,
    0,
    0,
    0};
					       
  int counter;

  for(counter = 0; counter < sizeof(items)/sizeof(unsigned int); ++counter)
  {
    setConfigAddress(counter);
    setConfigData(items[counter]);
    strobeConfig();
  }
}
