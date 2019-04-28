#include <iconv.h>

#ifndef ENCODINGUTILS_HPP
#define ENCODINGUTILS_HPP

class EncodingUtils{
    private:
        static const unsigned int UTF8Table_size=22645;
        static const unsigned int UTF8Table[UTF8Table_size];
        static const unsigned int GBKTable_size=22645;
        static const unsigned short GBKTable[GBKTable_size];
        int isLittleEndian;
        
    public:
    
        static const char* ENCODING_UTF8;
        static const char* ENCODING_GBK;
        static const char* ENCODING_GB2312;
		static const char* ENCODING_UCS4;
		static const char* ENCODING_UCS2;
        
        EncodingUtils();
        
        char* convert(const char*,const char*,const char*);
    
        char* convertUTF8toGBK(const char*);
        
        char* convertGBKtoUTF8(const char*);
		
		char* convertUTF8toUnicode(const char* input);
		
		unsigned int getUTF8TableSize();
		
		char* getUTF8TableElement(unsigned int);
		
		unsigned int getGBKTableSize();
		
		char* getGBKTableElement(unsigned int);
};

#endif