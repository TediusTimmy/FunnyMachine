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
#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP

#include <map>
#include <string>
#include <vector>
#include <set>
#include <memory>

class StatementSeq;

class CallingContext
 {
   private:
      std::map<std::string, int> m_locals;
      std::map<std::string, std::map<std::string, int> > & m_allGlobals;

      std::map<std::string, int> & m_statics;
      std::map<std::string, int> & m_globals;
      std::map<std::string, int> & m_constants;
      std::map<std::string, int> & m_allLocals;
      std::map<std::string, std::vector<std::string> > & m_functions;
      std::map<std::string, std::map<std::string, int> > & m_funLocals;
      std::map<std::string, std::shared_ptr<StatementSeq> > & m_funDefs;

   public:
      const std::string Name;
      std::map<std::string, int> & Locals() { return m_locals; }
      std::map<std::string, int> & Statics() { return m_statics; }
      std::map<std::string, int> & Globals() { return m_globals; }
      std::map<std::string, int> & Constants() { return m_constants; }
      std::map<std::string, int> & AllLocals() { return m_allLocals; }
      std::map<std::string, std::vector<std::string> > & Functions() { return m_functions; }
      const std::map<std::string, std::vector<std::string> > & Functions() const { return m_functions; }
      std::map<std::string, std::map<std::string, int> > & FunLocals() { return m_funLocals; }
      std::map<std::string, std::shared_ptr<StatementSeq> > & FunDefs() { return m_funDefs; }
      const CallingContext * Parent;
      size_t ParentLine;

      CallingContext(CallingContext & src, const std::string & name, size_t lineNo) :
         m_allGlobals(src.m_allGlobals),
         m_statics(src.m_allGlobals[name]),
         m_globals(src.m_globals),
         m_constants(src.m_constants),
         m_allLocals(src.m_allLocals),
         m_functions(src.m_functions),
         m_funLocals(src.m_funLocals),
         m_funDefs(src.m_funDefs),
         Name(name),
         Parent(&src),
         ParentLine(lineNo)
       {
       }

      CallingContext(
         std::map<std::string, std::map<std::string, int> > & allGlobals,
         std::map<std::string, int> & constants,
         std::map<std::string, int> & allLocals,
         std::map<std::string, std::vector<std::string> > & functions,
         std::map<std::string, std::map<std::string, int> > & funLocals,
         std::map<std::string, std::shared_ptr<StatementSeq> > & funDefs) :

         m_allGlobals(allGlobals),
         m_statics(allGlobals[""]),
         m_globals(allGlobals[""]),
         m_constants(constants),
         m_allLocals(allLocals),
         m_functions(functions),
         m_funLocals(funLocals),
         m_funDefs(funDefs),
         Name("BaseContext"),
         Parent(NULL),
         ParentLine(0U)
       {
       }

   // Base CallingContext: CallingContext(Globals, Constants, FunctionArgs, FunctionVars, FunctionDefs);

      // Should be a stack, but I need random-access.
      std::vector<std::string> m_labels;
      std::string m_currentFunction;

      enum IdentifierType
       {
         UNDEFINED,
         VARIABLE, // Global, Static, or Local
         CONSTANT,
         FUNCTION,
         LABEL
       };

      IdentifierType lookup (const std::string & name) const
       {
         if (m_locals.end() != m_locals.find(name)) return VARIABLE;
         if (m_statics.end() != m_statics.find(name)) return VARIABLE;
         if (m_globals.end() != m_globals.find(name)) return VARIABLE;
         if (m_constants.end() != m_constants.find(name)) return CONSTANT;
         if (m_functions.end() != m_functions.find(name)) return FUNCTION;
         for (std::vector<std::string>::const_iterator iter = m_labels.begin();
            iter != m_labels.end(); ++iter)
          {
            if (name == *iter) return LABEL;
          }
         return UNDEFINED;
       }

      int getValue (const std::string &, size_t) const;
      void setValue (const std::string &, int, size_t);

      int getNumLocals(const std::string&, size_t) const;
 };

class GlobalData
 {
private:
   int nextLabel;
   int nextGlobal;

public:
   GlobalData();

   std::string getNextLabel();
   int createArray (int length);
 };

#endif /* SYMBOLTABLE_HPP */
