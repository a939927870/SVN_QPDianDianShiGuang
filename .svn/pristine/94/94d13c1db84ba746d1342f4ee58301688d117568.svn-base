#ifndef _L_INI_H
#define _L_INI_H

#include "LBase.h"

class LIniConfig
{
public:
	LIniConfig();
    LIniConfig(Lstring file);
	~LIniConfig();

	void	LoadFile(const Lstring& file);

	Lstring	GetString(const Lstring& key);
	Lstring GetString(const Lstring& key,const Lstring& def);

	Lint	GetInt(const Lstring& key);
	Lint	GetInt(const Lstring& key,Lint def);
    
    Lshort	GetShort(const Lstring& key);
    Lshort  GetShort(const Lstring& key,Lshort def);
   
    Lchar	GetChar(const Lstring& key);
    Lchar	GetChar(const Lstring& key,Lchar def);

private:
    std::map<Lstring,Lstring> m_ini_map;
	
};

#endif