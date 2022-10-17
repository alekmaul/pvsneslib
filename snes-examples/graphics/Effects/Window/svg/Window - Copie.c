/*---------------------------------------------------------------------------------


    Simple window effect in mode 1 
    -- alekmaul


---------------------------------------------------------------------------------*/
#include <snes.h>
#include <string.h>

extern char patterns, patterns_end;
extern char palette, palette_end;
extern char map, map_end;

extern const unsigned char hdmaGradientList[];

extern calc_circle_hdma(u16 x, u16 y, u16 radius, u8 *tablevalL, u8 *tablevalR);

u8 pada,padb,padu,padd,padl,padr;
u16 pad0;
u8 rr, x0, y0;
	
u8 varlefttriangle[SCREEN_HEIGHT+1],varrighttriangle[SCREEN_HEIGHT+1],circletriangle[SCREEN_HEIGHT+1];


// NOTE, if the # of scanlines has the upper bit $80 set
// it indicates a number of single scanline waits	
const u8 tablelefttriangle[]=
{
    // Disable window for 62 scanlines
    62, 0xff, 

    // repeat mode, 50 scanlines
        //    y = r - i - 1
        //    offset = math.sqrt(r * r - y *y) * x_scale
        //    left = round(cx - offset)
        //    right = round(cx + offset)
    0x80 | 50,

//122, 120, 118, 117, 115, 114, 113, 112, 111, 110, 109, 108, 107, 107, 106, 105, 104, 104, 103, 103, 102, 101, 101, 100, 100, 99, 99, 98, 98, 97, 97, 96, 96, 96, 95, 95, 94, 94, 94, 93, 93, 93, 92, 92, 92, 91, 91, 91, 90, 90,

	119, 116, 113, 111, 109, 107, 106, 104, 103, 102, 
	101, 100, 99, 98, 97, 96, 95, 94, 94, 93, 
	92, 92, 91, 91, 90, 90, 89, 89, 88, 88, 
	88, 87, 87, 87, 86, 86, 86, 86, 85, 85, 
	85, 85, 85, 85, 84, 84, 84, 84, 84, 84,
	1,0xff,
	0
/*

	 //20, 0xff, 	// first we wait 60 scanlines
	 0xc0, 		// 192-128 = 64 lines of single entries
	 
	 0x7f,	 0x7e,	 0x7d,	 0x7c,	 0x7b,	 0x7a,	 0x79,	 0x78,	 0x77,	 0x76,	 0x75,	 0x74,	 0x73,	 0x72,	 0x71,	 0x70,
	 0x6f,	 0x6e,	 0x6d,	 0x6c,	 0x6b,	 0x6a,	 0x69,	 0x68,	 0x67,	 0x66,	 0x65,	 0x64,	 0x63,	 0x62,	 0x61,	 0x60,

	 0x61,	 0x62,	 0x63,	 0x64,	 0x65,	 0x66,	 0x67,	 0x68,	 0x69,	 0x6a,	 0x6b,	 0x6c,	 0x6d,	 0x6e,	 0x6f,	 0x70,
	 0x71,	 0x72,	 0x73,	 0x74,	 0x75,	 0x76,	 0x77,	 0x78,	 0x79,	 0x7a,	 0x7b,	 0x7c,	 0x7d,	 0x7e,	 0x7f,	 0xff,
	 0*/
};

const u8 tablerighttriangle[]=
{
    // Disable window for 62 scanlines
    62, 0,

    // repeat mode, 50 scanlines
    0x80 | 50,
//134, 136, 138, 139, 141, 142, 143, 144, 145, 146, 147, 148, 149, 149, 150, 151, 152, 152, 153, 153, 154, 155, 155, 156, 156, 157, 157, 158, 158, 159, 159, 160, 160, 160, 161, 161, 162, 162, 162, 163, 163, 163, 164, 164, 164, 165, 165, 165, 166, 166,


	137, 140, 143, 145, 147, 149, 150, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 162, 163, 164, 164, 165, 165, 166, 166, 167, 167, 168, 168, 168, 169, 169, 169, 170, 170, 170, 170, 171, 171, 171, 171, 171, 171, 172, 172, 172, 172, 172, 172,
	
	1,0,
	0
/*
   //20, 0x00,
   0xc0, 			// 64 lines of single entries
   
   0x81,   0x82,   0x83,   0x84,   0x85,   0x86,   0x87,   0x88,   0x89,   0x8a,   0x8b,   0x8c,   0x8d,   0x8e,   0x8f,   0x90,
   0x91,   0x92,   0x93,   0x94,   0x95,   0x96,   0x97,   0x98,   0x99,   0x9a,   0x9b,   0x9c,   0x9d,   0x9e,   0x9f,   0xa0,

   0x9f,   0x9e,   0x9d,   0x9c,   0x9b,   0x9a,   0x99,   0x98,   0x97,   0x96,   0x95,   0x94,   0x93,   0x92,   0x91,   0x90,
   0x8f,   0x8e,   0x8d,   0x8c,   0x8b,   0x8a,   0x89,   0x88,   0x87,   0x86,   0x85,   0x84,   0x83,   0x82,   0x81,   0x00,
   0	*/

};

