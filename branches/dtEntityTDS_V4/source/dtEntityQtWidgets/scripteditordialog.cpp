/*
* dtEntity Game and Simulation Engine
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* Martin Scheffler
*/

#include <dtEntityQtWidgets/scripteditordialog.h>
#include <QtGui/QtGui>
#include "ui_scripteditordialog.h"

namespace dtEntityQtWidgets
{

   class QLuaSyntaxHighlighter : public QSyntaxHighlighter
   {

   public:

       QLuaSyntaxHighlighter(QTextDocument *document)
         : QSyntaxHighlighter(document)
      {
          HighlightingRule rule;

          keywordFormat.setForeground(Qt::darkBlue);
          keywordFormat.setFontWeight(QFont::Bold);
          QStringList keywordPatterns;
          keywordPatterns << "\\band\\b" << "\\bbreak\\b" << "\\bdo\\b"
                          << "\\belse\\b" << "\\belseif\\b" << "\\bend\\b"
                          << "\\bfalse\\b" << "\\bfor\\b" << "\\bfunction\\b"
                          << "\\bif\\b" << "\\bin\\b" << "\\blocal\\b"
                          << "\\bnil\\b" << "\\bnot\\b" << "\\bor\\b"
                          << "\\brepeat\\b" << "\\breturn\\b" << "\\bthen\\b"
                          << "\\btrue\\b" << "\\buntil\\b" << "\\bwhile\\b";

          foreach (const QString &pattern, keywordPatterns) {
              rule.pattern = QRegExp(pattern);
              rule.format = keywordFormat;
              highlightingRules.append(rule);
          }

          classFormat.setFontWeight(QFont::Bold);
          classFormat.setForeground(Qt::darkMagenta);
          rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
          rule.format = classFormat;
          highlightingRules.append(rule);

          singleLineCommentFormat.setForeground(Qt::red);
          rule.pattern = QRegExp("//[^\n]*");
          rule.format = singleLineCommentFormat;
          highlightingRules.append(rule);

          multiLineCommentFormat.setForeground(Qt::red);

          quotationFormat.setForeground(Qt::darkGreen);
          rule.pattern = QRegExp("\".*\"");
          rule.format = quotationFormat;
          highlightingRules.append(rule);

          functionFormat.setFontItalic(true);
          functionFormat.setForeground(Qt::blue);
          rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
          rule.format = functionFormat;
          highlightingRules.append(rule);

          commentStartExpression = QRegExp("/\\*");
          commentEndExpression = QRegExp("\\*/");
      }

   protected:
       void highlightBlock(const QString &text)
       {
           foreach (const HighlightingRule &rule, highlightingRules) {
               QRegExp expression(rule.pattern);
               int index = expression.indexIn(text);
               while (index >= 0) {
                   int length = expression.matchedLength();
                   setFormat(index, length, rule.format);
                   index = expression.indexIn(text, index + length);
               }
           }
           setCurrentBlockState(0);

           int startIndex = 0;
           if (previousBlockState() != 1)
               startIndex = commentStartExpression.indexIn(text);

           while (startIndex >= 0) {
               int endIndex = commentEndExpression.indexIn(text, startIndex);
               int commentLength;
               if (endIndex == -1) {
                   setCurrentBlockState(1);
                   commentLength = text.length() - startIndex;
               } else {
                   commentLength = endIndex - startIndex
                           + commentEndExpression.matchedLength();
               }
               setFormat(startIndex, commentLength, multiLineCommentFormat);
               startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
           }
       }

       struct HighlightingRule
       {
           QRegExp pattern;
           QTextCharFormat format;
       };
       QVector<HighlightingRule> highlightingRules;

       QRegExp commentStartExpression;
       QRegExp commentEndExpression;

       QTextCharFormat keywordFormat;
       QTextCharFormat classFormat;
       QTextCharFormat singleLineCommentFormat;
       QTextCharFormat multiLineCommentFormat;
       QTextCharFormat quotationFormat;
       QTextCharFormat functionFormat;
   };

   class QJavaScriptSyntaxHighlighter : public QSyntaxHighlighter
   {

   public:

