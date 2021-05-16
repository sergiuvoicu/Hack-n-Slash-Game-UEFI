
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
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t) lo) | (((uint64_t) hi) << 32);
}

void square(uint32_t *buf, uint32_t color, unsigned x, unsigned y, unsigned width, unsigned height, unsigned stride) {
  for (unsigned yy = y; yy < y + height; ++yy)
    for (unsigned xx = x; xx < x + width; ++xx)
      buf[yy * stride + xx] = color;
}



void deleteEnemy( int enArr[], int n, int *num, int x, int y, int w2, int h2, int w){
		
	for( int i = 0; i < n ; i+=2){
     
    	if( abs(x-enArr[i]) < w2 &&  abs(y-enArr[i+1]) < h2 && enArr[i] != -100 && enArr[i+1] != -100){
        	enArr[i] = -100;
        	enArr[i+1] = -100;
		*num -= 2;
    	}
    }
}

void moveEnemies( int enArr [], int n, int delta, int x, int y, int w2, int h2, int width, int height ){

	for( int i = 0; i < n; i+=2)
	   if(enArr[i] != -100){
		int direction = (uint32_t) rdtsc()%100;
         	if( direction <= 50){
			if(enArr[i] < x)
                	enArr[i]+= delta;
			if(enArr[i] > x)
			enArr[i] -= delta;
			if( enArr[i] + w2 > width) enArr[i] = width-w2;
			if( enArr[i] < 0 ) enArr[i] = 0;
		}
         	else if(direction > 50){
			if(enArr[i+1] < y)
                	enArr[i+1]+= delta;
			if(enArr[i+1] > y)
			enArr[i+1] -= delta;
			if( enArr[i+1] < 0) enArr[i+1] = 0;
			if( enArr[i+1] + h2 > height) enArr[i+1] = height-h2;
         	}
	   }
}


EFI_STATUS EFIAPI efi_main(EFI_HANDLE img, EFI_SYSTEM_TABLE *systab) {
  InitializeLib(img, systab);

  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
 // EFI_SIMPLE_POINTER_PROTOCOL *mouse = 0;

  EFI_STATUS status = LibLocateProtocol(&GraphicsOutputProtocol, (void**)&gop);
  CHECK(status, TRUE);

  int width = gop->Mode->Info->HorizontalResolution;
  int height = gop->Mode->Info->VerticalResolution-100;
 // gST->ConOut->EnableCursor(gST->ConOut,TRUE);
  if (gop->Mode->Info->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) {
    Print(L"Error: don't know about pixel format %d\r\n", gop->Mode->Info->PixelFormat);
    Exit(EFI_SUCCESS, 0, NULL);
  }

  uint64_t size = width * height * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  uint32_t *buffer = AllocateZeroPool(size);

  int x = (uint32_t) rdtsc() % width;
  int y = 450;

  int w1 = 25;
  int h1 = 25;

  int w2 = 25;
  int h2 = 25;

  int delta = 40;

  int level = 1;
  int lifes = 3;
   
  int n = 0;
  while(level <= 3 && lifes > 0){
  if(level == 1)
   n = 8;
  if (level == 2)
   n = 12;
  else if (level == 3)
   n = 20;

  int enemies[n];
  int num = n;

  for( int i=0; i< n; ++i)
	enemies[i] = 0;

  for( int i = 0; i< n; i+=2){
        enemies[i] = (uint32_t) rdtsc()%750;
        enemies[i+1] = (uint32_t) rdtsc()%400;
}
  


      
  while (num!=0) {

    moveEnemies(enemies,n,10,x,y,w2,h2,width,height);
  
  // background
    square(buffer, color(0xf4, 0x71, 0x42), 0, 0, width, height, width);
  // player
    square(buffer, color(0x59, 0xf4, 0x42), x, y, w1, h1, width);
   
  // enemies
    for( int i = 0 ; i< n; i+=2)
       if(enemies[i] != -100 && enemies[i+1] != -100)
	  square(buffer, color(0, 0, 0xff), enemies[i], enemies[i+1], w2, h2, width);
	
    status = uefi_call_wrapper(gop->Blt, 10, gop, buffer, EfiBltBufferToVideo,0,0,0,0, width, height, 0);
    CHECK(status, FALSE);
    WaitForSingleEvent(ST->ConIn->WaitForKey, 0);
    EFI_INPUT_KEY key;
    uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);

    if (key.UnicodeChar == 'w') y -= delta;
    if (key.UnicodeChar == 'a') x -= delta;
    if (key.UnicodeChar == 's') y += delta;
    if (key.UnicodeChar == 'd') x += delta;
    if (key.UnicodeChar == 'm') deleteEnemy(enemies,n,&num,x,y,w2,h2,width);    

    if (y < 0) y = 0;
    if (y + h1 > height) y = height-h1;
    if (x < 0) x = 0;
    if (x + w1 > width) x = width-w1;

    if(num==0)
	level+=1;
  }
  }

  Print(L"press any key to exit...\r\n");
  WaitForSingleEvent(systab->ConIn->WaitForKey, 0);
  
  return EFI_SUCCESS;
}