#define IN_RANGE(x, min, max)   ( (x) >= (min) && (x) < (max) )

u8 clamp(u8 x, u8 min, u8 max) {	
	u8 val;
	
	if (x>=max) val=(max-1);
	else if (x<min) val=min;
	else val=x;
	return val;	
}

/*
#define plot(x, y) put_pixel_clip(img, x, y, r, g, b)

void raster_circle(
        image img,
        unsigned int x0,
        unsigned int y0,
        unsigned int radius,
        color_component r,
        color_component g,
        color_component b )
{
    int f = 1 - radius;
    int ddF_x = 0;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    plot(x0, y0 + radius);
    plot(x0, y0 - radius);
    plot(x0 + radius, y0);
    plot(x0 - radius, y0);

    while(x < y) 
    {
        if(f >= 0) 
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x + 1;    
        plot(x0 + x, y0 + y);
        plot(x0 - x, y0 + y);
        plot(x0 + x, y0 - y);
        plot(x0 - x, y0 - y);
        plot(x0 + y, y0 + x);
        plot(x0 - y, y0 + x);
        plot(x0 + y, y0 - x);
        plot(x0 - y, y0 - x);
    }
}
#undef plot
*/


/*
void win_circle(u16 winh[], int x0, int y0, int rr)
    int x=0, y= rr, d= 1-rr;
    u32 tmp;

    // clear the whole array first.
    memset16(winh, 0, SCREEN_HEIGHT+1);
    while(y >= x)
    {
        // Side octs
        tmp  = clamp(x0+y, 0, SCREEN_WIDTH);
        tmp += clamp(x0-y, 0, SCREEN_WIDTH)<<8;
        if(IN_RANGE(y0-x, 0, SCREEN_HEIGHT))       // o4, o7
            winh[y0-x]= tmp;
        if(IN_RANGE(y0+x, 0, SCREEN_HEIGHT))       // o0, o3
            winh[y0+x]= tmp;

        // Change in y: top/bottom octs
        if(d >= 0)      
        {
            tmp  = clamp(x0+x, 0, SCREEN_WIDTH);
            tmp += clamp(x0-x, 0, SCREEN_WIDTH)<<8;
            if(IN_RANGE(y0-y, 0, SCREEN_HEIGHT))   // o5, o6
                winh[y0-y]= tmp;
            if(IN_RANGE(y0+y, 0, SCREEN_HEIGHT))   // o1, o2
                winh[y0+y]= tmp;
            d -= 2*(--y);
        }
        d += 2*(x++)+3;
    }
*/
//---------------------------------------------------------------------------------
void calc_triangle(u8 x1, u8 y1, u8 rad) {
    u8 x,y;
	s8 d;
	u8 tmp;
	u8 maxl,minl;
	
	consoleNocashMessage("calc_triangle\r\n");
	
	x=0; y= rad; d= 1-rad;
	maxl=0; minl=0;
    // clear the whole array first.
    memset(circletriangle, 0x00, SCREEN_HEIGHT+1);

    while(y >= x)
    {
	    tmp = clamp(x1-y, 0, SCREEN_WIDTH/2);
        if(IN_RANGE(y1-x, 0, SCREEN_HEIGHT)) { circletriangle[y1-x]= tmp;  consoleNocashMessage("y1-x %04d val=%04d\r\n",(u16) (y1-x),(u16) tmp); }
        if(IN_RANGE(y1+x, 0, SCREEN_HEIGHT)) { circletriangle[y1+x]= tmp;  consoleNocashMessage("y1+x %04d val=%04d\r\n",(u16) (y1+x),(u16) tmp); } 
		if (maxl<y1+x) maxl=y1+x;
		if (minl<y1-x) minl=y1-x;
		
	    // Change in y: top/bottom octs
		if(d >= 0)      
        {
		    tmp = clamp(x1-x, 0, SCREEN_WIDTH/2);
			if(IN_RANGE(y1-y, 0, SCREEN_HEIGHT)) { circletriangle[y1-y]= tmp;  consoleNocashMessage("y1-y val=%04d\r\n",(u16) tmp); }
            if(IN_RANGE(y1+y, 0, SCREEN_HEIGHT))  { circletriangle[y1+y]= tmp;  consoleNocashMessage("y1+y val=%04d\r\n",(u16) tmp); }
			if (maxl<y1+y) maxl=y1+y;
			if (minl<y1-y) minl=y1-y;
            d -= 2*(y);
			y--;
		}
        d += 2*(x)+3;
		x++;
/*
	    tmp = clamp(x1+y, 0, SCREEN_WIDTH/2);
        if(IN_RANGE(y1-x, 0, SCREEN_HEIGHT)) { circletriangle[y1-x]= SCREEN_WIDTH-tmp;  consoleNocashMessage("y1-x %04d val=%04d\r\n",(u16) (y1-x),(u16) tmp); }
        if(IN_RANGE(y1+x, 0, SCREEN_HEIGHT)) { circletriangle[y1+x]= SCREEN_WIDTH-tmp;  consoleNocashMessage("y1+x %04d val=%04d\r\n",(u16) (y1+x),(u16) tmp); } 
		if (maxl<y1+x) maxl=y1+x;
		if (minl<y1-x) minl=y1-x;
		
	    // Change in y: top/bottom octs
		if(d >= 0)      
        {
		    tmp = clamp(x1-x, 0, SCREEN_WIDTH/2);
			if(IN_RANGE(y1-y, 0, SCREEN_HEIGHT)) { circletriangle[y1-y]= SCREEN_WIDTH-tmp;  consoleNocashMessage("y1-y val=%04d\r\n",(u16) tmp); }
            if(IN_RANGE(y1+y, 0, SCREEN_HEIGHT))  { circletriangle[y1+y]= -SCREEN_WIDTHtmp;  consoleNocashMessage("y1+y val=%04d\r\n",(u16) tmp); }
			if (maxl<y1+y) maxl=y1+y;
			if (minl<y1-y) minl=y1-y;
            d -= 2*(y);
			y--;
		}
        d += 2*(x)+3;
		x++;
	*/
	}
	consoleNocashMessage("calc_triangle min=%04d max=%04d\r\n",(u16) minl,(u16) maxl);
	varlefttriangle[0]=y1;
	varrighttriangle[0]=y1;
	varlefttriangle[1]=0xFF;
	varrighttriangle[1]=0x00;
	varlefttriangle[2]=(maxl-minl) | 0x80;
	varrighttriangle[2]=(maxl-minl) | 0x80;
	for (tmp=3;tmp<(maxl-minl);tmp++) {
		varlefttriangle[tmp]=circletriangle[tmp+minl-1];
		varrighttriangle[tmp]=SCREEN_WIDTH-circletriangle[tmp+minl-1];
		consoleNocashMessage("val %04d=%04d %04d\r\n",(u16) tmp,(u16) varlefttriangle[tmp],(u16) varrighttriangle[tmp]);
	}
	varlefttriangle[tmp]=0;
	varrighttriangle[tmp]=0;
}

