 /*
   Copyright (C) 2014 Thomas DiModica <ricinwich@yahoo.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "SymbolTable.hpp"

void DB_panic (const std::string &, size_t) __attribute__ ((__noreturn__));

int CallingContext::getValue (const std::string & name, size_t lineNo) const
 {
   std::map<std::string, int>::const_iterator test;

   test = m_locals.find(name);
   if (m_locals.end() != test) return test->second;

   test = m_statics.find(name);
   if (m_statics.end() != test) return test->second;

   test = m_globals.find(name);
   if (m_globals.end() != test) return test->second;

   test = m_constants.find(name);
   if (m_constants.end() != test) return test->second;

   DB_panic("COMPILER ERROR!!! : getValue to non-existent variable : " + name, lineNo);
 }

void CallingContext::setValue (const std::string & name, int value, size_t lineNo)
 {
   std::map<std::string, int>::iterator test;

   test = m_locals.find(name);
   if (m_locals.end() != test)
    {
      test->second = value;
      return;
    }

   test = m_statics.find(name);
   if (m_statics.end() != test)
    {
      test->second = value;
      return;
    }

   test = m_globals.find(name);
   if (m_globals.end() != test)
    {
      test->second = value;
      return;
    }

   DB_panic("COMPILER ERROR!!! : setValue to non-existent variable", lineNo);
 }

int CallingContext::getNumLocals(const std::string& name, size_t lineNo) const
 {
   std::map<std::string, int>::const_iterator test;

   test = m_allLocals.find(name);
   if (m_allLocals.end() != test)
    {
      return test->second;
    }

   DB_panic("COMPILER ERROR!!! : getNumLocals to non-existent function", lineNo);
 }

GlobalData::GlobalData() : nextLabel(0), nextGlobal(128), nextConstant(57344), nextResult(0)
 {
 }

int GlobalData::createArray (int length)
 {
   int next = nextGlobal;
   nextGlobal += 2;
   if (length > 1)
    {
      nextGlobal += (length - 1) * 2;
    }
   if (length > 0) // Nasty hack to mark arrays as arrays. Arrays have ODD addresses.
    {
      next |= 1;
    }
   return next;
 }

std::string GlobalData::getNextLabel()
 {
   return "auto_" + std::to_string(++nextLabel);
 }

void addByte(int& temp, int& count, unsigned char add, std::vector<unsigned short>& result)
 {
   temp |= (add << (8 * count));
   ++count;
   if (2 == count)
    {
      result.push_back(temp);
      temp = 0;
      count = 0;
    }
 }

int GlobalData::addString(const std::string& strang)
 {
   int next = nextConstant;
   int temp = 0, count = 0;
   for (size_t i = 0U; i < strang.length(); ++i)
    {
      addByte(temp, count, static_cast<unsigned char>(strang[i]), constantData);
    }
   addByte(temp, count, 0U, constantData);
   addByte(temp, count, 0U, constantData);
   if (0 != count)
    {
      addByte(temp, count, 0U, constantData);
    }
   nextConstant += (((strang.length() / 2) + (strang.length() & 1) + 1) * 2);
   return next;
 }

int GlobalData::addWord(int val)
 {
   int next = nextConstant;
   constantData.push_back(static_cast<unsigned short>(val));
   nextConstant += 2;
   return next;
 }

#include <sstream>
#include <iomanip>

std::string GlobalData::getNextResult()
 {
   std::stringstream temp;
   temp << "a_" << std::hex << std::setw(3) << std::setfill('0') << nextResult++;
   return temp.str();
 }
