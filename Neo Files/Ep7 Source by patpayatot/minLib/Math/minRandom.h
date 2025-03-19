#pragma once

#include "../minLib.h"

namespace minLib
{

class minRandom
{
public:
	minRandom(void);
	~minRandom(void);

	/**
	* Generate randim string
	* \param str ������ ���ڿ��� ���� ��
	* \param Length ���ڿ�����
	* \param Small 
	* \param Capital 
	* \param Numbers 
	* \return true / false
	*/
	bool getRandomString(std::string& str, int Length, int Small, int Capital, int Numbers);
};

}