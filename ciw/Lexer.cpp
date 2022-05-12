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
#include "Lexer.hpp"

Lexer::Lexer (const std::string & input) : input(input), inputPosition(0), lineNumber(1), nextToken(0)
 {
   keyWords.insert(std::make_pair("const", CONST));
   keyWords.insert(std::make_pair("dim", DIM));
   keyWords.insert(std::make_pair("static", STATIC));
   keyWords.insert(std::make_pair("function", FUNCTION));
   keyWords.insert(std::make_pair("declare", DECLARE));
   keyWords.insert(std::make_pair("end", END));
   keyWords.insert(std::make_pair("if", IF));
   keyWords.insert(std::make_pair("then", THEN));
   keyWords.insert(std::make_pair("else", ELSE));
   keyWords.insert(std::make_pair("fi", FI));
   keyWords.insert(std::make_pair("do", DO));
   keyWords.insert(std::make_pair("while", WHILE));
   keyWords.insert(std::make_pair("until", UNTIL));
   keyWords.insert(std::make_pair("loop", LOOP));
   keyWords.insert(std::make_pair("break", BREAK));
   keyWords.insert(std::make_pair("continue", CONTINUE));
   keyWords.insert(std::make_pair("return", RETURN));
   keyWords.insert(std::make_pair("tailcall", TAILCALL));
   keyWords.insert(std::make_pair("call", CALL));
   keyWords.insert(std::make_pair("and", AND));
   keyWords.insert(std::make_pair("or", OR));
   keyWords.insert(std::make_pair("xor", XOR_OP));
   keyWords.insert(std::make_pair("not", NOT));
   keyWords.insert(std::make_pair("asm", ASM));

   AfterNewline();
   Get_NextToken();
 }

void Lexer::AfterNewline (void)
 {
   char c = GetNextChar();
   while ('#' == c)
    {
      while (('\n' != c) && ('\0' != c)) c = GetNextChar();
      c = GetNextChar();
    }
   UnGetNextChar();
 }

char Lexer::GetNextNonWhite (void)
 {
   char c = GetNextChar();
   while ((' ' == c) || ('\t' == c))
      c = GetNextChar(); // Return a '\0' on EOF
   return c;
 }

char Lexer::GetNextChar (void)
 {
   char ret;
   if (inputPosition >= input.length())
    {
      ret = '\0';
      ++inputPosition;
    }
   else ret = input[inputPosition++];

   if ('\n' == ret) ++lineNumber;

   return ret;
 }

void Lexer::UnGetNextChar (void)
 {
   --inputPosition;
   if ((inputPosition < input.length()) && ('\n' == input[inputPosition]))
      --lineNumber;
 }

