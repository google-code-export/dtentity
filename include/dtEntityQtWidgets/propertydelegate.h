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

#include <QtGui/QtGui>
#include <QtCore/QModelIndex>
#include <QtCore/QObject>

namespace dtEntity
{
   class Property;
}

namespace dtEntityQtWidgets
{
   class DelegateFactory;
   class PropertyEditorView;

   class PropertySubDelegate 
      : public QItemDelegate
   {
      typedef QItemDelegate BaseClass;

   public:
      PropertySubDelegate(QObject* parent = 0)
         : QItemDelegate(parent)
      {
      }

      virtual QIcon GetIcon() const { 
         return QIcon("ProjectAssets:icons/property.png"); 
      }

      virtual void MouseButtonPressed(const QModelIndex& index, int x, int y);

      void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

   private:
      mutable QRect mRemoveButtonArea;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class PropertyEditorDelegate 
      : public QItemDelegate
   {
      Q_OBJECT

   public:
      
     PropertyEditorDelegate(PropertyEditorView* view, QObject *parent = 0);

     
     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

     QSize sizeHint(const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;
     void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

   private:
     PropertyEditorView* mView;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class CharPropertyDelegate
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;
   public:

     CharPropertyDelegate(QObject *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class FloatPropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     FloatPropertyDelegate(QObject *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

   };
   
   ////////////////////////////////////////////////////////////////////////////////
   class StringPropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     StringPropertyDelegate(QObject *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

   };

 
   ////////////////////////////////////////////////////////////////////////////////

   class ArrayPropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     ArrayPropertyDelegate(dtEntity::Property* prototype, QObject *parent = 0);
      ~ArrayPropertyDelegate();

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

     // prototype for creating new array entries
     dtEntity::Property* GetDataPrototype();
     void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

     virtual void MouseButtonPressed(const QModelIndex& index, int x, int y);

   private:
      dtEntity::Property* mDataPrototype;
      mutable QRect mAddButtonArea;
    };

   ////////////////////////////////////////////////////////////////////////////////
   class BoolPropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     BoolPropertyDelegate(QObject *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class ComponentPropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     ComponentPropertyDelegate(QObject *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

   };
      
   ////////////////////////////////////////////////////////////////////////////////
   class DateTimePropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     DateTimePropertyDelegate(QObject *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

     QSize sizeHint(const QStyleOptionViewItem &option,
                              const QModelIndex &index) const;
     void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DoublePropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     DoublePropertyDelegate(QObject *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

   };


   ////////////////////////////////////////////////////////////////////////////////
   class EnumPropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     EnumPropertyDelegate(const QString& values, QObject *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

   private:
      QStringList mValues;
   };
 
   ////////////////////////////////////////////////////////////////////////////////
   class GroupPropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     GroupPropertyDelegate(QObject *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class IntPropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     IntPropertyDelegate(QObject *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

   };
     
   ////////////////////////////////////////////////////////////////////////////////
   class MatrixPropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     MatrixPropertyDelegate(QObject *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class QuatPropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     QuatPropertyDelegate(QObject *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class StringIdPropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     StringIdPropertyDelegate(QObject *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class Vec2PropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     Vec2PropertyDelegate(QObject *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class Vec3PropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     Vec3PropertyDelegate(QObject *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class Vec4PropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     Vec4PropertyDelegate(QObject *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

   };

   ////////////////////////////////////////////////////////////////////////////////

   class FilePathPropertyDelegate;
   class FileSelectorWidget : public QWidget
   {
      Q_OBJECT


   public:
      FileSelectorWidget(const QString& filters, FilePathPropertyDelegate* dlgt, QWidget* parent = 0);
      QLabel* mLabel;

      
     public slots:
        void GetFile();
        void ClearFile();

   private:
      QString mFilters;
      FilePathPropertyDelegate* mDelegate;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class FilePathPropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     FilePathPropertyDelegate(const QString& filters, QWidget *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;

     void FinishedEditing(FileSelectorWidget* editor);

   private:
      QString mFilters;

   };

   ////////////////////////////////////////////////////////////////////////////////

   class ColorPropertyDelegate;
   class ColorSelectorWidget : public QWidget
   {
      Q_OBJECT


   public:
      ColorSelectorWidget(ColorPropertyDelegate* dlgt, QWidget* parent = 0);
      QLabel* mLabel;

      
     public slots:
        void GetColor();

   private:
      ColorPropertyDelegate* mDelegate;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class ColorPropertyDelegate 
      : public PropertySubDelegate
   {
      Q_OBJECT

      typedef PropertySubDelegate BaseClass;

   public:
      
     ColorPropertyDelegate(QWidget *parent = 0);

     QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                           const QModelIndex& index) const;

     void setEditorData(QWidget* editor, const QModelIndex& index) const;
     void setModelData(QWidget* editor, QAbstractItemModel* model,
                       const QModelIndex& index) const;

     void updateEditorGeometry(QWidget* editor,
         const QStyleOptionViewItem& option, const QModelIndex& index) const;
     void FinishedEditing(ColorSelectorWidget*);
   };
}