// babylonian method for square roots
u16 sqrt(u16 n)
{
  // double a = (eventually the main method will plug values into a)
  u16 a = n;
  u16 x = 1;
  u16 i;
 
  // For loop to get the square root value of the entered number.
  for( i = 0; i < n; i++)
  {
    x = ( x+a / x )/2;
  }
 
  return x;
}

/*
for (int x = -radius; x < radius ; x++)
{
    int hh = (int)std::sqrt(radius_sqr - x * x);
    int rx = center_x + x;
    int ph = center_y + hh;

    for (int y = center_y-hh; y < ph; y++)
        canvas[rx][y] = 1;
}
*/

//---------------------------------------------------------------------------------
void calc_triangle2(u8 x1, u8 y1, u8 rad) {
	u8 i,j;
	u16 sq;
	
	consoleNocashMessage("calc_triangle\r\n");
	memcpy((u8 *) &varlefttriangle, &tablelefttriangle,sizeof(tablelefttriangle));
	//memcpy((u8 *) &varrighttriangle, &tablerighttriangle,sizeof(tablerighttriangle));
	
	// change nb scanlines regarding y1
	if (y1-rad>1) {
		varlefttriangle[0]=y1-rad;
		varrighttriangle[0]=y1-rad;
	}
	else {
		varlefttriangle[0]=1;
		varrighttriangle[0]=1;
	}
	
	j=rad-96;

			consoleNocashMessage(" j=%04d\r\n",(u16) j);
	for (i=0+3;i<50+3;i++) {

		varlefttriangle[i]=varlefttriangle[i]-j;
		varrighttriangle[i]=256-varlefttriangle[i];
	}
	
	/*
	for (i=0;i<rad;i++) {
		consoleNocashMessage(" r - i - 1=%04d\r\n",(u16) ( rad - i - 1));
		sq=sqrt( rad*rad);
		consoleNocashMessage(" sqrt=%04d\r\n",(u16) sq);

        //    y = r - i - 1
        //    offset = math.sqrt(r * r - y *y) * x_scale
        //    left = round(cx - offset)
        //    right = round(cx + offset)
	}
	*/
}

