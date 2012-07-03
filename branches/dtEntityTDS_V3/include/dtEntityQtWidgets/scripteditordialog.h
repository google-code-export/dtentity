#pragma once

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

#include <dtEntityQtWidgets/export.h>

#include <QtGui/QDialog>
#include <QtGui/QTextEdit>
#include <QtGui/QSyntaxHighlighter>
#include <QtCore/QHash>
#include <QtGui/QTextCharFormat>
#include <QtGui/QTextDocument>

namespace dtEntityQtWidgets
{


   class ENTITYQTWIDGETS_EXPORT ScriptEditorDialog
      : public QDialog
   {
      Q_OBJECT

   public:
      ScriptEditorDialog(const QString& text, const QString& language = "");
      ~ScriptEditorDialog();
      
      QString GetText() const;

   private:
      QTextEdit* mEditor;
      QSyntaxHighlighter* mHighlighter;
      
   };
}
