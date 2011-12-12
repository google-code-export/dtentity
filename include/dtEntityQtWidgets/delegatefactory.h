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


#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <dtEntity/property.h>

namespace dtEntityQtWidgets
{
   ////////////////////////////////////////////////////////////////////////////////
   class Translator
   {
   public:
      Translator()
      {
      }

      QMap<QString, QString> mPropertyNames;
      QMap<QString, QString> mDescriptions;

      QString mComponentName;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class TreeItem;
   class PropertySubDelegate;

   class DelegateFactory
   {
   public:     
   
      DelegateFactory();     
      virtual ~DelegateFactory();     
      virtual PropertySubDelegate* Create(TreeItem* parent, const QString& propname, const dtEntity::Property* prop) const;
      DelegateFactory* GetFactoryForChildren(const QString& propname);
      void SetFactoryForChildren(const QString& propname, DelegateFactory* factory);
      PropertySubDelegate* CreateDefault(dtEntity::DataType::e dtype) const;
      virtual bool AllowCreateFromGUI() const { return mAllowCreateFromGUI; }
      void SetAllowCreateFromGUI(bool v) { mAllowCreateFromGUI = v; }
   protected:
      QMap<QString, DelegateFactory*> mChildFactories;
      bool mAllowCreateFromGUI;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class DateTimeDelegateFactory : public DelegateFactory
   {
   public:
      DateTimeDelegateFactory();
      virtual PropertySubDelegate* Create(TreeItem* parent, const QString& propname, const dtEntity::Property* prop) const;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class SwitchDelegateFactory : public DelegateFactory
   {
   public:
      SwitchDelegateFactory(const QMap<QString, dtEntity::Property*>& groups);
      virtual PropertySubDelegate* Create(TreeItem* parent, const QString& propname, const dtEntity::Property* prop) const;

   private:

      QMap<QString, dtEntity::Property*> mGroups;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class FileSelectorDelegateFactory : public DelegateFactory
   {
   public:
      FileSelectorDelegateFactory(const QString& filters);
      virtual PropertySubDelegate* Create(TreeItem* parent, const QString& propname, const dtEntity::Property* prop) const;

   private:
      QString mFilters;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class ColorSelectorDelegateFactory : public DelegateFactory
   {
   public:
      ColorSelectorDelegateFactory();
      virtual PropertySubDelegate* Create(TreeItem* parent, const QString& propname, const dtEntity::Property* prop) const;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class ComponentDelegateFactory : public DelegateFactory
   {
   public:
      ComponentDelegateFactory();
      virtual PropertySubDelegate* Create(TreeItem* parent, const QString& propname, const dtEntity::Property* prop) const;

   };

   ////////////////////////////////////////////////////////////////////////////////
   class EnumDelegateFactory : public DelegateFactory
   {
   public:
      EnumDelegateFactory(const QString& values);
      virtual PropertySubDelegate* Create(TreeItem* parent, const QString& propname, const dtEntity::Property* prop) const;
   private:
      QString mValues;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class ArrayDelegateFactory : public DelegateFactory
   {
   public:
      ArrayDelegateFactory();
      ~ArrayDelegateFactory();
      virtual PropertySubDelegate* Create(TreeItem* parent, const QString& propname, const dtEntity::Property* prop) const;
      virtual DelegateFactory* GetFactoryForChildren(const QString& propname);
      dtEntity::Property* mDataPrototype;
   };

   ////////////////////////////////////////////////////////////////////////////////
   class RootDelegateFactory : public DelegateFactory
   {
   public:
      RootDelegateFactory();
      virtual PropertySubDelegate* Create(TreeItem* parent, const QString& propname, const dtEntity::Property* prop) const;
   };

   ////////////////////////////////////////////////////////////////////////////////
   bool CreateDelegateFactory(const QString& componentname, DelegateFactory*&, Translator*&);

}
