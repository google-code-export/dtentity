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

#include <dtEntityQtWidgets/propertydelegate.h>

#include <dtEntityQtWidgets/assetselector.h>
#include <dtEntityQtWidgets/delegatefactory.h>
#include <dtEntityQtWidgets/propertyeditor.h>
#include <iostream>
#include <dtEntity/property.h>
#include <float.h>
#include <sstream>

namespace dtEntityQtWidgets
{
   QLocale* s_locale = new QLocale("en_GB");

   ////////////////////////////////////////////////////////////////////////////////
   QString FormatNumber(double v)
   {
      return QString("%1").arg(v);
   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertySubDelegate* GetSubDelegate(const QModelIndex& index)
   {
      TreeItem* item = static_cast<TreeItem*>(index.internalPointer());    
      PropertyTreeItem* pitem = dynamic_cast<PropertyTreeItem*>(item);  
      if(!pitem) { return NULL; }
      return pitem->mDelegate;      
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertySubDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
   {

      if(index.column() == 2)
      {
         TreeItem* item = static_cast<TreeItem*>(index.parent().internalPointer());
         PropertyTreeItem* pitem = dynamic_cast<PropertyTreeItem*>(item);
         if(   pitem &&
               pitem->mProperty->GetType() == dtEntity::DataType::ARRAY &&
               dynamic_cast<ArrayDelegateFactory*>(item->GetChildDelegateFactory()) != NULL)
         {

            QPushButton b;
            b.setText("-");
            b.resize(20, option.rect.size().height());
            painter->save();
            painter->translate(option.rect.topLeft());
            b.render(painter);
            painter->restore();

            mRemoveButtonArea = b.rect();
            return;
         }
      }

      BaseClass::paint(painter, option, index);

   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertySubDelegate::MouseButtonPressed(const QModelIndex& index, int x, int y)
   {
      if(mRemoveButtonArea.contains(QPoint(x, y)))
      {
         const PropertyEditorModel* model = static_cast<const PropertyEditorModel*>(index.model());
         const_cast<PropertyEditorModel*>(model)->RemoveArrayEntry(index);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertySubDelegate::SetValueByString(dtEntity::Property& prop, const QString& val) const
   {
      prop.SetString(val.toStdString());
   }

   ////////////////////////////////////////////////////////////////////////////////
   QVariant PropertySubDelegate::GetEditableValue(const dtEntity::Property& prop) const
   {
      return prop.StringValue().c_str();
   }

   ////////////////////////////////////////////////////////////////////////////////
   Qt::ItemFlags PropertySubDelegate::GetEditFlags() const
   {
      return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
   }

   ////////////////////////////////////////////////////////////////////////////////
   PropertyEditorDelegate::PropertyEditorDelegate(PropertyEditorView* view, QObject *parent)
     : QItemDelegate(parent)
     , mView(view)
   {
   }   

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* PropertyEditorDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem& optn,
     const QModelIndex& index) const
   {
      QItemDelegate* dlgt = GetSubDelegate(index);
      assert(dlgt);
      connect(dlgt, SIGNAL(commitData(QWidget*)), mView->GetComponentTree(), SLOT(commitData(QWidget*)));
      return dlgt->createEditor(parent, optn, index);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      QItemDelegate* dlgt = GetSubDelegate(index);
      assert(dlgt);
      dlgt->setEditorData(editor, index);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QItemDelegate* dlgt = GetSubDelegate(index);
      assert(dlgt);
      dlgt->setModelData(editor, model, index);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem& option, const QModelIndex& index) const
   {
      QItemDelegate* dlgt = GetSubDelegate(index);
      assert(dlgt);
      dlgt->updateEditorGeometry(editor, option, index);
   }

   ////////////////////////////////////////////////////////////////////////////////
   QSize PropertyEditorDelegate::sizeHint(const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
   {
      QItemDelegate* dlgt = GetSubDelegate(index);
      if(!dlgt)
      {
         return QItemDelegate::sizeHint(option, index);
      }
      return dlgt->sizeHint(option, index);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditorDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
   {
      QItemDelegate* dlgt = GetSubDelegate(index);
      if(!dlgt)
      {
         QItemDelegate::paint(painter, option, index);
         return;
      }
      dlgt->paint(painter, option, index);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   CharPropertyDelegate::CharPropertyDelegate(QObject *parent)
     : PropertySubDelegate(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* CharPropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
     QLineEdit* editor = new QLineEdit(parent);
     editor->setMaxLength(1);
     return editor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharPropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      QString value = index.model()->data(index, Qt::EditRole).toString();
      QLineEdit * e = static_cast<QLineEdit*>(editor);
      e->setText(value);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharPropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QLineEdit* e = static_cast<QLineEdit*>(editor);

      QString value = e->text();
      model->setData(index, value, Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void CharPropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   FloatPropertyDelegate::FloatPropertyDelegate(QObject *parent)
     : PropertySubDelegate(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* FloatPropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
     QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
     editor->setSingleStep(1.0);
     editor->setDecimals(5);
     editor->setMaximum(FLT_MAX);
     editor->setMinimum(-FLT_MAX);
     return editor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FloatPropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      float value = index.model()->data(index, Qt::EditRole).toFloat();
      QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
      spinBox->setValue(value);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FloatPropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
      spinBox->interpretText();
      float value = spinBox->value();
      model->setData(index, value, Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FloatPropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }



   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   StringPropertyDelegate::StringPropertyDelegate(QObject *parent)
     : PropertySubDelegate(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* StringPropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
     QLineEdit* editor = new QLineEdit(parent);
     return editor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void StringPropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      QString value = index.model()->data(index, Qt::EditRole).toString();
      QLineEdit * e = static_cast<QLineEdit*>(editor);
      e->setText(value);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void StringPropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QLineEdit* e = static_cast<QLineEdit*>(editor);
      
      QString value = e->text();
      model->setData(index, value, Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void StringPropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   ArrayPropertyDelegate::ArrayPropertyDelegate(dtEntity::Property* prototype, QObject *parent)
     : PropertySubDelegate(parent)
     , mDataPrototype(prototype)
   {
      
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* ArrayPropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
      assert(false && "Array property not editable!");
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   ArrayPropertyDelegate::~ArrayPropertyDelegate()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayPropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      assert(false && "Array property not editable!");
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayPropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      assert(false && "Array property not editable!");      
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayPropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
      assert(false && "Array property not editable!");
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::Property* ArrayPropertyDelegate::GetDataPrototype()
   {
      return mDataPrototype;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayPropertyDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
   {
     
      if(index.column() == 1)
      {
         QPushButton b;
         b.setText("+");
         b.resize(20, option.rect.size().height());
         painter->save();
         painter->translate(option.rect.topLeft());
         b.render(painter);
         painter->restore(); 

         mAddButtonArea = b.rect();
      }
      else
      {
         BaseClass::paint(painter, option, index);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ArrayPropertyDelegate::MouseButtonPressed(const QModelIndex& index, int x, int y)
   {
      if(mAddButtonArea.contains(QPoint(x, y)))
      {
         const PropertyEditorModel* model = static_cast<const PropertyEditorModel*>(index.model());
         const_cast<PropertyEditorModel*>(model)->AppendArrayEntry(index);
      }
      else
      {
         BaseClass::MouseButtonPressed(index, x, y);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   BoolPropertyDelegate::BoolPropertyDelegate(QObject *parent)
     : PropertySubDelegate(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* BoolPropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
      return new QCheckBox(parent);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BoolPropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      bool value = index.model()->data(index, Qt::EditRole).toBool();
      QCheckBox * e = static_cast<QCheckBox*>(editor);
      e->setChecked(value);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BoolPropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QCheckBox* e = static_cast<QCheckBox*>(editor);
      
      bool value = e->isChecked();
      model->setData(index, value, Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BoolPropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   ComponentPropertyDelegate::ComponentPropertyDelegate(QObject *parent)
     : PropertySubDelegate(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* ComponentPropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {

      QComboBox* editor = new QComboBox(parent);

      const PropertyEditorModel* model = static_cast<const PropertyEditorModel*>(index.model());
      QList<dtEntity::ComponentType> ctypes = model->GetComponentTypesInCurrent();
      QList<dtEntity::ComponentType>::iterator i;
      for(i = ctypes.begin(); i != ctypes.end(); ++i)
      {
         editor->addItem(dtEntity::GetStringFromSID(*i).c_str());
      }

      return editor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ComponentPropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      QString value = index.model()->data(index, Qt::EditRole).toString();
      QComboBox * e = static_cast<QComboBox*>(editor);
      e->setCurrentIndex(e->findText(value));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ComponentPropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QComboBox* e = static_cast<QComboBox*>(editor);
      
      QString value = e->currentText();
      model->setData(index, value, Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ComponentPropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   DateTimePropertyDelegate::DateTimePropertyDelegate(QObject *parent)
     : PropertySubDelegate(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* DateTimePropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
      QDateTimeEdit* e = new QDateTimeEdit(QDateTime::currentDateTime(), parent);
      e->setDisplayFormat("hh:mm:ss dd.MM.yyyy");
      return e;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DateTimePropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      unsigned int value = index.model()->data(index, Qt::EditRole).toUInt();
      QDateTimeEdit* e = static_cast<QDateTimeEdit*>(editor);
      QDateTime dt = QDateTime::fromTime_t(value);
      e->setDateTime(dt);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DateTimePropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QDateTimeEdit* e = static_cast<QDateTimeEdit*>(editor);
      QDateTime dt = e->dateTime();
      model->setData(index, dt.toTime_t(), Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DateTimePropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   QSize DateTimePropertyDelegate::sizeHint(const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
   {
      return QSize(10,20);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DateTimePropertyDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
   {
      /*if(option.state & QStyle::State_Selected){
         painter->fillRect(option.rect, option.palette.color(QPalette::Highlight));
      }*/
      if(index.column() == 1)
      {
         QDateTime dt = QDateTime::fromTime_t(index.data(Qt::DisplayRole).toUInt());
         QString txt = dt.toString("hh:mm:ss dd.MM.yyyy");
         QRect r = option.rect.adjusted(2, 2, -2, -2);
         painter->drawText(r.left(), r.top(), r.width(), r.height(), Qt::AlignVCenter|Qt::AlignLeft|Qt::TextWordWrap, txt, &r);
      }
      else
      {
         BaseClass::paint(painter, option, index);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   DoublePropertyDelegate::DoublePropertyDelegate(QObject *parent)
     : PropertySubDelegate(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* DoublePropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
     QDoubleSpinBox* editor = new QDoubleSpinBox(parent);
     editor->setSingleStep(1.0);
     editor->setDecimals(5);
     editor->setMaximum(DBL_MAX);
     editor->setMinimum(-DBL_MAX);
     return editor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DoublePropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      double value = index.model()->data(index, Qt::EditRole).toDouble();
      QDoubleSpinBox* spinBox = static_cast<QDoubleSpinBox*>(editor);
      spinBox->setValue(value);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DoublePropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
      spinBox->interpretText();
      double value = spinBox->value();
      model->setData(index, value, Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DoublePropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   EnumPropertyDelegate::EnumPropertyDelegate(const QString& values, QObject *parent)
     : PropertySubDelegate(parent)
   {
      mValues = values.split("|");
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* EnumPropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
     QComboBox* editor = new QComboBox(parent);
     editor->insertItems(0, mValues);
     return editor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EnumPropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      QString value = index.model()->data(index, Qt::EditRole).toString();
      QComboBox* e = static_cast<QComboBox*>(editor);
      e->setCurrentIndex(e->findText(value));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EnumPropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QComboBox* e = static_cast<QComboBox*>(editor);      
      QString value = e->currentText();
      model->setData(index, value, Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void EnumPropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   SwitchPropertyDelegate::SwitchPropertyDelegate(QObject *parent)
     : PropertySubDelegate(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* SwitchPropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
      QComboBox* editor = new QComboBox(parent);

      TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
      PropertyTreeItem* pitem = dynamic_cast<PropertyTreeItem*>(item);
      assert(pitem);

      dtEntity::PropertyGroup grp = pitem->mProperty->GroupValue();

      dtEntity::StringId selindex;

      dtEntity::PropertyGroup::iterator sel = grp.find(dtEntity::SIDHash("__SELECTED__"));
      if(sel != grp.end())
      {
         selindex = sel->second->StringIdValue();
      }

      for(int i = 0; i < item->childCount(); ++i)
      {
         TreeItem* child = item->child(i);

         PropertyTreeItem* pchild = dynamic_cast<PropertyTreeItem*>(child);
         if(pchild && pchild->mName.left(2) != "__")
         {
            editor->addItem(pchild->mName);
            if(dtEntity::SIDHash(pchild->mName.toStdString()) == selindex)
            {
               editor->setCurrentIndex(editor->count() - 1);
            }
         }
      }
      return editor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SwitchPropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
      PropertyTreeItem* pitem = dynamic_cast<PropertyTreeItem*>(item);
      assert(pitem);

      dtEntity::PropertyGroup grp = pitem->mProperty->GroupValue();
      dtEntity::StringId selindex;

      dtEntity::PropertyGroup::iterator sel = grp.find(dtEntity::SIDHash("__SELECTED__"));
      if(sel != grp.end())
      {
         selindex = sel->second->StringIdValue();
      }

      QComboBox* e = static_cast<QComboBox*>(editor);
      for(int i = 0; i < e->count(); ++i)
      {
         if(dtEntity::SIDHash(e->itemText(i).toStdString()) == selindex)
         {
            e->setCurrentIndex(i);
            break;
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SwitchPropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QComboBox* e = static_cast<QComboBox*>(editor);
      QString value = e->currentText();

      TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
      PropertyTreeItem* pitem = dynamic_cast<PropertyTreeItem*>(item);
      assert(pitem);
      SetValueByString(*pitem->mProperty, value);
      dynamic_cast<PropertyEditorModel*>(model)->SwitchChanged(index);

      pitem->mChanged = true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SwitchPropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void SwitchPropertyDelegate::SetValueByString(dtEntity::Property& prop, const QString& val) const
   {
      assert(prop.GetType() == dtEntity::DataType::GROUP);
      dtEntity::PropertyGroup grp = prop.GroupValue();
      dtEntity::PropertyGroup::iterator sel = grp.find(dtEntity::SIDHash("__SELECTED__"));
      if(sel != grp.end())
      {
         sel->second->SetStringId(dtEntity::SIDHash(val.toStdString()));
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   QVariant SwitchPropertyDelegate::GetEditableValue(const dtEntity::Property& prop) const
   {
      assert(prop.GetType() == dtEntity::DataType::GROUP);

      dtEntity::PropertyGroup grp = prop.GroupValue();

      dtEntity::PropertyGroup::iterator sel = grp.find(dtEntity::SIDHash("__SELECTED__"));
      if(sel != grp.end())
      {
         QString str = dtEntity::GetStringFromSID(sel->second->StringIdValue()).c_str();
         return str;
      }
      return "";
   }


   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   GroupPropertyDelegate::GroupPropertyDelegate(QObject *parent)
     : PropertySubDelegate(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* GroupPropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
     QLineEdit* editor = new QLineEdit(parent);
     return editor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GroupPropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      QString value = index.model()->data(index, Qt::EditRole).toString();
      QLineEdit * e = static_cast<QLineEdit*>(editor);
      e->setText(value);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GroupPropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QLineEdit* e = static_cast<QLineEdit*>(editor);
      
      QString value = e->text();
      model->setData(index, value, Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GroupPropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }
 
   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   IntPropertyDelegate::IntPropertyDelegate(QObject *parent)
     : PropertySubDelegate(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* IntPropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
     QLineEdit* editor = new QLineEdit(parent);
     return editor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IntPropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      QString value = index.model()->data(index, Qt::EditRole).toString();
      QLineEdit * e = static_cast<QLineEdit*>(editor);
      e->setText(value);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IntPropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QLineEdit* e = static_cast<QLineEdit*>(editor);
      
      QString value = e->text();
      model->setData(index, value, Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void IntPropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   MatrixPropertyDelegate::MatrixPropertyDelegate(QObject *parent)
     : PropertySubDelegate(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* MatrixPropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
     QLineEdit* editor = new QLineEdit(parent);
     return editor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MatrixPropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      QString value = index.model()->data(index, Qt::EditRole).toString();
      QLineEdit * e = static_cast<QLineEdit*>(editor);
      e->setText(value);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MatrixPropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QLineEdit* e = static_cast<QLineEdit*>(editor);
      
      QString value = e->text();
      model->setData(index, value, Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void MatrixPropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }
   
   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   QuatPropertyDelegate::QuatPropertyDelegate(QObject *parent)
     : PropertySubDelegate(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* QuatPropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
     QLineEdit* editor = new QLineEdit(parent);
     return editor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void QuatPropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      QString value = index.model()->data(index, Qt::EditRole).toString();
      QLineEdit * e = static_cast<QLineEdit*>(editor);
      e->setText(value);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void QuatPropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QLineEdit* e = static_cast<QLineEdit*>(editor);
      
      QString value = e->text();
      model->setData(index, value, Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void QuatPropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   StringIdPropertyDelegate::StringIdPropertyDelegate(QObject *parent)
     : PropertySubDelegate(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* StringIdPropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
     QLineEdit* editor = new QLineEdit(parent);
     return editor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void StringIdPropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      QString value = index.model()->data(index, Qt::EditRole).toString();
      QLineEdit * e = static_cast<QLineEdit*>(editor);
      e->setText(value);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void StringIdPropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QLineEdit* e = static_cast<QLineEdit*>(editor);
      
      QString value = e->text();
      model->setData(index, value, Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void StringIdPropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   Vec2PropertyDelegate::Vec2PropertyDelegate(QObject *parent)
     : PropertySubDelegate(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* Vec2PropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
     QLineEdit* editor = new QLineEdit(parent);
     return editor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec2PropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      QString value = index.model()->data(index, Qt::EditRole).toString();
      QLineEdit * e = static_cast<QLineEdit*>(editor);
      e->setText(value);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec2PropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QLineEdit* e = static_cast<QLineEdit*>(editor);
      
      QString value = e->text();
      model->setData(index, value, Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec2PropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   Vec3PropertyDelegate::Vec3PropertyDelegate(QObject *parent)
     : PropertySubDelegate(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* Vec3PropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
     QLineEdit* editor = new QLineEdit(parent);
     return editor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec3PropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      QString value = index.model()->data(index, Qt::EditRole).toString();
      QLineEdit * e = static_cast<QLineEdit*>(editor);
      e->setText(value);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec3PropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QLineEdit* e = static_cast<QLineEdit*>(editor);
      
      QString value = e->text();
      model->setData(index, value, Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec3PropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////
   Vec4PropertyDelegate::Vec4PropertyDelegate(QObject *parent)
     : PropertySubDelegate(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* Vec4PropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
     QLineEdit* editor = new QLineEdit(parent);
     return editor;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec4PropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      QString value = index.model()->data(index, Qt::EditRole).toString();
      QLineEdit * e = static_cast<QLineEdit*>(editor);
      e->setText(value);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec4PropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      QLineEdit* e = static_cast<QLineEdit*>(editor);
      
      QString value = e->text();
      model->setData(index, value, Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void Vec4PropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////

   FileSelectorWidget::FileSelectorWidget(const QString& filters, FilePathPropertyDelegate* dlgt, QWidget* parent)
      : QWidget(parent)
      , mFilters(filters)
      , mDelegate(dlgt)
   {
      setFocusPolicy(Qt::StrongFocus);
	  
      QHBoxLayout* horizontalLayout = new QHBoxLayout(this);    
      horizontalLayout->setContentsMargins(0, 0, 0, 0);
      mLabel = new QLabel();
      horizontalLayout->addWidget(mLabel);
      QPushButton* choosebutton = new QPushButton();
      choosebutton->setText("Choose");
      choosebutton->setMaximumHeight(15);
      horizontalLayout->addWidget(choosebutton);
      connect(choosebutton, SIGNAL(clicked()), this, SLOT(GetFile()));

      QPushButton* clearbutton = new QPushButton();
      clearbutton->setText("Clear");
      clearbutton->setMaximumHeight(15);
      horizontalLayout->addWidget(clearbutton);
      connect(clearbutton, SIGNAL(clicked()), this, SLOT(ClearFile()));
      
      QPalette palette;
      QColor color(255, 255, 255);
      palette.setColor(QPalette::Active,   QPalette::Background, color);
      palette.setColor(QPalette::Inactive, QPalette::Background, color);
      palette.setColor(QPalette::Disabled, QPalette::Background, color) ;
      setPalette(palette);
      
      setAutoFillBackground(true);

   }

   ////////////////////////////////////////////////////////////////////////////////
   FileSelectorWidget::~FileSelectorWidget()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FileSelectorWidget::GetFile()
   {
      bool hidden = this->isHidden();
      AssetSelector sel (mFilters, this);
      if(sel.exec() == QDialog::Accepted)
      {
         if(!hidden)
         {
            mLabel->setText(sel.GetSelected());
         }
         mDelegate->FinishedEditing(this);
      }
      sel.close();
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FileSelectorWidget::ClearFile()
   {
      mLabel->setText("");
      mDelegate->FinishedEditing(this);
   }

   ////////////////////////////////////////////////////////////////////////////////
   FilePathPropertyDelegate::FilePathPropertyDelegate(const QString& filters, QWidget *parent)
      : PropertySubDelegate(parent)
      , mFilters(filters)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* FilePathPropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
      return new FileSelectorWidget(mFilters, const_cast<FilePathPropertyDelegate*>(this), parent);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FilePathPropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      QString value = index.model()->data(index, Qt::EditRole).toString();
      FileSelectorWidget* e = static_cast<FileSelectorWidget*>(editor);
      e->mLabel->setText(value);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FilePathPropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      FileSelectorWidget* e = static_cast<FileSelectorWidget*>(editor);
      QString value = e->mLabel->text();
      model->setData(index, value, Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FilePathPropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void FilePathPropertyDelegate::FinishedEditing(FileSelectorWidget* editor)
   {
      //TODO this does not cause the data to be submitted to the model. Why?
      emit(commitData(editor));
      emit(closeEditor(editor, QAbstractItemDelegate::EditNextItem));
   }


   ////////////////////////////////////////////////////////////////////////////////
   ////////////////////////////////////////////////////////////////////////////////

   ColorSelectorWidget::ColorSelectorWidget(ColorPropertyDelegate* dlgt, QWidget* parent)
      : QWidget(parent)
      , mDelegate(dlgt)
   {
      setFocusPolicy(Qt::StrongFocus);
	  
		QHBoxLayout* horizontalLayout = new QHBoxLayout(this);
      horizontalLayout->setContentsMargins(0, 0, 0, 0);
      mLabel = new QLabel();
      horizontalLayout->addWidget(mLabel);
      QPushButton* button = new QPushButton();
      button->setText("Choose");
      button->setMaximumHeight(15);
      horizontalLayout->addWidget(button);
      connect(button, SIGNAL(clicked()), this, SLOT(GetColor()));
      
      QPalette palette;
      QColor color(255, 255, 255);
      palette.setColor(QPalette::Active,   QPalette::Background, color);
      palette.setColor(QPalette::Inactive, QPalette::Background, color);
      palette.setColor(QPalette::Disabled, QPalette::Background, color) ;
      setPalette(palette);
      
      setAutoFillBackground(true);

   }

   ////////////////////////////////////////////////////////////////////////////////
   void ColorSelectorWidget::GetColor()
   {
      
      QString currColor = mLabel->text();
      QStringList l = currColor.split(" ");
      QColor current;
      if(l.size() == 4)
      { 
         current.setRgbF(l[0].toFloat(), l[1].toFloat(), l[2].toFloat(), l[3].toFloat());
      }

      QColorDialog d;
      QColor c = d.getColor(current, this, "Get Color", QColorDialog::ShowAlphaChannel);
      
      mLabel->setText(QString("%1 %2 %3 %4")
         .arg(c.redF()).arg(c.greenF()).arg(c.blueF()).arg(c.alphaF()));
      mDelegate->FinishedEditing(this);      
   }

   ////////////////////////////////////////////////////////////////////////////////
   ColorPropertyDelegate::ColorPropertyDelegate(QWidget *parent)
      : PropertySubDelegate(parent)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   QWidget* ColorPropertyDelegate::createEditor(QWidget* parent,
     const QStyleOptionViewItem&/* option */,
     const QModelIndex& index) const
   {
      return new ColorSelectorWidget(const_cast<ColorPropertyDelegate*>(this), parent);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ColorPropertyDelegate::setEditorData(QWidget *editor,
                                     const QModelIndex &index) const
   {
      QString value = index.model()->data(index, Qt::EditRole).toString();
      ColorSelectorWidget* e = static_cast<ColorSelectorWidget*>(editor);
      e->mLabel->setText(value);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ColorPropertyDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
   {
      ColorSelectorWidget* e = static_cast<ColorSelectorWidget*>(editor);
      QString value = e->mLabel->text();
      model->setData(index, value, Qt::EditRole);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ColorPropertyDelegate::updateEditorGeometry(QWidget *editor,
     const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
   {
     editor->setGeometry(option.rect);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ColorPropertyDelegate::FinishedEditing(ColorSelectorWidget* editor)
   {

      //TODO this does not cause the data to be submitted to the model. Why?
      emit(commitData(editor));
      emit(closeEditor(editor, QAbstractItemDelegate::SubmitModelCache));
   }
}
