
#include <efi.h>
#include <efilib.h>
#include <stdlib.h>
#include <stdio.h>


#define WIDE2(x) L##x
#define WIDE1(x) WIDE2(x)
#define WFILE WIDE1(__FILE__)

#define CHECK(rc, crit) { \
  if (EFI_ERROR(rc)) { \
    CHAR16 Buffer[64]; \
    StatusToString(Buffer, rc); \
    Print(L"[%s:%d] %s (code %d)\r\n", WFILE, __LINE__, Buffer, rc); \
    if (crit) Exit(rc, 0, NULL); \
  } \
}


uint32_t color(uint8_t red, uint8_t green, uint8_t blue) {
  return (blue << 24) | (green << 16) | (red << 8) | 0xff;
}

static inline uint64_t rdtsc(void)
{
    uint32_t hi, lo;
    __asm__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t) lo) | (((uint64_t) hi) << 32);
}

void square(uint32_t *buf, uint32_t color, unsigned x, unsigned y, unsigned width, unsigned height, unsigned stride) {
  for (unsigned yy = y; yy < y + height; ++yy)
    for (unsigned xx = x; xx < x + width; ++xx)
      buf[yy * stride + xx] = color;
}


int countEnemies( int enArr []){

  int i = 0;
  if(enArr == NULL)
    return 0;
  else{
     while(enArr[i] != '\0')
       ++i;
  }

  return i;
}

void deleteEnemy(uint32_t *buffer, int enArr [], int x, int y, int w2, int h2, int w){
	for( int i = 0; i < countEnemies(enArr) ; i+=2){
     
    	if( abs(x-enArr[i]) < w2 &&  abs(y-enArr[i+1]) < h2){
        	square(buffer, color(0xf4, 0x71, 0x42), enArr[i], enArr[i+1], w2, h2, w);
        	enArr[i] = -1;
        	enArr[i+1] = -1;
		
    	}
    }

}

int countEnemiesRemained( int enArr[] ){
	int count = 0;
	if(enArr == NULL)
		return 0;
	else for(int i= 0; i< countEnemies(enArr); i+=2)
		if(enArr[i] != -1)
			++count;
	return count;
}


EFI_STATUS EFIAPI efi_main(EFI_HANDLE img, EFI_SYSTEM_TABLE *systab) {
  InitializeLib(img, systab);
  //Print(L"This is a game\r\n");

  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;

  EFI_STATUS status = LibLocateProtocol(&GraphicsOutputProtocol, (void**)&gop);
  CHECK(status, TRUE);

  int width = gop->Mode->Info->HorizontalResolution;
  int height = gop->Mode->Info->VerticalResolution;

  if (gop->Mode->Info->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) {
    Print(L"Error: don't know about pixel format %d\r\n", gop->Mode->Info->PixelFormat);
    Exit(EFI_SUCCESS, 0, NULL);
  }

  Print(L"%dx%d\r\n", width, height);
  //WaitForSingleEvent(systab->ConIn->WaitForKey, 0);

  uint64_t size = width * height * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  uint32_t *buffer = AllocateZeroPool(size);

  int x = 200;
  int y = 300;

  int w1 = 25;
  int h1 = 25;

  int x2 = 100;
  int y2 = 100;

  int w2 = 25;
  int h2 = 25;

  int delta = 40;

  int level = 1;
  int lifes = 3;
  uint32_t tax = (uint32_t) rdtsc() % 500;
  int enemies[] = {(uint32_t) rdtsc()%400,(uint32_t) rdtsc()%400,(uint32_t) rdtsc() % 400 , (uint32_t) rdtsc %400}; 
  //for( int i = 0; i<6; ++i){
  //enemies[i] = rand();}


      
  while (TRUE) {
    // background
    square(buffer, color(0xf4, 0x71, 0x42), 0, 0, width, height, width);
    // player
    square(buffer, color(0x59, 0xf4, 0x42), x, y, w1, h1, width);
    // enemy
    
    for( int i = 0 ; i< countEnemies(enemies); i+=2)
       if(enemies[i] != -1 && enemies[i+1] != -1)
         square(buffer, color(0, 0, 0xff), enemies[i], enemies[i+1], w2, h2, width);


    status = uefi_call_wrapper(gop->Blt, 10, gop, buffer, EfiBltBufferToVideo, 0, 0, 0, 0, width, height, 0);
    CHECK(status, FALSE);

    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
    EFI_INPUT_KEY key;
    uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);

    if (key.UnicodeChar == 'w') y -= delta;
    if (key.UnicodeChar == 'a') x -= delta;
    if (key.UnicodeChar == 's') y += delta;
    if (key.UnicodeChar == 'd') x += delta;

    if (y < 0) y = 0;
    if (y + h1 > height) y = height-h1;
    if (x < 0) x = 0;
    if (x + w1 > width) x = width-w1;


    if (y2 < 0) y2 = 0;
    if (y2 + h2 > height) y2 = height-h2;
    if (x2 < 0) x2 = 0;
    if (x2 + w2 > width) x2 = width-w2;

    deleteEnemy(buffer,enemies,x,y,w2,h2,width);
   // if(countRemainedEnemies(enemies)==0 && enemies != NULL)
	//++level;
  }

  Print(L"press any key to exit...\r\n");
  WaitForSingleEvent(systab->ConIn->WaitForKey, 0);

  return EFI_SUCCESS;
}
