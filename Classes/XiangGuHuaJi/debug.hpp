/* XiangGuHuaJi 2016, debug.hpp
 * wtf
 * 
 */

#ifndef _XIANGGUHUAJI_DEBUG_HPP__
#define _XIANGGUHUAJI_DEBUG_HPP__
#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

using std::cout;
using std::endl;
using std::vector;
using std::string;


const char separator  = ' ';

#define GAME_DEBUG

//����ĸ������� ������ӡһ������
template<typename T> 
inline void printElement(T t)
{
	cout << std::left << std::setw(6) << std::setfill(separator) << t;
}

template<> 
inline void printElement<float>(float t)
{
	const static char separator  = ' ';
	if (t < 0.01)
	{
		cout << std::left << std::setw(6) << std::setprecision(2) << std::setfill(separator) << "<0.01";
	}
	else
	{
		cout << std::left << std::setw(6) << std::setprecision(2) << std::setfill(separator) << t;
	}
}

template<> 
inline void printElement<unsigned char>(unsigned char t)
{
	const static char separator  = ' ';
	cout << std::left << std::setw(6) << std::setfill(separator) << (int)t;
}

template <typename vecT>
void printVecMat(vector<vector<vecT>> m, string name)
{
	cout<<name<<endl;
	for (unsigned int i=0; i<m.size(); i++)
	{
		for (unsigned int j=0; j<m[i].size(); j++)
		{
			printElement<vecT>(m[i][j]);
		}
		cout<<endl;
	}
}

#endif
