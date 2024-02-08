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
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <map>
#include <string>
#include <vector>
#include <set>

class SymbolTable
 {
   private:
      std::map<std::string, int> labels;
      std::map<std::string, int> results;
      std::map<std::string, int> symbols;

      int currentResult;
      int currentLocation;
      int useLocation;

      std::set<std::string> localLabels;

   public:
      std::map<std::string, int>& Labels() { return labels; }
      std::map<std::string, int>& Results() { return results; }
      std::map<std::string, int>& Symbols() { return symbols; }

      SymbolTable() : currentResult(0), currentLocation(0), useLocation(0) { }

      int getLabel (const std::string&) const;
      void setLabel (const std::string&, int);
      bool hasLabel (const std::string&) const;
      int getResult (const std::string&) const;
      void setResult (const std::string&, int);
      bool hasResult (const std::string&) const;
      int getSymbol (const std::string&) const;
      void setSymbol (const std::string&, int);
      bool hasSymbol (const std::string&) const;

      int getCurrentResult() const { return currentResult; }
      int getCurrentLocation() const { return currentLocation; }
      void setCurrentLocation(int newLocation) { currentLocation = newLocation; }
      int getUseLocation() const { return useLocation; }
      void setUseLocation(int newLocation) { useLocation = newLocation; }

      void addResult (int extra) { currentResult += 1 + extra; }
      void addLocation () { currentLocation += 2; }

      void addLocalLabel(const std::string&);
      void flushLocalLabels();
 };

#endif /* SYMBOLTABLE_H */
