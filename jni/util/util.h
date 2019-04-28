#pragma once

uintptr_t FindLibrary(const char* library);
void cp1251_to_utf8(char *out, const char *in, unsigned int len = 0);
//转码
int code_to(char *f_charset,char* to_charset,char *inbuf,int inlen,char *outbuf,int outlen);
	//gbk转utf8
void gbk_to_utf8(char *out,char *in);
int g_to_u(char *inbuf,int inlen,char *outbuf,int outlen);
//utf8转gbk
char* utf8_to_gbk(char *in);
