#include <efi.h>
#include <efilib.h>
#include <stdlib.h>
#include <stdio.h>


#define WIDE2(x) L##x
#define WIDE1(x) WIDE2(x)
#define WFILE WIDE1(__FILE__)
#define MAX_BUFFER_SIZE 1024*1024

#define CHECK(rc, crit) { \
  if (EFI_ERROR(rc)) { \
    CHAR16 Buffer[64]; \
    StatusToString(Buffer, rc); \
    Print(L"[%s:%d] %s (code %d)\r\n", WFILE, __LINE__, Buffer, rc); \
    if (crit) Exit(rc, 0, NULL); \
  } \
}


uint32_t color(uint8_t red, uint8_t green, uint8_t blue) {
  return (blue << 24) | (green << 16) | (red << 8) | 0xFF;

}
static inline uint8_t inb(uint16_t p)
{
    uint8_t r;
    __asm__("inb %1, %0" : "=a" (r) : "dN" (p));
    return r;
}

static inline void outb(uint16_t p, uint8_t d)
{
    __asm__("outb %1, %0" : : "dN" (p), "a" (d));
}
static uint8_t rtcs(){
    uint8_t last = 0, sec;
    do { /* until value is the same twice in a row */
        /* wait for update not in progress */
        do { outb(0x70, 0x0A); } while (inb(0x71) & 0x80);
        outb(0x70, 0x00);
        sec = inb(0x71);
    } while (sec != last && (last = sec));
    return sec;

}

static inline uint64_t rdtsc()
{
    uint32_t hi, lo;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t) lo) | (((uint64_t) hi) << 32);
}
uint64_t tpms;
static void tps()
{
    static uint64_t ti = 0;
    static uint8_t last_sec = 0xFF;
    uint8_t sec = rtcs();
    if (sec != last_sec) {
        last_sec = sec;
        uint64_t tf = rdtsc();
        tpms = (uint32_t) ((tf - ti) >> 3) / 125;
        ti = tf;
    }
}

uint64_t timer = 0;

static int interval(uint32_t ms){
	uint64_t tf = rdtsc();
	if( tf - timer >= tpms*ms){ timer = tf; return 1;}
	else return 0;
}

  int w1 = 25;
  int h1 = 25;

  int w2 = 25;
  int h2 = 25;

  int w3 = 35;
  int h3 = 35;

  int w4 = 5;
  int h4 = 5;

  int xinit;
  int yinit;

void square(uint32_t *buf, uint32_t color, unsigned x, unsigned y, unsigned width, unsigned height, unsigned stride) {
  for (unsigned yy = y; yy < y + height; ++yy)
    for (unsigned xx = x; xx < x + width; ++xx)
      buf[yy * stride + xx] = color;
}


void deleteEnemy( int enArr[], int n, int *num, int x, int y, int w2, int h2){

	for( int i = 0; i < n ; i+=2){

    	if( abs(x-enArr[i]) < w2 &&  abs(y-enArr[i+1]) < h2 && enArr[i] != -100 && enArr[i+1] != -100){
        	enArr[i] = -100;
        	enArr[i+1] = -100;
		*num -= 2;
    	}
    }
}

