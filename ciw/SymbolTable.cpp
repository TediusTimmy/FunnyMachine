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

void DB_panic (const std::string &, const CallingContext &, size_t) __attribute__ ((__noreturn__));

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

   DB_panic("COMPILER ERROR!!! : getValue to non-existent variable : " + name, *this, lineNo);
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

   DB_panic("COMPILER ERROR!!! : setValue to non-existent variable", *this, lineNo);
 }

int CallingContext::nextGlobal = 128;

int CallingContext::createArray (int length)
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

int CallingContext::nextLabel = 0;

std::string CallingContext::getNextLabel()
 {
   return "auto_" + std::to_string(++nextLabel);
 }

int CallingContext::getNumLocals(const std::string& name, size_t lineNo) const
 {
   std::map<std::string, int>::const_iterator test;

   test = m_allLocals.find(name);
   if (m_allLocals.end() != test)
    {
      return test->second;
    }

   DB_panic("COMPILER ERROR!!! : getNumLocals to non-existent function", *this, lineNo);
 }
