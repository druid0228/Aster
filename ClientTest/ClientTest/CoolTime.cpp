#include "CoolTime.h"

CoolTime::CoolTime()
{
	dict.clear();
}

CoolTime::~CoolTime()
{
	dict.clear();
}

// add actType
void CoolTime::insert(unsigned actType)
{
	if (dict.count(actType) == 0)
	{
		dict.insert({ actType,true });
	}
	else
	{
		cout << "Error CoolTime::insert\n";
	}
}

void CoolTime::update(float elapsed)
{
}

bool CoolTime::checkCool(unsigned actType)
{
	if (dict.count(actType) != 0)
	{
		return dict[actType];
	}
	else {
		cout << "error checkCool: " << actType << "\n";
		return false;
	}
}





// player move, act move, move...
// 구분을 전부 enum? x
// pair?

// object?					coolTIme
//    - cont []		~~~		dict - [id,type,bool]
//  | gameframework