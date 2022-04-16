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
#include "Lexer.h"
#include <cctype>

Lexer::Lexer (const std::string & input) : input(input), inputPosition(0), lineNumber(1), nextToken(0)
 {
   Get_NextToken();
 }

char Lexer::GetNextNonWhite (void)
 {
   char c = GetNextChar();
   while ((' ' == c) || ('\t' == c) || ('\r' == c))
      c = GetNextChar(); // Return a '\0' on EOF
   return c;
 }

char Lexer::GetNextChar (void)
 {
   char ret;
   if (inputPosition >= input.length())
    {
      ret = '\0';
    }
   else ret = input[inputPosition++];

   if ('\n' == ret) ++lineNumber;

   return ret;
 }

char Lexer::PeekNextChar (void)
 {
   char ret;
   if (inputPosition >= input.length())
    {
      ret = '\0';
    }
   else ret = input[inputPosition];

   return ret;
 }

void Lexer::Get_NextToken (void)
 {
   nextToken = Token(lineNumber);

   nextToken.lexeme = BLANK;
   while (nextToken.lexeme == BLANK)
    {
      int n = GetNextNonWhite();
      if (std::isalpha(n)) // Read a letter
       { // Read in an identifier or mnemonic -- loop until not letter, number, or '_'
         nextToken.text = static_cast<char>(n);
         n = PeekNextChar();
         while (std::isalnum(n) || ('_' == n))
          {
            GetNextChar();
            nextToken.text += static_cast<char>(n);
            n = PeekNextChar();
          }

         nextToken.lexeme = IDENTIFIER;
       }
      else if (std::isdigit(n)) //Read a decimal number or the beginning of one
       { //Read in a number
         while (std::isdigit(n))
          {
            GetNextChar();
            nextToken.text += static_cast<char>(n);
            n = PeekNextChar();
          }

         nextToken.lexeme = NUMBER;
       }
      else
       { //DFA for all other tokens
         if ((n != '\0') && (n != '\n')) nextToken.text = static_cast<char>(n);
         switch (n)
          {
            case '\0':
               nextToken.text = "__END_OF_FILE__";
               nextToken.lexeme = END_OF_FILE;
               break;

            case ';':
               n = GetNextChar();
               while ((n != '\n') && (n != '\0')) n = GetNextChar();
               [[fallthrough]];
            case '\n':
               nextToken.text = "__END_OF_LINE__";
               nextToken.lexeme = END_OF_LINE;
               break;

            case '.':
               nextToken.lexeme = PERIOD;
               break;

            case ',':
               nextToken.lexeme = COMMA;
               break;

            case ':':
               nextToken.lexeme = COLON;
               break;

            case '#':
               nextToken.lexeme = POUND;
               break;

            case '@':
               nextToken.lexeme = AT;
               break;

            case '$':
               n = PeekNextChar();
               while (std::isdigit(n) || ((n >= 'A') && (n <= 'F'))  || ((n >= 'a') && (n <= 'f')))
                {
                  GetNextChar();
                  nextToken.text += static_cast<char>(n);
                  n = PeekNextChar();
                }

               if ("$" == nextToken.text)
                {
                  nextToken.lexeme = INVALID;
                }
               else
                {
                  nextToken.lexeme = NUMBER;
                }
               break;

            case '\'':
               n = PeekNextChar();
               while ((n != '\'') && (n != '\0') && (n != '\n'))
                {
                  GetNextChar();
                  nextToken.text += static_cast<char>(n);
                  n = PeekNextChar();
                }
               if (n == '\'')
                {
                  GetNextChar();
                }

               if ((n == '\n') || (n == '\0') || ("'" == nextToken.text))
                {
                  nextToken.lexeme = INVALID;
                }
               else
                {
                  nextToken.lexeme = NUMBER;
                }
               break;

            case '"':
               nextToken.text = "";
               n = PeekNextChar();
               while ((n != '"') && (n != '\0') && (n != '\n'))
                {
                  GetNextChar();
                  nextToken.text += static_cast<char>(n);
                  n = PeekNextChar();
                }
               if (n == '"')
                {
                  GetNextChar();
                }

               if ((n == '\n') || (n == '\0'))
                {
                  nextToken.lexeme = INVALID;
                }
               else
                {
                  nextToken.lexeme = STRING;
                }
               break;

            case '(':
               nextToken.lexeme = OPEN_PARENS;
               break;

            case ')':
               nextToken.lexeme = CLOSE_PARENS;
               break;

            case '+':
               nextToken.lexeme = PLUS;
               break;

            case '-':
               nextToken.lexeme = MINUS;
               break;

            case '~':
               nextToken.lexeme = NOT;
               break;

            case '>':
               switch (PeekNextChar())
                {
                  case '=':
                     GetNextChar();
                     nextToken.text += '=';
                     nextToken.lexeme = GREATER_THAN_OR_EQUAL_TO;
                     break;
                  case '>':
                     GetNextChar();
                     nextToken.text += '>';
                     if ('>' == PeekNextChar())
                     {
                        GetNextChar();
                        nextToken.text += '>';
                        nextToken.lexeme = UNSIGNED_SHIFT_RIGHT;
                     }
                     else
                     {
                        nextToken.lexeme = SHIFT_RIGHT;
                     }
                     break;
                  default:
                     nextToken.lexeme = GREATER_THAN;
                     break;
                }
               break;

            case '<':
               switch (PeekNextChar())
                {
                  case '=':
                     GetNextChar();
                     nextToken.text += '=';
                     nextToken.lexeme = LESS_THAN_OR_EQUAL_TO;
                     break;
                  case '<':
                     GetNextChar();
                     nextToken.text += '<';
                     nextToken.lexeme = SHIFT_LEFT;
                     break;
                  case '>':
                     GetNextChar();
                     nextToken.text += '>';
                     nextToken.lexeme = INEQUALITY;
                     break;
                  default:
                     nextToken.lexeme = LESS_THAN;
                     break;
                }
               break;

            case '*':
               nextToken.lexeme = MULTIPLY;
               break;

            case '/':
               nextToken.lexeme = DIVIDE;
               break;

            case '%':
               switch (PeekNextChar())
                {
                  case '0':
                  case '1':
                     n = PeekNextChar();
                     while ((n == '0') || (n == '1'))
                      {
                        GetNextChar();
                        nextToken.text += static_cast<char>(n);
                        n = PeekNextChar();
                      }

                     nextToken.lexeme = NUMBER;
                     break;
                  default:
                     nextToken.lexeme = MOD;
                     break;
                }
               break;

            case '&':
               switch (PeekNextChar())
                {
                  case '&':
                     GetNextChar();
                     nextToken.text += '&';
                     nextToken.lexeme = SHORT_AND;
                     break;
                  default:
                     nextToken.lexeme = AND;
                     break;
                }
               break;

            case '|':
               switch (PeekNextChar())
                {
                  case '|':
                     GetNextChar();
                     nextToken.text += '|';
                     nextToken.lexeme = SHORT_OR;
                     break;
                  default:
                     nextToken.lexeme = OR;
                     break;
                }
               break;

            case '^':
               nextToken.lexeme = XOR;
               break;

            case '=':
               nextToken.lexeme = EQUALITY;
               break;

            case '!':
               nextToken.lexeme = BOOL_NOT;
               break;

            case '`':
               nextToken.lexeme = LOCAL_REF;
               break;

            default:
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

   return result;
 }