void Lexer::Get_NextToken (void)
 {
   nextToken = Token(lineNumber);

   while (nextToken.lexeme == COMMENT)
    {
      long n = GetNextNonWhite();
      if (std::isalpha(n)) // Read a letter
       { // Read in an identifier or keyword -- loop until not letter, number, or '_'
         nextToken.text = static_cast<char>(n);
         n = GetNextChar();
         while (std::isalnum(n) || ('_' == n))
          {
            nextToken.text += static_cast<char>(n);
            n = GetNextChar();
          }
         UnGetNextChar();

         std::map<std::string, Lexeme>::iterator keyWord = keyWords.find(nextToken.text);

         if (keyWords.end() == keyWord)
          {
            nextToken.lexeme = IDENTIFIER;
          }
         else
          {
            nextToken.lexeme = keyWord->second;
          }
         // Get_NextToken doesn't do Token collapsing. That is GetNextToken's job.
       }
      else if (std::isdigit(n)) //Read a number or the begining of one
       { //Read in a number
         nextToken.text = ""; // Clear a previous \\\n comment, if present.
         while (std::isdigit(n))
          {
            nextToken.text += static_cast<char>(n);
            n = GetNextChar();
          }
         nextToken.lexeme = NUMBER;
         UnGetNextChar();
       }
      else
       { //DFA for all other tokens
         if ((n != '\0') && (n != '\n')) nextToken.text = static_cast<char>(n);
         switch (n)
          {
         case '<':
            switch (GetNextChar())
             {
            case '-':
               nextToken.text += '-';
               nextToken.lexeme = ASSIGNMENT;
               break;
            case '>':
               nextToken.text += '>';
               nextToken.lexeme = INEQUALITY;
               break;
            case '=':
               nextToken.text += '=';
               nextToken.lexeme = LESS_THAN_OR_EQUAL_TO;
               break;
            case '<':
               nextToken.text += '<';
               switch (GetNextChar())
                {
               case '>':
                  nextToken.text += '>';
                  nextToken.lexeme = ROTATE_LEFT;
                  break;
               default:
                  UnGetNextChar();
                  nextToken.lexeme = SHIFT_LEFT;
                  break;
                }
               break;
            default:
               UnGetNextChar();
               nextToken.lexeme = LESS_THAN;
               break;
             }
            break;

         case '/':
            switch (GetNextChar())
             {
            case '/':
               n = GetNextChar();
               while ((n != '\n') && (n != '\0')) n = GetNextChar();
               AfterNewline();
               nextToken.text = "__NEWLINE__";
               nextToken.lexeme = NEW_LINE;
               break;
            case '\\':
               nextToken.text += '\\';
               nextToken.lexeme = AND_OP;
               break;
            default:
               UnGetNextChar();
               nextToken.lexeme = DIVIDE;
               break;
             }
            break;

         case '>':
            switch (GetNextChar())
             {
            case '>':
               nextToken.text += '>';
               switch (GetNextChar())
                {
               case '>':
                  nextToken.text += '>';
                  nextToken.lexeme = UNSIGNED_SHIFT_RIGHT;
                  break;
               case '<':
                  nextToken.text += '<';
                  nextToken.lexeme = ROTATE_RIGHT;
                  break;
               default:
                  UnGetNextChar();
                  nextToken.lexeme = SHIFT_RIGHT;
                  break;
                }
               break;
            case '=':
               nextToken.text += '=';
               nextToken.lexeme = GREATER_THAN_OR_EQUAL_TO;
               break;
            default:
               UnGetNextChar();
               nextToken.lexeme = GREATER_THAN;
               break;
             }
            break;

         case '\\':
            switch (GetNextChar())
             {
            case '/':
               nextToken.text += '/';
               nextToken.lexeme = OR_OP;
               break;
            case '\n':
               nextToken.lexeme = COMMENT;
               break;
            default:
               UnGetNextChar();
               nextToken.lexeme = INVALID;
               break;
             }
            break;

         case ':':
            switch (GetNextChar())
             {
            case '=':
               nextToken.text += '=';
               nextToken.lexeme = ASSIGNMENT;
               break;
            default:
               UnGetNextChar();
               nextToken.lexeme = COLON;
               break;
             }
            break;

         case '!':
            switch (GetNextChar())
             {
            case '=':
               nextToken.text += '=';
               nextToken.lexeme = INEQUALITY;
               break;
            default:
               UnGetNextChar();
               nextToken.lexeme = NOT;
               break;
             }
            break;

         case '=':
            switch (GetNextChar())
             {
            case '=':
               nextToken.text += '=';
               nextToken.lexeme = EQUALITY;
               break;
            default:
               UnGetNextChar();
               nextToken.lexeme = EQUAL_SIGN;
               break;
             }
            break;

         case '&':
            switch (GetNextChar())
             {
            case '&':
               nextToken.text += '&';
               nextToken.lexeme = SHORT_AND;
               break;
            default:
               UnGetNextChar();
               nextToken.lexeme = AND_OP;
               break;
             }
            break;

         case '|':
            switch (GetNextChar())
             {
            case '|':
               nextToken.text += '|';
               nextToken.lexeme = SHORT_OR;
               break;
            default:
               UnGetNextChar();
               nextToken.lexeme = OR_OP;
               break;
             }
            break;

         case '(':
            switch (GetNextChar())
             {
            case '*':
             {
               nextToken.text = "";
               nextToken.lexeme = COMMENT;
               bool done = false;
               while (!done)
                {
                  n = GetNextChar();
                  while ((n != '*') && (n != '\0')) n = GetNextChar();
                  if (n != '\0') n = GetNextChar();
                  while ((n == '*') && (n != '\0')) n = GetNextChar();
                  if ((n == ')') || (n == '\0')) done = true;
                }
               if (n == '\0') Error("End of File found before comment terminated.");
               break;
             }
            default:
               UnGetNextChar();
               nextToken.lexeme = OPEN_PARENS;
               break;
             }
            break;

         case '?':
            nextToken.lexeme = XOR_OP;
            break;

         case '@':
            nextToken.lexeme = AT;
            break;

         case '*':
            nextToken.lexeme = MULTIPLY;
            break;

         case '#':
            nextToken.lexeme = INEQUALITY;
            break;

         case ';':
            nextToken.lexeme = SEMICOLON;
            break;

         case '+':
            nextToken.lexeme = PLUS;
            break;

         case '-':
            nextToken.lexeme = MINUS;
            break;

         case '%':
            nextToken.lexeme = REMAINDER;
            break;

         case ')':
            nextToken.lexeme = CLOSE_PARENS;
            break;

         case '[':
            nextToken.lexeme = LEFT_BRACKET;
            break;

         case ']':
            nextToken.lexeme = RIGHT_BRACKET;
            break;

         case '{':
            nextToken.lexeme = LEFT_BRACE;
            break;

         case '}':
            nextToken.lexeme = RIGHT_BRACE;
            break;

         case '\0':
            nextToken.text = "__EOF__";
            nextToken.lexeme = END_OF_FILE;
            break;

         case '\n':
            AfterNewline();
            nextToken.text = "__NEWLINE__";
            nextToken.lexeme = NEW_LINE;
            break;

         case '"':
            nextToken.text = "";
            n = GetNextChar();
            while ((n != '"') && (n != '\0') && (n != '\n'))
             {
               nextToken.text += n;
               n = GetNextChar();
             }
            if (n == '\n')
             {
               Error("End of Line found before string terminated.");
               UnGetNextChar();
             }
            if (n == '\0') Error("End of File found before string terminated.");

            nextToken.lexeme = STRING;
            break;

         case '\'':
            nextToken.text = "";
            n = GetNextChar();
            while ((n != '\'') && (n != '\0') && (n != '\n'))
             {
               nextToken.text += n;
               n = GetNextChar();
             }
            if (n == '\n')
             {
               Error("End of Line found before string terminated.");
               UnGetNextChar();
             }
            if (n == '\0') Error("End of File found before string terminated.");

            nextToken.lexeme = STRING;
            break;

         default:
            nextToken.text = "INVALID : ";
            nextToken.text += n;
            nextToken.lexeme = INVALID;
            break;
          }
       }
    }
 }

Token Lexer::GetNextToken (void)
 {
   Token result (nextToken);

   Get_NextToken();

   switch (result.lexeme)
    {
   case AND:
      switch (nextToken.lexeme)
       {
      case THEN:
         result.lexeme = SHORT_AND;
         result.text = result.text + " " + nextToken.text;

         Get_NextToken();

         break;
      default:
         result.lexeme = AND_OP;
         break;
       }
      break;

   case OR:
      switch (nextToken.lexeme)
       {
      case ELSE:
         result.lexeme = SHORT_OR;
         result.text = result.text + " " + nextToken.text;

         Get_NextToken();

         break;
      default:
         result.lexeme = OR_OP;
         break;
       }
      break;

   case END:
      switch (nextToken.lexeme)
       {
      case IF:
         result.lexeme = FI;
         result.text = result.text + " " + nextToken.text;

         Get_NextToken();
         break;

      case FUNCTION:
         result.lexeme = END_FUNCTION;
         result.text = result.text + " " + nextToken.text;

         Get_NextToken();
         break;

      default:
         Error("END without IF or FUNCTION");
         result.lexeme = INVALID;
         break;
       }
      break;

   default:
      break;
    }

   return result;
 }
