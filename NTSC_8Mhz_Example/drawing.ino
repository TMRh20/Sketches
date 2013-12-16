


void printChar(const char str[],byte column, int row){
  letter(str,column,row);
}

void printChar( unsigned long num, int column, byte row){
     String str;
  char dat[7];
  str = String(num);
  str.toCharArray(dat,7);
  printChar(dat, column, row); 
}

void letter(const char *str, int column,byte row){
  unsigned int cursr = 0;
 while(*str){
    letter(*str++,column,row); 
    column+=8;
 }
}

void letter(char dat, int posY, int posX){
  //int tmp = 184; //width

  unsigned int y;
  y = posX * wLDiv;
  y = (posY / 8) + y;
  for(byte i=0; i<7; i++){
      byte tmp = pgm_read_word_near(ascii[dat] + i);
      data[y+(i*wLDiv) ] = tmp;
  }
  
}



void dot(int w, int h){
  
  
  h = h*wLDiv;
  int wByte = h+(w/8);
  byte wBit = 7-(w%8);
  bitSet(data[wByte],wBit);
  
}



void dotCl(int w, int h){
  
  
  h = h*wLDiv;
  int wByte = h+(w/8);
  //byte wBit = 7-(w%8);
  //bitClear(data[wByte],wBit);
  data[wByte] = 0;
}

boolean clearTog = 0;

void clearScreen(){

//
//if(clearTog){
  for(unsigned int i=0; i<totalBytes; i++){
    data[i] = 0;
  }
//}else{
//
//  for(unsigned int i=totalBytes-1; i>0; i--){
//    data[i] = 0b00000000;
//  }
//}
//
//clearTog = !clearTog;
}


//void clearLine(byte *byteNo, byte *bitNo){
//  
//
//  for(int i=0;i< displayHeight;i++){
//    if( bitRead(data[i][*byteNo],*bitNo) ) {
//      bitClear(data[i][*byteNo], *bitNo);
//      return; 
//    }
//  }
//  
//}


boolean shiftedBit = 0;

void shiftRight(){
  byte first, second;
  
  for(int i = 0; i < totalBytes; i+=wLDiv){
    for(unsigned int j=0; j<wLDiv; j++){

      boolean Bit = bitRead(data[i+j],0);    
      data[i+j] = data[i+j] >> 1;
      if(j < wLDiv-1){ 
        bitWrite(data[i+j],7,shiftedBit);
      }else{        
        bitWrite(data[i],7,shiftedBit);
      }
      shiftedBit = Bit;
    }
  
  }
}


byte prevByte = 0;

void shiftDown(){
  
  prevByte = data[totalBytes-wLDiv];
  for(int j=0; j<wLDiv; j++ ){
  for(int i= 0; i < totalBytes; i+=wLDiv){
    
    byte tmpByte = data[i+j];
    if(i < totalBytes - wLDiv){
      data[i+j] = prevByte;
    }else{
      data[j]=prevByte;
    }
    prevByte = tmpByte;
  }
  //prevByte = 0;
}
  
  
}

void set_pixel(uint16_t x, uint8_t y, char c) {
	if (x >= displayWidth*8 || y >= displayHeight)
		return;
	sp(x,y,c);
        //dot(x,y);
}

static void inline sp(uint16_t x, uint8_t y, char c) {
	if (c==1)
		data[(x/8) + (y*wLDiv)] |= 0x80 >> (x&7);
	else if (c==0)
		data[(x/8) + (y*wLDiv)] &= ~0x80 >> (x&7);
	else
		data[(x/8) + (y*wLDiv)] ^= 0x80 >> (x&7);
}


void draw_line(int x0, uint8_t y0, int x1, uint8_t y1, char c) {

	if (x0 >= displayWidth*8 || y0 >= displayHeight-1 || x1 >= displayWidth*8 || y1 >= displayHeight-1)
		return;
	if (x0 == x1){
		draw_column(x0,y0,y1,c);
	}else if (y0 == y1){
		draw_row(y0,x0,x1,c);
	}else {
		int e;
		signed int dx,dy,j, temp;
		signed int s1,s2, xchange;
		signed int x,y;

		x = x0;
		y = y0;
	
		//take absolute value
		if (x1 < x0) {
			dx = x0 - x1;
			s1 = -1;
		}
		else if (x1 == x0) {
			dx = 0;
			s1 = 0;
		}
		else {
			dx = x1 - x0;
			s1 = 1;
		}

		if (y1 < y0) {
			dy = y0 - y1;
			s2 = -1;
		}
		else if (y1 == y0) {
			dy = 0;
			s2 = 0;
		}
		else {
			dy = y1 - y0;
			s2 = 1;
		}

		xchange = 0;   

		if (dy>dx) {
			temp = dx;
			dx = dy;
			dy = temp;
			xchange = 1;
		} 

		e = ((int)dy<<1) - dx;  
	 
		for (j=0; j<=dx; j++) {
			sp(x,y,c);
                        //dot(x,y);
		 
			if (e>=0) {
				if (xchange==1) x = x + s1;
				else y = y + s2;
				e = e - ((int)dx<<1);
			}
			if (xchange==1)
				y = y + s2;
			else
				x = x + s1;
			e = e + ((int)dy<<1);
		}
	}
}

