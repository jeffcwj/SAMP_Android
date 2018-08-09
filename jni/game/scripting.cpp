#include "main.h"

GAME_SCRIPT_THREAD *gst;
uint8_t ScriptBuf[255];
uintptr_t *pdwParamVars[18];

int ExecuteScriptBuf()
{
	gst->dwScriptIP = (uintptr_t)ScriptBuf;
	uint32_t (*ProcessOneCommand)(GAME_SCRIPT_THREAD*);
	*(void **) (&ProcessOneCommand) = (void*)(g_libGTASA+ADDR_PROCESSONECOMMAND+1);
	(*ProcessOneCommand)(gst);

	return 1;
}

int ScriptCommand(const SCRIPT_COMMAND *pScriptCommand, ...)
{
	va_list ap;
	const char* p = pScriptCommand->Params;
	va_start(ap, pScriptCommand);
	memcpy(&ScriptBuf, &pScriptCommand->OpCode, 2);
	int buf_pos = 2;
	uint16_t var_pos = 0;

	for(int i = 0; i < 18; i++)
		gst->dwLocalVar[i] = 0;

	while(*p)
	{
		switch(*p)
		{
			case 'i':
			{
				int i = va_arg(ap, int);
				ScriptBuf[buf_pos] = 0x01;
				buf_pos++;
				memcpy(&ScriptBuf[buf_pos], &i, 4);
				buf_pos += 4;
				break;
			}
			case 'f':
			{
				float f = (float)va_arg(ap, double);
				ScriptBuf[buf_pos] = 0x06;
				buf_pos++;
				memcpy(&ScriptBuf[buf_pos], &f, 4);
				buf_pos += 4;
				break;
			}
			case 'v':
			{
				uint32_t *v = va_arg(ap, uint32_t*);
				ScriptBuf[buf_pos] = 0x03;
				buf_pos++;
				pdwParamVars[var_pos] = v;
				gst->dwLocalVar[var_pos] = *v;
				memcpy(&ScriptBuf[buf_pos], &var_pos, 2);
				buf_pos += 2;
				var_pos++;
				break;
			}
			default:
			{
				return 0;
			}
		}
		++p;
	}
	va_end(ap);

	int result = ExecuteScriptBuf();
	if (var_pos)
	{
		for (int i=0; i < var_pos; i++)
			*pdwParamVars[i] = gst->dwLocalVar[i];
	}

	return result;
}

void InitScripting()
{
	gst = new GAME_SCRIPT_THREAD;
	memset(gst, 0, sizeof(GAME_SCRIPT_THREAD));
}