       QJavaScriptSyntaxHighlighter(QTextDocument *document)
         : QSyntaxHighlighter(document)
      {
          HighlightingRule rule;

          keywordFormat.setForeground(Qt::darkBlue);
          keywordFormat.setFontWeight(QFont::Bold);
          QStringList keywordPatterns;

         keywordPatterns << "\\bbreak\\b";
         keywordPatterns << "\\bcase\\b";
         keywordPatterns << "\\bcatch\\b";
         keywordPatterns << "\\bcontinue\\b";
         keywordPatterns << "\\bdefault\\b";
         keywordPatterns << "\\bdelete\\b";
         keywordPatterns << "\\bdo\\b";
         keywordPatterns << "\\belse\\b";
         keywordPatterns << "\\bfinally\\b";
         keywordPatterns << "\\bfor\\b";
         keywordPatterns << "\\bfunction\\b";
         keywordPatterns << "\\bif\\b";
         keywordPatterns << "\\bin\\b";
         keywordPatterns << "\\binstanceof\\b";
         keywordPatterns << "\\bnew\\b";
         keywordPatterns << "\\breturn\\b";
         keywordPatterns << "\\bswitch\\b";
         keywordPatterns << "\\bthis\\b";
         keywordPatterns << "\\bthrow\\b";
         keywordPatterns << "\\btry\\b";
         keywordPatterns << "\\btypeof\\b";
         keywordPatterns << "\\bva\\br";
         keywordPatterns << "\\bvoid\\b";
         keywordPatterns << "\\bwhile\\b";
         keywordPatterns << "\\bwith\\b";

         keywordPatterns << "\\btrue\\b";
         keywordPatterns << "\\bfalse\\b";
         keywordPatterns << "\\bnull\\b";


          foreach (const QString &pattern, keywordPatterns) {
              rule.pattern = QRegExp(pattern);
              rule.format = keywordFormat;
              highlightingRules.append(rule);
          }

          classFormat.setFontWeight(QFont::Bold);
          classFormat.setForeground(Qt::darkMagenta);
          rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
          rule.format = classFormat;
          highlightingRules.append(rule);

          singleLineCommentFormat.setForeground(Qt::red);
          rule.pattern = QRegExp("//[^\n]*");
          rule.format = singleLineCommentFormat;
          highlightingRules.append(rule);

          multiLineCommentFormat.setForeground(Qt::red);

          quotationFormat.setForeground(Qt::darkGreen);
          rule.pattern = QRegExp("\".*\"");
          rule.format = quotationFormat;
          highlightingRules.append(rule);

          functionFormat.setFontItalic(true);
          functionFormat.setForeground(Qt::blue);
          rule.pattern = QRegExp("\\b[A-Za-z0-9_]+(?=\\()");
          rule.format = functionFormat;
          highlightingRules.append(rule);

          commentStartExpression = QRegExp("/\\*");
          commentEndExpression = QRegExp("\\*/");
      }

   protected:
       void highlightBlock(const QString &text)
       {
           foreach (const HighlightingRule &rule, highlightingRules) {
               QRegExp expression(rule.pattern);
               int index = expression.indexIn(text);
               while (index >= 0) {
                   int length = expression.matchedLength();
                   setFormat(index, length, rule.format);
                   index = expression.indexIn(text, index + length);
               }
           }
           setCurrentBlockState(0);

           int startIndex = 0;
           if (previousBlockState() != 1)
               startIndex = commentStartExpression.indexIn(text);

           while (startIndex >= 0) {
               int endIndex = commentEndExpression.indexIn(text, startIndex);
               int commentLength;
               if (endIndex == -1) {
                   setCurrentBlockState(1);
                   commentLength = text.length() - startIndex;
               } else {
                   commentLength = endIndex - startIndex
                           + commentEndExpression.matchedLength();
               }
               setFormat(startIndex, commentLength, multiLineCommentFormat);
               startIndex = commentStartExpression.indexIn(text, startIndex + commentLength);
           }
       }

       struct HighlightingRule
       {
           QRegExp pattern;
           QTextCharFormat format;
       };
       QVector<HighlightingRule> highlightingRules;

       QRegExp commentStartExpression;
       QRegExp commentEndExpression;

       QTextCharFormat keywordFormat;
       QTextCharFormat classFormat;
       QTextCharFormat singleLineCommentFormat;
       QTextCharFormat multiLineCommentFormat;
       QTextCharFormat quotationFormat;
       QTextCharFormat functionFormat;
   };


