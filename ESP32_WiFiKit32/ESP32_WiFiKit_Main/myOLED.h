

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, 16, 15, 4);  // [page buffer, size = 128 bytes]

char charBuffer1[21] = {"This is not a test"};
char charBuffer2[21] = {"This is just a test"};
char charBuffer3[21] = {"Ok maybe..."};
char charBuffer4[21] = {"Testing, testing.."};
char charBuffer5[21] = {"123..."};

const char *charPtr[5];

// This is a simple function that allows me to just send data
// to u8g2 lib and specify the line # to display it on
void printString(char *cString, uint8_t sLength, uint8_t lineNo){  
  if(lineNo > 0){
    memcpy((char*)charPtr[lineNo-1], cString, ((sLength <= 20) ? sLength : 20)  );    
  }
}