void draw_column(uint16_t row, uint16_t y0, uint16_t y1, uint8_t c) {

	unsigned char bit;
	int byte;
	
	if (y0 == y1)
		set_pixel(row,y0,c);
	else {
		if (y1 < y0) {
			bit = y0;
			y0 = y1;
			y1 = bit;
		}
		bit = 0x80 >> (row&7);
		byte = row/8 + y0*wLDiv;
		if (c == WHITE) {
			while ( y0 <= y1) {
				data[byte] |= bit;
				byte += wLDiv;
				y0++;
			}
		}
		else if (c == 0) {
			while ( y0 <= y1) {
				data[byte] &= ~bit;
				byte += wLDiv;
				y0++;
			}
		}
		else if (c == 2) {
			while ( y0 <= y1) {
				data[byte] ^= bit;
				byte += wLDiv;
				y0++;
			}
		}
	}
}


void draw_rect(uint8_t x0, uint8_t y0, uint8_t w, uint8_t h, char c, char fc) {
	
	if (fc != -1) {
		for (unsigned char i = y0; i < y0+h; i++)
			draw_row(i,x0,x0+w,fc);
	}
	draw_line(x0,y0,x0+w,y0,c);
	draw_line(x0,y0,x0,y0+h,c);
	draw_line(x0+w,y0,x0+w,y0+h,c);
	draw_line(x0,y0+h,x0+w,y0+h,c);
}

void draw_row(uint8_t line, uint16_t x0, uint16_t x1, uint8_t c) {
	uint8_t lbit, rbit;
	
	if (x0 == x1)
		set_pixel(x0,line,c);
	else {
		if (x0 > x1) {
			lbit = x0;
			x0 = x1;
			x1 = lbit;
		}
		lbit = 0xff >> (x0&7);
		x0 = x0/8 + wLDiv*line;
		rbit = ~(0xff >> (x1&7));
		x1 = x1/8 + wLDiv*line;
		if (x0 == x1) {
			lbit = lbit & rbit;
			rbit = 0;
		}
		if (c == 1) {
			data[x0++] |= lbit;
			while (x0 < x1)
				data[x0++] = 0xff;
			data[x0] |= rbit;
		}
		else if (c == 0) {
			data[x0++] &= ~lbit;
			while (x0 < x1)
				data[x0++] = 0;
			data[x0] &= ~rbit;
		}
		else if (c == 2) {
			data[x0++] ^= lbit;
			while (x0 < x1)
				data[x0++] ^= 0xff;
			data[x0] ^= rbit;
		}
	}
} 



void shift(uint8_t distance, uint8_t direction) {
	uint8_t * src;
	uint8_t * dst;
	uint8_t * end;
	uint8_t shift;
	uint8_t tmp;
	switch(direction) {
		case 0://UP:
			dst = data;
			src = data + distance*wLDiv;
			end = data + displayHeight*wLDiv;
				
			while (src <= end) {
				*dst = *src;
				*src = 0;
				dst++;
				src++;
			}
			break;
		case 1://DOWN:
			dst = data + displayHeight*wLDiv;
			src = dst - distance*wLDiv;
			end = data;
				
			while (src >= end) {
				*dst = *src;
				*src = 0;
				dst--;
				src--;
			}
			break;
		case 2://LEFT:
			shift = distance & 7;
			
			for (uint8_t line = 0; line < displayHeight; line++) {
				dst = data + wLDiv*line;
				src = dst + distance/8;
				end = dst + wLDiv-2;
				while (src <= end) {
					tmp = 0;
					tmp = *src << shift;
					*src = 0;
					src++;
					tmp |= *src >> (8 - shift);
					*dst = tmp;
					dst++;
				}
				tmp = 0;
				tmp = *src << shift;
				*src = 0;
				*dst = tmp;
			}
			break;
		case 3://RIGHT:
			shift = distance & 7;
			
			for (uint8_t line = 0; line < displayHeight; line++) {
				dst = data + wLDiv-1 + wLDiv*line;
				src = dst - distance/8;
				end = dst - wLDiv+2;
				while (src >= end) {
					tmp = 0;
					tmp = *src >> shift;
					*src = 0;
					src--;
					tmp |= *src << (8 - shift);
					*dst = tmp;
					dst--;
				}
				tmp = 0;
				tmp = *src >> shift;
				*src = 0;
				*dst = tmp;
			}
			break;
	}
}




void fill(uint8_t color) {
	switch(color) {
		case 0:
			//cursor_x = 0;
			//cursor_y = 0;
			for (int i = 0; i < totalBytes; i++)
				data[i] = 0;
			break;
		case 1:
			//cursor_x = 0;
			//cursor_y = 0;
			for (int i = 0; i < totalBytes; i++)
				data[i] = 0xFF;
			break;
		case 2:
			for (int i = 0; i < totalBytes; i++)
				data[i] = ~data[i];
			break;
	}
}