void blockCollision( int blocks[], int *en1, int *en2, int nBl, int delta, int w3, int h3, int w2, int h2, int flag, int isBullet, int *nmr){
	for( int i = 0; i < nBl; i+=2){

	   if(*en2+h2 >= blocks[i+1] && *en2 <= blocks[i+1]+h3 && flag == 0){

		// right side
		if( *en1 - blocks[i] <= w3 && \
		    *en1 - blocks[i] >= 0 && \
		    *en1 + delta >= blocks[i]+w3){
		if(isBullet == 1){ *en1 = -100; *en2 = -100; *nmr-=1;}
		else *en1 = blocks[i]+w3+1;
		}

		// left side
		if( blocks[i] - *en1 <= w2 && \
		    blocks[i] - *en1 >= 0 && \
		    *en1+w2 - delta <= blocks[i]){
		if(isBullet == 1){ *en1 = -100; *en2 = -100; *nmr -= 1;}
		else *en1 = blocks[i]-w2-1;
		}

	   }
	   if( *en1 <= blocks[i]+w3 && *en1 + w2 >= blocks[i] && flag == 1){
		// bottom side
		if( *en2 - blocks[i+1] <= h3 && \
                    *en2 - blocks[i+1] >= 0 && \
		    *en2 + delta >= blocks[i+1] + h3){
		if(isBullet == 1){ *en1 = -100; *en2 = -100; *nmr -= 1;}
 	        else  *en2 = blocks[i+1]+h3+1;
		}
		// upper side
		if( blocks[i+1] - *en2 <= h2 && \
                    blocks[i+1] - *en2 >= 0 && \
		    *en2 + h2 - delta <= blocks[i+1]){
		if(isBullet == 1){ *en1 = -100; *en2 = -100; *nmr -= 1;}
                else *en2 = blocks[i+1]-h2-1;
		}
	   }
	}
}

void moveEnemies( int enArr [], int blocks[], int nEn, int nBl, int delta, int x, int y, int w3, int h3, int w2, int h2, int width, int height, int isBullet, int *nmr){

	for( int i = 0; i < nEn; i+=2)
	   if(enArr[i] != -100){
		int flag = -1;
		int direction = (uint32_t) rdtsc()%100;
         	if( direction <= 50){
			flag = 0;
			if(enArr[i] < x)
                	enArr[i]+= delta;
			if(enArr[i] > x)
			enArr[i] -= delta;
			if( enArr[i] + w2 > width) enArr[i] = width-w2;
			if( enArr[i] < 0 ) enArr[i] = 0;
		}
         	else if(direction > 50){
			flag = 1;
			if(enArr[i+1] < y)
                	enArr[i+1]+= delta;
			if(enArr[i+1] > y)
			enArr[i+1] -= delta;
			if( enArr[i+1] < 0) enArr[i+1] = 0;
			if( enArr[i+1] + h2 > height) enArr[i+1] = height-h2;
         	}
		blockCollision(blocks,&enArr[i],&enArr[i+1],nBl,delta,w3,h3,w2,h2,flag,isBullet,nmr);
	  }
}

void generateBlocks( int arr[], int n, int x, int y, int width, int height, int w, int h){
	for(int i = 0; i < n; i+=2){
		arr[i] = (uint32_t) rdtsc()%(width-w);
		arr[i+1] = (uint32_t) rdtsc()%(height-h);;
		if(abs(arr[i]-x) < w && abs(arr[i+1]-y) < h){
        	generateBlocks(arr,n,x,y,width,height,w,h);
        }}
	return;
}

void generateEnemies( int enArr[], int nEn, int blArr[], int nBl, int x, int y, int w, int h, int width, int height){
	generateBlocks(enArr,nEn,x,y, width, height,w,h);
	for(int i = 0; i < nEn; i+=2){
		for(int j = 0; j < nBl; j+=2)
		if( abs(enArr[i]-blArr[j]) < w && \
		    abs(enArr[i+1]-blArr[j+1]) < h)
		  generateEnemies(enArr,nEn,blArr,nBl,x,y,w,h, width, height);
	}
	return;
}

void shootBullet( int bullets[], int enArr[], int nEn,int x, int y){
	xinit = x;
	yinit = y;
	for(int i = 0; i< nEn; i+=2)
		if(enArr[i] != -100 && enArr[i+1] != -100){

		if(enArr[i] < x){
			bullets[i] = enArr[i]+w2;
                        if(enArr[i+1] == y) bullets[i+1] = enArr[i+1];
                        if(enArr[i+1] < y) bullets[i+1] = enArr[i+1]+h2;
                        if(enArr[i+1] > y) bullets[i+1] = enArr[i+1];
                }
                else if(enArr[i] == x){
			bullets[i] = enArr[i];
                        if(enArr[i+1] < y) bullets[i+1] = enArr[i+1]+h2;
                        if(enArr[i+1] > y) bullets[i+1] = enArr[i+1]-h2;
                }
                else if(enArr[i] > x){
			bullets[i] = enArr[i]-w4;
                        if(enArr[i+1] == y) bullets[i+1] = enArr[i+1];
                        if(enArr[i+1] < y) bullets[i+1] = enArr[i+1]+h2;
                        if(enArr[i+1] > y) bullets[i+1] = enArr[i+1]-h4;
                }

		}

}

