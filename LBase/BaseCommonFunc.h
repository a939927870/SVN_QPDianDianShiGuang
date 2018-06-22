#ifndef __BaseCommonFunc_h
#define __BaseCommonFunc_h

#include <string>
#include "LBase.h"
#include "LMsg.h"

void			DisableIP(const std::string& ip);

unsigned short csum (unsigned char *data, int dataSize);

Lint			TransfromMsgId(Lint timestamp, Lint msgId);
Lint			InverseTransfromMsgId(Lint timestamp, Lint transedMsgId);

std::time_t		GetLastModifiedTime(const char* path);
LMsg*			ParseInsideNetPacket(LBuffPtr recv, LSocketPtr s);
Llong           GetOSTickCount();

bool			tokenize(const Lstring& str, std::vector<Lstring>& tokens, const Lstring& delims);
bool			safe_atoi(const Lstring &str, Lint &num);
bool			safe_atof(const std::string &str, Lfloat &num);
bool            tokenize_atovi(const Lstring &str, std::vector<Lint>& tokens, const Lstring& delims);
bool            tokenize_vitoa(const std::vector<Lint>& tokens, Lstring &str, const Lstring& delims);

#endif		// __BaseCommonFunc_h