   class XmlHighlighter : public QSyntaxHighlighter {
     enum Construct  {
      Entity,
      TagName,
      TagAttValue,
      Comment,
      LastConstruct = Comment
     };
   public:
     XmlHighlighter(QTextDocument* parent)
        : QSyntaxHighlighter(parent)
     {
       QTextCharFormat entityFormat;
       entityFormat.setForeground(QColor(0x33, 0x33, 0x33));
       entityFormat.setFontItalic(true);
       setFormatFor(Entity, entityFormat);

       QTextCharFormat tagFormat;
       tagFormat.setForeground(QColor(0, 0x99, 0));
       setFormatFor(TagName, tagFormat);

       QTextCharFormat tagValueFormat;
       tagValueFormat.setForeground(QColor(0xCC, 0, 0));
       setFormatFor(TagAttValue, tagValueFormat);

       QTextCharFormat commentFormat;
       commentFormat.setForeground(QColor(0x99, 0x99, 0x99));
       commentFormat.setFontItalic(true);
       setFormatFor(Comment, commentFormat);
     }

     void setFormatFor(Construct construct, const QTextCharFormat format)
     {
       __formats[construct] = format;
       rehighlight();
     }


     QTextCharFormat formatFor(Construct construct) const
     {
        return __formats[construct];
     }
   protected:

     void highlightBlock(const QString &text) {
       int state = previousBlockState();
       int len = text.length();
       int start = 0;
       int pos = 0;

       while (pos < len) {
        switch (state) {
        case NormalState:
        default:
        while (pos < len) {
           QChar ch = text.at(pos);
           if (ch == '<') {
              if (text.mid(pos, 4) == "<!--") {
              state = InComment;
              } else {
              state = InTagName;
              }
              break;
           } else if (ch == '&') {
              start = pos;
              while (pos < len && text.at(pos++) != ';') ;
              setFormat(start, pos - start, __formats[Entity]);
           } else {
              ++pos;
           }
        }
        break;
        case InComment:
        start = pos;
        while (pos < len) {
           if (text.mid(pos, 3) == "-->") {
              pos += 3;
              state = NormalState;
              break;
           } else {
              ++pos;
           }
        }
        setFormat(start, pos - start, __formats[Comment]);
        break;
        case InTagName:
        start = pos;
        while (pos < len) {
           QChar ch = text.at(pos);
           if (ch == '\'') {
              ++pos;
              state = InSingleQuote;
              break;
           } else if(ch == '"') {
              ++pos;
              state = InDoubleQuote;
              break;
           } else if (ch == '>') {
              ++pos;
              state = NormalState;
              break;
           }
           ++pos;
        }
        setFormat(start, pos - start, __formats[TagName]);
        break;
        case InDoubleQuote:
        case InSingleQuote:
        start = pos;
        while (pos < len) {
           QChar ch = text.at(pos);
           if (ch == (state == InSingleQuote ? '\'' : '"')) {
              ++pos;
              state = InTagName;
              break;
           }
           ++pos;
        }
        setFormat(start, pos - start, __formats[TagAttValue]);
        break;
        }
       }

       setCurrentBlockState(state);
     }

   private:
     enum State {
      NormalState = -1,
      InTagName,
      InDoubleQuote,
      InSingleQuote,
      InComment
     };

     QTextCharFormat __formats[LastConstruct + 1];
   };



   ////////////////////////////////////////////////////////////////////////////////
   ScriptEditorDialog::ScriptEditorDialog(const QString& text, const QString& language)
   { 
      Ui_ScriptEditorDialog dialog;
      dialog.setupUi(this);
      mEditor = dialog.mTextEdit;
      mEditor->setPlainText(text);

      if(language == "lua")
      {
         mHighlighter = new QLuaSyntaxHighlighter(mEditor->document());
      }
      else if(language == "xml")
      {
         mHighlighter = new XmlHighlighter(mEditor->document());
      }
      else if(language == "js")
      {
         mHighlighter = new QJavaScriptSyntaxHighlighter(mEditor->document());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ScriptEditorDialog::~ScriptEditorDialog()
   { 
   }

   ////////////////////////////////////////////////////////////////////////////////
   QString ScriptEditorDialog::GetText() const
   {
      return mEditor->toPlainText();
   }
}
