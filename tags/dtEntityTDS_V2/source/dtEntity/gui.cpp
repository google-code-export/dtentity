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

#include <dtEntity/gui.h>
#include <dtEntity/scriptmodule.h>
#include <dtEntity/resourceprovider.h>
#include <dtEntity/log.h>
#include <osgDB/FileUtils>
#include <dtEntity/basemessages.h>
#include <dtEntity/entitymanager.h>
#include <osg/Drawable>
#include <osg/StateSet>
#include <osg/Geode>
#include <osg/Texture2D>
#include <osgDB/FileNameUtils>
#include <osgViewer/GraphicsWindow>
#include <dtEntity/applicationcomponent.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/RendererModules/OpenGL/CEGUIOpenGLRenderer.h>  // for base class
#include <CEGUI/CEGUIDefaultResourceProvider.h>
#include <CEGUI/CEGUIImageset.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIScriptModule.h>
#include <CEGUI/CEGUIScheme.h>
#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUIFont.h>
#include <CEGUI/CEGUIPropertyHelper.h>
#include <CEGUI/CEGUITexture.h>
#include <CEGUI/CEGUILogger.h>
#include <CEGUI/RendererModules/OpenGL/CEGUIOpenGLTexture.h>
#include <CEGUI/falagard/CEGUIFalWidgetLookManager.h>
#include <osgViewer/View>

namespace dtEntity
{

   /**
   * @brief
   *     an osg::Drawable which will render a provided(defined via the constructor) gui
   */
   class HUDDrawable : public osg::Drawable
   {
   public:

      ///copy constructor
      HUDDrawable(const HUDDrawable& drawable, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
      {
         this->setSupportsDisplayList(false);
         this->setUseDisplayList(false);
         setDataVariance(osg::Object::DYNAMIC);
      }

      ///constructs an GUIdrawable-object and assign an HUD-object
      HUDDrawable()
      {
         this->setSupportsDisplayList(false);
         this->setUseDisplayList(false);
         setDataVariance(osg::Object::DYNAMIC);
      }

      ///implementation of osg::Object
      virtual osg::Object* cloneType() const
      {
         return new HUDDrawable();
      }

      ///implementation of osg::Object
      virtual osg::Object* clone(const osg::CopyOp& copyop) const
      {
         return new HUDDrawable(*this, copyop);
      }

      ///draw-implementation of osg::Object
      virtual void drawImplementation(osg::RenderInfo & renderInfo) const
      {
         osg::State* state = renderInfo.getState();

         const CEGUI::Size& ceguiSize = CEGUI::System::getSingleton().getRenderer()->getDisplaySize();
         if (ceguiSize.d_width != state->getCurrentViewport()->width() ||
             ceguiSize.d_height != state->getCurrentViewport()->height())
         {
            CEGUI::System::getSingleton().notifyDisplaySizeChanged(CEGUI::Size(state->getCurrentViewport()->width(), state->getCurrentViewport()->height()));
         }

         state->disableAllVertexArrays();
         state->setActiveTextureUnit(0);
         state->setClientActiveTextureUnit(0);

         CEGUI::System::getSingletonPtr()->renderGUI();
      }

   protected:

      ///destructs an HUDDrawable-object
      virtual ~HUDDrawable()
      {}

   private:
   };



   //////////////////////////////////////////////////////////////////////////
   //CEGUI Logger
   //////////////////////////////////////////////////////////////////////////
   class CEGUILogger : public CEGUI::Logger
   {
      void logEvent(const CEGUI::String& message, CEGUI::LoggingLevel logLevel)
      {
         
         if(logLevel == CEGUI::Errors)
         {
            LOG_ERROR(message.c_str());
         }
         else if(logLevel == CEGUI::Warnings)
         {
            LOG_WARNING(message.c_str());
         }
         else if(logLevel == CEGUI::Standard)
         {
            LOG_INFO(message.c_str());
         }
         else
         {
            LOG_DEBUG(message.c_str());
         }

      }

      void setLogFilename(const CEGUI::String& str, bool b)
      {
         //we dont need to do anything here
      }
   };

   //declaring this overrides the default logger for CEGUI
   CEGUILogger CEGUILogger;


   ////////////////////////////////////////////////////////////////////////////////
   // STATIC VARIABLES & OBJECTS
   ////////////////////////////////////////////////////////////////////////////////
   dtEntity::ResourceProvider mResProvider;
   bool dtEntity::GUI::SystemAndRendererCreatedByHUD = false;



   ////////////////////////////////////////////////////////////////////////////////
   // GUI IMPLEMENTATION
   ////////////////////////////////////////////////////////////////////////////////
   GUI::GUI(dtEntity::EntityManager& em, osg::Camera* camera)
     : mRootSheet(NULL)
     , mEntityManager(&em)
   {
      ApplicationSystem* appsys;
      mEntityManager->GetEntitySystem(ApplicationSystem::TYPE, appsys);
      appsys->GetPrimaryView()->addEventHandler(this);
      appsys->GetPrimaryWindow()->makeCurrent();

      _SetupInternalGraph();

      SetCamera(camera);
     
      _SetupSystemAndRenderer();
      _SetupDefaultUI();  
      
   }

