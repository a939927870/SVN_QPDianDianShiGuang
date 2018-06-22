#ifndef _L_TABFILE_H
#define _L_TABFILE_H
#endif
#include "LBase.h"

#define MAX_TXT_FILE	(1024)
class LTabFile
{
private:
	int			m_index;	
	std::vector< std::vector< std::string > > fileItems;
public:
	LTabFile(void);
	~LTabFile(void);
	BOOL		Load(LPSTR FileName);
	void		clear();
	std::vector< std::string >& getTapFileLine();
};

