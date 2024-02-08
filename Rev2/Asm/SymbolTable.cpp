 /*
   Copyright (C) 2022 Thomas DiModica <ricinwich@yahoo.com>

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
#include "SymbolTable.h"

int SymbolTable::getLabel (const std::string& name) const
 {
   return labels.find(name)->second;
 }

void SymbolTable::setLabel (const std::string& name, int value)
 {
   labels[name] = value;
 }

bool SymbolTable::hasLabel (const std::string& name) const
 {
   return labels.find(name) != labels.end();
 }

int SymbolTable::getResult (const std::string& name) const
 {
   return results.find(name)->second;
 }

void SymbolTable::setResult (const std::string& name, int value)
 {
   results[name] = value;
 }

bool SymbolTable::hasResult (const std::string& name) const
 {
   return results.find(name) != results.end();
 }

int SymbolTable::getSymbol (const std::string& name) const
 {
   return symbols.find(name)->second;
 }

void SymbolTable::setSymbol (const std::string& name, int value)
 {
   symbols[name] = value;
 }

bool SymbolTable::hasSymbol (const std::string& name) const
 {
   return symbols.find(name) != symbols.end();
 }

void SymbolTable::addLocalLabel(const std::string& name)
 {
   localLabels.insert(name);
 }

void SymbolTable::flushLocalLabels()
 {
   for (const auto& label : localLabels)
    {
      labels.erase(label);
    }
   localLabels.clear();
 }