   ////////////////////////////////////////////////////////////////////////////////
   GUI::~GUI()
   {

      ApplicationSystem* appsys;
      if(mEntityManager->GetEntitySystem(ApplicationSystem::TYPE, appsys))
      {
         appsys->GetPrimaryView()->removeEventHandler(this);
      }

      if (mCamera.valid() && mInternalGraph.valid())
      {
         mCamera->removeChild(mInternalGraph.get());
      }

      if (mRootSheet)
      {
         CEGUI::WindowManager::getSingletonPtr()->destroyWindow(mRootSheet);
         mRootSheet = NULL;
      }

      CEGUI::OpenGLRenderer* renderer = static_cast<CEGUI::OpenGLRenderer*>(CEGUI::System::getSingletonPtr()->getRenderer());
      CEGUI::System::destroy();
      if (renderer)
      {
         CEGUI::OpenGLRenderer::destroy(*renderer);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GUI::_SetupInternalGraph()
   {
      osg::Camera* camera = new osg::Camera();

      //don't clear the color buffer (allows the UI to be superimposed on the scene)
      camera->setClearMask(GL_DEPTH_BUFFER_BIT);
      camera->setRenderOrder(osg::Camera::POST_RENDER, 100);

      // we don't want the camera to grab event focus from the viewers main camera(s).
      camera->setAllowEventFocus(false);

      mInternalGraph = camera;

      osg::StateSet* states = mInternalGraph->getOrCreateStateSet();

      //m_pInternalGraph->setName("internal_GUI_Geode");
      states->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
      states->setMode(GL_BLEND, osg::StateAttribute::ON);
      states->setTextureMode(0, GL_TEXTURE_2D, osg::StateAttribute::ON);

      osg::Geode* geode = new osg::Geode;
      geode->addDrawable(new HUDDrawable());
      mInternalGraph->addChild(geode);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GUI::SetCamera(osg::Camera* camera)
   {
      // if this was already a child of another camera remove itself from there:
      if (mCamera.valid())
      {
         mCamera->removeChild(mInternalGraph.get());
      }

      // set ("parent") camera
      mCamera = camera;

      // that'll force the camera to draw this gui via the HUDDrawable-object
      if (mCamera.valid())
      {
         mCamera->addChild(mInternalGraph.get());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GUI::_SetupSystemAndRenderer()
   {
      if (!CEGUI::System::getSingletonPtr())
      {
         CEGUI::OpenGLRenderer& renderer = CEGUI::OpenGLRenderer::create();
         renderer.enableExtraStateSettings(true);
         CEGUI::System::create(renderer, &mResProvider);

         //CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());

         CEGUI::Imageset::setDefaultResourceGroup("imagesets");
   //      SetResourceGroupDirectory("imagesets", dtUtil::FindFileInPathList("imagesets"));
   //
         CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeel");
   //      SetResourceGroupDirectory("looknfeels", dtUtil::FindFileInPathList("looknfeel"));
   //
         CEGUI::WindowManager::setDefaultResourceGroup("layouts");
   //      SetResourceGroupDirectory("layouts", dtUtil::FindFileInPathList("layouts"));
   //
         CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
   //      SetResourceGroupDirectory("lua_scripts", dtUtil::FindFileInPathList("lua_scripts"));
   //
         CEGUI::Scheme::setDefaultResourceGroup("schemes");
   //      SetResourceGroupDirectory("schemes", dtUtil::FindFileInPathList("schemes"));
   //
         CEGUI::Font::setDefaultResourceGroup("fonts");
   //      SetResourceGroupDirectory("fonts", dtUtil::FindFileInPathList("fonts"));

         if (CEGUI::System::getSingletonPtr() == NULL)
         {
            return;
         }

         dtEntity::ResourceProvider* rp = dynamic_cast<dtEntity::ResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());

         if (rp != NULL)
         {
            rp->AddSearchSuffix("imagesets", "");
            rp->AddSearchSuffix("imagesets", "CEGUI/imagesets");
            rp->AddSearchSuffix("imagesets", "imagesets");
            rp->AddSearchSuffix("imagesets", "gui/imagesets");
            rp->AddSearchSuffix("looknfeel", "");
            rp->AddSearchSuffix("looknfeel", "CEGUI/looknfeel");
            rp->AddSearchSuffix("looknfeel", "looknfeel");
            rp->AddSearchSuffix("looknfeel", "gui/looknfeel");
            rp->AddSearchSuffix("layouts", "");
            rp->AddSearchSuffix("layouts", "CEGUI/layouts");
            rp->AddSearchSuffix("layouts", "layouts");
            rp->AddSearchSuffix("layouts", "gui/layouts");
            rp->AddSearchSuffix("lua_scripts", "");
            rp->AddSearchSuffix("lua_scripts", "CEGUI/lua_scripts");
            rp->AddSearchSuffix("lua_scripts", "lua_scripts");
            rp->AddSearchSuffix("lua_scripts", "gui/lua_scripts");
            rp->AddSearchSuffix("schemes", "");
            rp->AddSearchSuffix("schemes", "CEGUI/schemes");
            rp->AddSearchSuffix("schemes", "schemes");
            rp->AddSearchSuffix("schemes", "gui/schemes");
            rp->AddSearchSuffix("fonts", "");
            rp->AddSearchSuffix("fonts", "CEGUI/fonts");
            rp->AddSearchSuffix("fonts", "fonts");
            rp->AddSearchSuffix("fonts", "gui/fonts");
            rp->AddSearchSuffix("", "");
            rp->AddSearchSuffix("", "CEGUI");
            rp->AddSearchSuffix("", "gui");
         }

         SystemAndRendererCreatedByHUD = true;
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GUI::_SetupDefaultUI()
   {
      //generate unique root-window-name:
      std::string generatedUniquePrefix="";

      //generate unqiue prefix
      for (unsigned int i = 0; i < 65000; i++)
      {
         std::stringstream ssTryName;
         ssTryName << "gui" << i;
         if (!CEGUI::WindowManager::getSingleton().isWindowPresent(ssTryName.str()+"rootsheet"))
         {
            generatedUniquePrefix = ssTryName.str();
            break;
         }
      }
      if (generatedUniquePrefix=="")
      {
         LOG_ERROR("cannot generate prefix");
         return;
      }

      mRootSheet = CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow", std::string(generatedUniquePrefix + "rootsheet").c_str());
      mRootSheet->setMousePassThroughEnabled(true);
   #if CEGUI_VERSION_MAJOR >= 0 && CEGUI_VERSION_MINOR >= 7 && CEGUI_VERSION_PATCH > 1
      mRootSheet->setMouseInputPropagationEnabled(true);
   #endif
      CEGUI::System::getSingleton().setGUISheet(mRootSheet);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool GUI::IsSchemePresent(const std::string& schemeName)
   {
      _SetupSystemAndRenderer();
      return CEGUI::SchemeManager::getSingletonPtr()->isDefined(schemeName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GUI::LoadScheme(const std::string& fileName, const std::string& resourceGroup)
   {
      _SetupSystemAndRenderer();
      CEGUI::SchemeManager::getSingleton().create(fileName, resourceGroup);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GUI::SetMouseCursor(const std::string& imagesetName, const std::string& imageName)
   {
      _SetupSystemAndRenderer();

      if (!imagesetName.empty() && !imageName.empty())
      {
         CEGUI::System::getSingletonPtr()->setDefaultMouseCursor(imagesetName, imageName);
      }
      else
      {
         CEGUI::System::getSingletonPtr()->setDefaultMouseCursor(CEGUI::BlankMouseCursor);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GUI::ShowCursor()
   {
      _SetupSystemAndRenderer();
      CEGUI::MouseCursor::getSingleton().show();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GUI::HideCursor()
   {
      _SetupSystemAndRenderer();
      CEGUI::MouseCursor::getSingleton().hide();
   }

   ////////////////////////////////////////////////////////////////////////////////
   CEGUI::Window* GUI::LoadLayout(const std::string& fileName, const std::string& prefix, const std::string& resourceGroup)
   {
      return LoadLayout(mRootSheet, fileName, prefix, resourceGroup);
   }

   ////////////////////////////////////////////////////////////////////////////////
   CEGUI::Window* GUI::LoadLayout(Widget* parent, const std::string& fileName,
                                  const std::string& prefix,
                                  const std::string& resourceGroup)
   {
      CEGUI::Window* layout = CEGUI::WindowManager::getSingleton().loadWindowLayout(fileName, prefix, resourceGroup);

      parent->addChildWindow(layout);

      if (layout)
      {
         mLayoutMap[fileName] = layout;
      }

      return layout;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Widget* GUI::CreateWidget(Widget* parent, const std::string& typeName,
                                    const std::string& name)
   {
      Widget* newWidget = CreateWidget(typeName, name);
      if (parent)
      {
         parent->addChildWindow(newWidget);
      }
      return newWidget;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GUI::DestroyWidget(Widget* widget)
   {
      if(widget->getParent() != NULL)
      {
         widget->getParent()->removeChildWindow(widget);
      }
      CEGUI::WindowManager::getSingleton().destroyWindow(widget);
   }

   ////////////////////////////////////////////////////////////////////////////////
   Widget* GUI::CreateWidget(const std::string& typeName, const std::string& name)
   {
      CEGUI::Window* window = CEGUI::WindowManager::getSingleton().createWindow(typeName, name);

      mRootSheet->addChildWindow(window);

      return window;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GUI::SetResourceGroupDirectory(const std::string& resourceType, const std::string& directory)
   {
      if (CEGUI::System::getSingletonPtr() == NULL)
      {
         return;
      }

      CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>(CEGUI::System::getSingleton().getResourceProvider());

      rp->setResourceGroupDirectory(resourceType, directory);
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string GUI::SetResourceGroupFromResource(const std::string& resourceGroup,
                                                        const std::string& resourceToFind)
   {
      //using data file search paths, find the resource and set the resourceGroup to that path
      const std::string fullPath = osgDB::findDataFile(resourceToFind);
      if (fullPath.empty())
      {
         //file not found
         return std::string();
      }

      const std::string path = osgDB::getFilePath(fullPath);

      SetResourceGroupDirectory(resourceGroup, path);
      return path;
   }


   ////////////////////////////////////////////////////////////////////////////////
   Widget* GUI::GetWidget(const std::string& name)
   {
      if (!CEGUI::WindowManager::getSingleton().isWindowPresent(name))
      {
         LOG_ERROR(name + " is not available in guin");
         return 0;
      }

      return CEGUI::WindowManager::getSingleton().getWindow(name);
   }


   ////////////////////////////////////////////////////////////////////////////////
   void GUI::FindWidgets(const std::string& subName, std::vector<Widget*>& toFill)
   {
      for (CEGUI::WindowManager::WindowIterator iter = CEGUI::WindowManager::getSingleton().getIterator(); !iter.isAtEnd(); ++iter)
      {
         Widget* win = iter.getCurrentValue();

         if (win->getName().find(subName) != CEGUI::String::npos)
         {
            toFill.push_back(win);
         }
      }

      if (!toFill.empty())
      {
         return;
      }

      LOG_ERROR(subName + " is not available in gui\n");
      return;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Widget* GUI::FindWidget(const std::string& subName)
   {
      std::vector<Widget*> toFill;
      FindWidgets(subName, toFill);

      if (!toFill.empty())
      {
         return toFill.front();
      }

      LOG_ERROR(subName + " is not available in gui");
      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   Widget* GUI::GetRootSheet()
   {
      return mRootSheet;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const Widget* GUI::GetRootSheet() const
   {
      return mRootSheet;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GUI::SetScriptModule(BaseScriptModule* scriptModule)
   {
      if (CEGUI::System::getSingletonPtr())
      {
         CEGUI::System::getSingletonPtr()->setScriptingModule(scriptModule);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   BaseScriptModule* GUI::GetScriptModule()
   {
      if (CEGUI::System::getSingletonPtr())
      {
         return CEGUI::System::getSingletonPtr()->getScriptingModule();
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Group& GUI::GetRootNode()
   {
      return *mInternalGraph;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const osg::Group& GUI::GetRootNode() const
   {
      return *mInternalGraph;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool GUI::SetDefaultParser(const std::string& parserName)
   {
      _SetupSystemAndRenderer();
      CEGUI::System& sys = CEGUI::System::getSingleton();
      const CEGUI::String currentParserName = sys.getDefaultXMLParserName();

      bool success = false;
      try
      {
         sys.setXMLParser(parserName);
         success = true;
      }
      catch(CEGUI::Exception& e)
      {
         LOG_WARNING("CEGUI system will use the original default XML parser \""
            << currentParserName.c_str() << "\" because the system could not link to use XML parser \""
            << parserName.c_str() << "\" because of the following CEGUI exception ("
            << e.getName().c_str() << "):\n"
            << e.getMessage().c_str() << "\n");
      }
      catch(...)
      {
         LOG_ERROR("CEGUI system will use the original default XML parser \""
            << currentParserName.c_str() << "\" because the system could not link to use XML parser \""
            << parserName << "\" because of some unknown exception.");
      }

      // If the intended parser assignment failed, ensure the last parser used is assigned.
      if( ! success)
      {
         sys.setXMLParser(currentParserName);
      }

      return success;
   }

   ////////////////////////////////////////////////////////////////////////////////
   CEGUI::Event::Connection GUI::SubscribeEvent(const std::string& widgetName,
                                                const std::string& event,
                                                GUI::Subscriber subscriber)
   {
      CEGUI::Window* window = GetWidget(widgetName);

      if (window)
         return SubscribeEvent(*window, event, subscriber);

      LOG_ERROR("Could not find widget for event subscription: " + widgetName);
      return CEGUI::Event::Connection();
   }

   ////////////////////////////////////////////////////////////////////////////////
   CEGUI::Event::Connection GUI::SubscribeEvent(CEGUI::Window& window,
                                                const std::string& event,
                                                GUI::Subscriber subscriber)
   {
      return window.subscribeEvent(event, subscriber);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool GUI::IsWindowPresent(const std::string& widgetName)
   {
      _SetupSystemAndRenderer();
      return CEGUI::WindowManager::getSingleton().isWindowPresent(widgetName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool GUI::IsImagesetPresent(const std::string& imagesetName)
   {
      _SetupSystemAndRenderer();
      return CEGUI::ImagesetManager::getSingleton().isDefined(imagesetName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GUI::CreateImageset(const std::string& imagesetName,
                                   const std::string& fileName,
                                   const std::string& resourceGroup)
   {
      _SetupSystemAndRenderer();
      CEGUI::ImagesetManager::getSingleton().createFromImageFile(imagesetName, fileName, resourceGroup);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GUI::DestroyImageset(const std::string& imagesetName)
   {
      _SetupSystemAndRenderer();
      if (IsImagesetPresent(imagesetName))
      {
         CEGUI::ImagesetManager::getSingleton().destroy(imagesetName);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GUI::AutoScaleImageset(const std::string& imagesetName, bool autoScale)
   {
      _SetupSystemAndRenderer();
      if (IsImagesetPresent(imagesetName))
      {
         CEGUI::ImagesetManager::getSingleton().get(imagesetName).setAutoScalingEnabled(autoScale);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void GUI::DefineImage(const std::string& imagesetName, const std::string& image,
      osg::Vec2 position, osg::Vec2 size, osg::Vec2 offset)
   {
      _SetupSystemAndRenderer();
      if (IsImagesetPresent(imagesetName))
      {
         CEGUI::Point ceguiPosition(position.x(), position.y());
         CEGUI::Size ceguiSize(size.x(), size.y());
         CEGUI::Point ceguiOffset(offset.x(), offset.y());
         CEGUI::ImagesetManager::getSingleton().get(imagesetName).defineImage(image, ceguiPosition, ceguiSize, ceguiOffset);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::ref_ptr<osg::Texture2D> GUI::CreateRenderTargetTexture(Widget& widget,
                                                                        const osg::Vec2* dimensions,
                                                                        const std::string& newImagesetName,
                                                                        const std::string& propertyName,
                                                                        const std::string& newImageName)
   {

      if (!widget.isPropertyPresent(propertyName))
      {
         LOG_WARNING("Widget \"" << widget.getName().c_str() << "\" does not have the \""
            << propertyName << "\" property. Cannot create render target texture.");
         return NULL;
      }

      // Determine the size of the texture.
      osg::Vec2 dims;
      if (dimensions != NULL)
      {
         dims.set(dimensions->x(), dimensions->y());
      }
      else
      {
         CEGUI::Size ceguiDims = widget.getPixelSize();
         dims.set(ceguiDims.d_width, ceguiDims.d_height);
      }

      // If no image name was specified, use the image set name.
      const std::string& imageName = newImageName.empty() ? newImagesetName : newImageName;

      CEGUI::Size ceguiDims(dims.x(), dims.y());
      CEGUI::Texture& texture = CEGUI::System::getSingleton().getRenderer()->createTexture(ceguiDims);
      CEGUI::Imageset& imageset = CEGUI::ImagesetManager::getSingleton().create(newImagesetName, texture);
      imageset.defineImage(imageName, CEGUI::Point(0,texture.getSize().d_height),
         CEGUI::Size(texture.getSize().d_width, -texture.getSize().d_height), CEGUI::Point(0,0));  //note: flipped upside down

      // create/allocate/setup osg-texture-render target
      osg::Texture2D* rttTexture = new osg::Texture2D();
      rttTexture->setTextureSize(int(dims.x()), int(dims.y()));
      rttTexture->setInternalFormat(GL_RGBA);
      rttTexture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
      rttTexture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

      osg::GraphicsContext* gc = mCamera->getGraphicsContext();
      rttTexture->apply(*gc->getState());

      //tell the CEGUI texture to use our OSG Texture
      GLuint textureID = rttTexture->getTextureObject(gc->getState()->getContextID())->_id;
      static_cast<CEGUI::OpenGLTexture&>(texture).setOpenGLTexture(textureID, ceguiDims);

      CEGUI::String ceguiSetImage = CEGUI::PropertyHelper::imageToString(&imageset.getImage(imageName));
      widget.setProperty(propertyName, ceguiSetImage);

      return rttTexture;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::ref_ptr<osg::Camera> GUI::CreateCameraForRenderTargetTexture(osg::Texture2D& renderTargetTexture,
                                                                                 const osg::Vec2& viewDimensions)
   {
      // Create a Camera to render the specified target texture.
      osg::Camera* rttCam = new osg::Camera();
      rttCam->setName("RTTCamera");
      rttCam->setGraphicsContext(mCamera->getGraphicsContext()); // Use the root camera to gain access to the main window.

      rttCam->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
      rttCam->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      rttCam->setClearColor(osg::Vec4(0.0, 0.0, 0.0, 0.0));
      rttCam->setViewport(0, 0, int(viewDimensions.x()), int(viewDimensions.y()));
      rttCam->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
      rttCam->setRenderOrder(osg::Camera::PRE_RENDER);
      rttCam->attach(osg::Camera::COLOR_BUFFER, &renderTargetTexture);

      return rttCam;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool GUI::AddSearchSuffix(const std::string& resourceGroup,
                                          const std::string& searchSuffix)
   {
      return mResProvider.AddSearchSuffix(resourceGroup, searchSuffix);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool GUI::RemoveSearchSuffix(const std::string& resourceGroup,
                                             const std::string& searchSuffix)
   {
      return mResProvider.RemoveSearchSuffix(resourceGroup, searchSuffix);
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned GUI::RemoveSearchSuffixes(const std::string& resourceGroup)
   {
      return mResProvider.RemoveSearchSuffixes(resourceGroup);
   }

   ////////////////////////////////////////////////////////////////////////////////
   unsigned GUI::ClearSearchSuffixes()
   {
      return mResProvider.ClearSearchSuffixes();
   }

   
   ////////////////////////////////////////////////////////////////////////////////
   CEGUI::Key::Scan KeyboardKeyToKeyScan(int key)
   {
      switch(key)
      {
   //      case Producer::Key_Unknown:
   //      {
   //         return CEGUI::Key::Scan(0);//?
   //      }
         case osgGA::GUIEventAdapter::KEY_Escape:
         {
            return CEGUI::Key::Escape;
         }
         case osgGA::GUIEventAdapter::KEY_F1:
         {
            return CEGUI::Key::F1;
         }
         case osgGA::GUIEventAdapter::KEY_F2:
         {
            return CEGUI::Key::F2;
         }
         case osgGA::GUIEventAdapter::KEY_F3:
         {
            return CEGUI::Key::F3;
         }
         case osgGA::GUIEventAdapter::KEY_F4:
         {
            return CEGUI::Key::F4;
         }
         case osgGA::GUIEventAdapter::KEY_F5:
         {
            return CEGUI::Key::F5;
         }
         case osgGA::GUIEventAdapter::KEY_F6:
         {
            return CEGUI::Key::F6;
         }
         case osgGA::GUIEventAdapter::KEY_F7:
         {
            return CEGUI::Key::F7;
         }
         case osgGA::GUIEventAdapter::KEY_F8:
         {
            return CEGUI::Key::F8;
         }
         case osgGA::GUIEventAdapter::KEY_F9:
         {
            return CEGUI::Key::F9;
         }
         case osgGA::GUIEventAdapter::KEY_F10:
         {
            return CEGUI::Key::F10;
         }
         case osgGA::GUIEventAdapter::KEY_F11:
         {
            return CEGUI::Key::F11;
         }
         case osgGA::GUIEventAdapter::KEY_F12:
         {
            return CEGUI::Key::F12;
         }
         case '`':
         {
            return CEGUI::Key::Apostrophe;
         }
         case '1':
         {
            return CEGUI::Key::One;
         }
         case '2':
         {
            return CEGUI::Key::Two;
         }
         case '3':
         {
            return CEGUI::Key::Three;
         }
         case '4':
         {
            return CEGUI::Key::Four;
         }
         case '5':
         {
            return CEGUI::Key::Five;
         }
         case '6':
         {
            return CEGUI::Key::Six;
         }
         case '7':
         {
            return CEGUI::Key::Seven;
         }
         case '8':
         {
            return CEGUI::Key::Eight;
         }
         case '9':
         {
            return CEGUI::Key::Nine;
         }
         case '0':
         {
            return CEGUI::Key::Zero;
         }
         case '-':
         {
            return CEGUI::Key::Minus;
         }
         case '=':
         {
            return CEGUI::Key::Equals;
         }
         case osgGA::GUIEventAdapter::KEY_BackSpace:
         {
            return CEGUI::Key::Backspace;
         }
         case osgGA::GUIEventAdapter::KEY_Tab:
         {
            return CEGUI::Key::Tab;
         }
         case 'A':
         {
            return CEGUI::Key::A;
         }
         case 'B':
         {
            return CEGUI::Key::B;
         }
         case 'C':
         {
            return CEGUI::Key::C;
         }
         case 'D':
         {
            return CEGUI::Key::D;
         }
         case 'E':
         {
            return CEGUI::Key::E;
         }
         case 'F':
         {
            return CEGUI::Key::F;
         }
         case 'G':
         {
            return CEGUI::Key::G;
         }
         case 'H':
         {
            return CEGUI::Key::H;
         }
         case 'I':
         {
            return CEGUI::Key::I;
         }
         case 'J':
         {
            return CEGUI::Key::J;
         }
         case 'K':
         {
            return CEGUI::Key::K;
         }
         case 'L':
         {
            return CEGUI::Key::L;
         }
         case 'M':
         {
            return CEGUI::Key::M;
         }
         case 'N':
         {
            return CEGUI::Key::N;
         }
         case 'O':
         {
            return CEGUI::Key::O;
         }
         case 'P':
         {
            return CEGUI::Key::P;
         }
         case 'Q':
         {
            return CEGUI::Key::Q;
         }
         case 'R':
         {
            return CEGUI::Key::R;
         }
         case 'S':
         {
            return CEGUI::Key::S;
         }
         case 'T':
         {
            return CEGUI::Key::T;
         }
         case 'U':
         {
            return CEGUI::Key::U;
         }
         case 'V':
         {
            return CEGUI::Key::V;
         }
         case 'W':
         {
            return CEGUI::Key::W;
         }
         case 'X':
         {
            return CEGUI::Key::X;
         }
         case 'Y':
         {
            return CEGUI::Key::Y;
         }
         case 'Z':
         {
            return CEGUI::Key::Z;
         }
         case '(':
         {
            return CEGUI::Key::LeftBracket;
         }
         case ')':
         {
            return CEGUI::Key::RightBracket;
         }
         case '\\':
         {
            return CEGUI::Key::Backslash;
         }
         case osgGA::GUIEventAdapter::KEY_Shift_Lock:
         {
            return CEGUI::Key::Capital;
         }
         case ';':
         {
            return CEGUI::Key::Semicolon;
         }
         case '\'':
         {
            return CEGUI::Key::Apostrophe;
         }
         case osgGA::GUIEventAdapter::KEY_Return:
         {
            return CEGUI::Key::Return;
         }
         case osgGA::GUIEventAdapter::KEY_Shift_L:
         {
            return CEGUI::Key::LeftShift;
         }
         case ',':
         {
            return CEGUI::Key::Comma;
         }
         case '.':
         {
            return CEGUI::Key::Period;
         }
         case '/':
         {
            return CEGUI::Key::Slash;
         }
         case osgGA::GUIEventAdapter::KEY_Shift_R:
         {
            return CEGUI::Key::RightShift;
         }
         case osgGA::GUIEventAdapter::KEY_Control_L:
         {
            return CEGUI::Key::LeftControl;
         }
         case osgGA::GUIEventAdapter::KEY_Super_L:
         {
            return CEGUI::Key::Scan(0);//?
         }
         case ' ':
         {
            return CEGUI::Key::Space;
         }
         case osgGA::GUIEventAdapter::KEY_Alt_L:
         {
            return CEGUI::Key::LeftAlt;
         }
         case osgGA::GUIEventAdapter::KEY_Alt_R:
         {
            return CEGUI::Key::RightAlt;
         }
         case osgGA::GUIEventAdapter::KEY_Super_R:
         {
            return CEGUI::Key::Scan(0);//?
         }
         case osgGA::GUIEventAdapter::KEY_Menu:
         {
            return CEGUI::Key::Scan(0);//?
         }
         case osgGA::GUIEventAdapter::KEY_Control_R:
         {
            return CEGUI::Key::RightControl;
         }
         case osgGA::GUIEventAdapter::KEY_Print:
         {
            return CEGUI::Key::SysRq;
         }
         case osgGA::GUIEventAdapter::KEY_Scroll_Lock:
         {
            return CEGUI::Key::ScrollLock;
         }
         case osgGA::GUIEventAdapter::KEY_Pause:
         {
            return CEGUI::Key::Pause;
         }
         case osgGA::GUIEventAdapter::KEY_Home:
         {
            return CEGUI::Key::Home;
         }
         case osgGA::GUIEventAdapter::KEY_Page_Up:
         {
            return CEGUI::Key::PageUp;
         }
         case osgGA::GUIEventAdapter::KEY_End:
         {
            return CEGUI::Key::End;
         }
         case osgGA::GUIEventAdapter::KEY_Page_Down:
         {
            return CEGUI::Key::PageDown;
         }
         case osgGA::GUIEventAdapter::KEY_Delete:
         {
            return CEGUI::Key::Delete;
         }
         case osgGA::GUIEventAdapter::KEY_Insert:
         {
            return CEGUI::Key::Insert;
         }
         case osgGA::GUIEventAdapter::KEY_Left:
         {
            return CEGUI::Key::ArrowLeft;
         }
         case osgGA::GUIEventAdapter::KEY_Up:
         {
            return CEGUI::Key::ArrowUp;
         }
         case osgGA::GUIEventAdapter::KEY_Right:
         {
            return CEGUI::Key::ArrowRight;
         }
         case osgGA::GUIEventAdapter::KEY_Down:
         {
            return CEGUI::Key::ArrowDown;
         }
         case osgGA::GUIEventAdapter::KEY_Num_Lock:
         {
            return CEGUI::Key::NumLock;
         }
         case osgGA::GUIEventAdapter::KEY_KP_Divide:
         {
            return CEGUI::Key::Divide;
         }
         case osgGA::GUIEventAdapter::KEY_KP_Multiply:
         {
            return CEGUI::Key::Multiply;
         }
         case osgGA::GUIEventAdapter::KEY_KP_Subtract:
         {
            return CEGUI::Key::Subtract;
         }
         case osgGA::GUIEventAdapter::KEY_KP_Add:
         {
            return CEGUI::Key::Add;
         }
         case osgGA::GUIEventAdapter::KEY_KP_Home:
         {
            return CEGUI::Key::Numpad7;
         }
         case osgGA::GUIEventAdapter::KEY_KP_Up:
         {
            return CEGUI::Key::Numpad8;
         }
         case osgGA::GUIEventAdapter::KEY_KP_Page_Up:
         {
            return CEGUI::Key::Numpad9;
         }
         case osgGA::GUIEventAdapter::KEY_KP_Left:
         {
            return CEGUI::Key::Numpad4;
         }
         case osgGA::GUIEventAdapter::KEY_KP_Begin:
         {
            return CEGUI::Key::Scan(0); //?
         }
         case osgGA::GUIEventAdapter::KEY_KP_Right:
         {
            return CEGUI::Key::Numpad6;
         }
         case osgGA::GUIEventAdapter::KEY_KP_End:
         {
            return CEGUI::Key::Numpad1;
         }
         case osgGA::GUIEventAdapter::KEY_KP_Down:
         {
            return CEGUI::Key::Numpad2;
         }
         case osgGA::GUIEventAdapter::KEY_KP_Page_Down:
         {
            return CEGUI::Key::Numpad3;
         }
         case osgGA::GUIEventAdapter::KEY_KP_Insert:
         {
            return CEGUI::Key::Numpad0;
         }
         case osgGA::GUIEventAdapter::KEY_KP_Delete:
         {
            return CEGUI::Key::Decimal;
         }
         case osgGA::GUIEventAdapter::KEY_KP_Enter:
         {
            return CEGUI::Key::NumpadEnter;
         }
         default:
         {
            return CEGUI::Key::Scan(0);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool GUI::handle(const osgGA::GUIEventAdapter& ea, 
                          osgGA::GUIActionAdapter& aa, osg::Object*,
                          osg::NodeVisitor *)
   {
      if(ea.getHandled()) 
      {
         return false;
      }
      switch (ea.getEventType() )
      {
         case osgGA::GUIEventAdapter::PUSH:
         {
            CEGUI::MouseButton index = CEGUI::LeftButton;
            switch(ea.getButton())
            {
               case osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON: index = CEGUI::MiddleButton; break;
               case osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON: index = CEGUI::RightButton; break;
            }
            return CEGUI::System::getSingleton().injectMouseButtonDown(index);
         }
         case osgGA::GUIEventAdapter::RELEASE:
         {
            CEGUI::MouseButton index = CEGUI::LeftButton;
            switch(ea.getButton())
            {
               case osgGA::GUIEventAdapter::MIDDLE_MOUSE_BUTTON: index = CEGUI::MiddleButton; break;
               case osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON: index = CEGUI::RightButton; break;
            }
            return CEGUI::System::getSingleton().injectMouseButtonUp(index);
         }
         case osgGA::GUIEventAdapter::DRAG:
         case osgGA::GUIEventAdapter::MOVE:
         {
            return CEGUI::System::getSingleton().injectMousePosition(ea.getX(), ea.getYmax() - ea.getY());
         }
         case osgGA::GUIEventAdapter::KEYDOWN:
         {
            int key = ea.getUnmodifiedKey();
            if( CEGUI::Key::Scan scanKey = KeyboardKeyToKeyScan(key) )
            {
               CEGUI::System::getSingleton().injectKeyDown(scanKey);
            }

            return CEGUI::System::getSingleton().injectChar( static_cast<CEGUI::utf32>(key) );   
         }
         case osgGA::GUIEventAdapter::KEYUP:
         {
            int key = ea.getUnmodifiedKey();
            bool handled(false);
            if( CEGUI::Key::Scan scanKey = KeyboardKeyToKeyScan(key) )
            {
               return CEGUI::System::getSingleton().injectKeyUp(scanKey);
            }

            return handled;
         }
         case osgGA::GUIEventAdapter::SCROLL:
         {
            float delta = ea.getScrollingMotion() == osgGA::GUIEventAdapter::SCROLL_UP ? 1 : -1;
            return CEGUI::System::getSingleton().injectMouseWheelChange(delta);
         }
         case osgGA::GUIEventAdapter::FRAME:
         {
            double current = ea.getTime();
            double delta = current - mLastFrameTime;
            mLastFrameTime = current;
            CEGUI::System::getSingletonPtr()->injectTimePulse(delta);
         }
         default: break;
      }
      return false;
   }
};
