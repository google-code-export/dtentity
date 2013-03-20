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
#include <QtGui/QComboBox>
#include <QtGui/QLineEdit>

namespace dtEntityQtWidgets
{

   class ENTITYQTWIDGETS_EXPORT AssetCreationDialog
      : public QDialog
   {
      Q_OBJECT

   public:
      AssetCreationDialog(const QStringList& datapaths, const QString& filename, const QString& extension);
      ~AssetCreationDialog();
      
      QString GetDataPath() const;
      QString GetMapPath() const;

   private:
      QComboBox* mDataPathChooser;
      QString mExtension;
      QLineEdit* mFileName;
   };
}