void moveBullets(uint32_t *buffer,int bullets[], int enemies[], int blocks[], int *nmr, int num, int nEn, int nBl, int delta, int width, int height, int x, int y, int *lifes){
	moveEnemies(bullets,blocks,nEn,nBl,delta, xinit,yinit,w3,h3,w4,h4,width,height,1,nmr);
}
void deleteBullet(int bullets[], int enemies[], int nEn, int *nmr, int num, int *lifes, int x, int y, int frames){
	for( int i = 0 ; i < nEn; i+=2){
                if(abs(bullets[i]-xinit) < w1 &&
		   abs(bullets[i+1]-yinit)<h1){
                        if(x != xinit || y != yinit){
                                bullets[i] = -100;
                                bullets[i+1] = -100;
                                *nmr -= 1;
                        }
                }
		if( abs(bullets[i] -x) < w1 && abs(bullets[i+1]-y)<h1){
                        bullets[i] = -100;
                        bullets[i+1] = -100;
                        *nmr -= 1;
                        *lifes -= 5;
                }
        }

}
int countEnemiesRemained( int enemies[], int nEn){
	int count = 0;
	for( int i = 0; i<nEn; i+=2)
		if(enemies[i] != -100 && enemies[i+1] != -100)
			++count;
	return count;
}
EFI_STATUS EFIAPI efi_main(EFI_HANDLE img, EFI_SYSTEM_TABLE *systab) {
  InitializeLib(img, systab);
  int start = 0;
  
  Print(L"Start game?Y/N");
  EFI_INPUT_KEY key1;
  WaitForSingleEvent(ST->ConIn->WaitForKey,0);
  uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key1);
  if(key1.UnicodeChar == 'Y') start = 1;
  while( start == 1){
  EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
  EFI_STATUS status = LibLocateProtocol(&GraphicsOutputProtocol, (void**)&gop);

  CHECK(status, TRUE);

  int width = gop->Mode->Info->HorizontalResolution;
  int height = gop->Mode->Info->VerticalResolution-100;

  if (gop->Mode->Info->PixelFormat != PixelBlueGreenRedReserved8BitPerColor) {
    Print(L"Error: don't know about pixel format %d\r\n", gop->Mode->Info->PixelFormat);
    Exit(EFI_SUCCESS, 0, NULL);
  }

  uint64_t size = width * height * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  uint32_t *buffer = AllocateZeroPool(size);

  int x = (uint32_t) rdtsc() % width;
  int y = (uint32_t) rdtsc() % height;

  int delta = 10;
  int level = 1;
  int lifes = 100;
  float deltaB = 1;

  int nEn = 0;
  int nBl = 0;

  int frames = 0;
  uint32_t itpms;
  tps();
  itpms = tpms; while (tpms == itpms) tps();
  itpms = tpms; while (tpms == itpms) tps();


  while(level <= 3 && lifes > 0){
  
  if(level == 1){
   nEn = 8;
   nBl = 12;
  }
  else if (level == 2){
   nEn = 12;
   nBl = 16;
  }
  else if (level == 3){
   nEn = 20;
   nBl = 26;
  }
  square(buffer, 0x00000000, 0, 0, width, height, width);
  uefi_call_wrapper(gop->Blt, 10, gop, buffer, EfiBltBufferToVideo,0,0,0,0, width, height, 0);
  Print(L"You have completed %d/3 levels. Press Y to continue \n",level-1);
  WaitForSingleEvent(ST->ConIn->WaitForKey,0);
  uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key1);
  if(key1.UnicodeChar == 'Y'){
  int enemies[nEn];
  int blocks[nBl];
  int num = nEn;
  int numB = nEn/2;
  int bullets[nEn];
  tps();
  itpms = tpms; while (tpms == itpms) tps();
  itpms = tpms; while (tpms == itpms) tps();

  generateBlocks(blocks,nBl,x,y,width, height,w3,h3+20);
  generateEnemies(enemies,nEn,blocks,nBl,x,y,w3,h3,width,height);
  shootBullet(bullets,enemies,nEn,x,y);

  while (num!=0 && lifes > 0) {
    tps();
    if(interval(600) == 1) moveEnemies(enemies,blocks,nEn,nBl,10,x,y,w3,h3,w2,h2,width,height,0,&nEn);

    moveBullets(buffer,bullets,enemies,blocks,&numB,num,nEn,nBl,deltaB,width,height,x,y,&lifes);
  // background
    square(buffer, color(0xf4, 0x71, 0x42), 0, 0, width, height, width);
  // enemies
    for( int i = 0 ; i< nEn; i+=2){
       if(bullets[i] != -100 && bullets[i+1] != -100)
          square(buffer, color(0, 200, 0),bullets[i],bullets[i+1],w4,h4,width);
       if(enemies[i] != -100 && enemies[i+1] != -100)
	  square(buffer, color(0, 150, 0), enemies[i], enemies[i+1], w2, h2, width);

    }
    deleteBullet(bullets,enemies,nEn,&numB,num,&lifes,x,y,frames);
    frames++;
    if(numB == 0)
	if(frames%263 == 0){ shootBullet(bullets,enemies,nEn,x,y); numB = num/2;}
  // player
    square(buffer, color(0, 0, 200), x, y, w1, h1, width);
  // blocks
    for( int i = 0 ; i < nBl; i+=2)
	square(buffer,color(0xff,0xff,0xff), blocks[i], blocks[i+1], w3, h3, width);
  // life
	square(buffer,color(150,150,150),width-150,height-20,100,10,width);
	square(buffer,color(0,250,0),width-150,height-20,lifes,10,width);
    status = uefi_call_wrapper(gop->Blt, 10, gop, buffer, EfiBltBufferToVideo,0,0,0,0, width, height, 0);
    CHECK(status, FALSE);
    EFI_INPUT_KEY key;
    uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key);
    int flag = -1;

    if (key.UnicodeChar == 'w'){ y -= delta; flag = 1;}
    if (key.UnicodeChar == 'a'){ x -= delta; flag = 0;}
    if (key.UnicodeChar == 's'){ y += delta; flag = 1;}
    if (key.UnicodeChar == 'd'){ x += delta; flag = 0;}
    if (key.UnicodeChar == 'm') deleteEnemy(enemies,nEn,&num,x,y,w2,h2);

    if (y < 0) y = 0;
    if (y + h1 > height) y = height-h1;
    if (x < 0) x = 0;
    if (x + w1 > width) x = width-w1;
    blockCollision(blocks, &x, &y, nBl,delta,w3,h3,w1,h1, flag,0,&numB);
    if(num==0){
	level += 1;
	lifes = 100;
    }
  }
  }
  }

   square(buffer, 0x00000000, 0, 0, width, height, width);
   uefi_call_wrapper(gop->Blt, 10, gop, buffer, EfiBltBufferToVideo,0,0,0,0,width,height,0);
   if(level == 4) Print(L"GAME OVER \t YOU WIN \t START OVER? Y/N \n");
   else if ( lifes == 0) Print(L"GAME OVER \t YOU LOSE \t TRY AGAIN? Y/N \n");
   WaitForSingleEvent(ST->ConIn->WaitForKey,0);
   uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2, ST->ConIn, &key1);
   if(key1.UnicodeChar == 'Y') start = 1;
   if(key1.UnicodeChar == 'N') start = 0;
  }
  return EFI_SUCCESS;
}