//---------------------------------------------------------------------------------
int main(void)
{
    // Initialize SNES
    consoleInit();

    // Copy tiles to VRAM
    bgInitTileSet(0, &patterns, &palette, 0, (&patterns_end - &patterns), (&palette_end - &palette), BG_16COLORS, 0x4000);

    // Copy Map to VRAM
    bgInitMapSet(0, &map, (&map_end - &map), SC_32x32, 0x0000);

    // Now Put in 16 color mode and disable other BGs except 1st one
    setMode(BG_MODE1, 0);
    bgSetDisable(1);
    bgSetDisable(2);
    setScreenOn();

	// Initialize the window effect on bg1
	pada=0;padb=0;padu=0;padd=0;padl=0;padr=0;
	rr=96; x0=128; y0=128;
	//calc_triangle2(x0, y0, rr);
	calc_circle_hdma(x0, y0, rr, (u8 *) &varlefttriangle, (u8 *) &varrighttriangle);
									
	setModeHdmaWindow(MSWIN_BG1, MSWIN1_BG1MSKENABLE | MSWIN1_BG1MSKOUT, (u8 *) &varlefttriangle, (u8 *) &varrighttriangle);
	//setModeHdmaWindow(MSWIN_BG1, MSWIN1_BG1MSKENABLE | MSWIN1_BG1MSKOUT, (u8 *) &tablelefttriangle, (u8 *) &tablerighttriangle);

    // Wait for nothing :P
    while (1)
    {
        // Get current #0 pad
        pad0 = padsCurrent(0);

		// move with pad
		if (pad0 & KEY_UP) {
			if (y0>2) y0-=2;
			calc_circle_hdma(x0, y0, rr, (u8 *) &varlefttriangle, (u8 *) &varrighttriangle);
		}
		if (pad0 & KEY_DOWN) {
			if (y0<222-rr) y0+=2;
			calc_circle_hdma(x0, y0, rr, (u8 *) &varlefttriangle, (u8 *) &varrighttriangle);
		}

		// enlarge with key a
		if (pad0 & KEY_A) {
			if (!pada) {
				pada=1;
				if (rr<96) rr+=4;
				// Calculate new losange & affect window
				calc_circle_hdma(x0, y0, rr, (u8 *) &varlefttriangle, (u8 *) &varrighttriangle);
			}
		}
		else pada=0;

		// deenlarge with key b
		if (pad0 & KEY_B) {
			if (!padb) {
				padb=1;
				if (rr>0) rr-=4;
				// Calculate new losange & affect window
				calc_circle_hdma(x0, y0, rr, (u8 *) &varlefttriangle, (u8 *) &varrighttriangle);
			}
		}
		else padb=0;

		// Wait vblank sync
        WaitForVBlank();
    }
    return 0;
}