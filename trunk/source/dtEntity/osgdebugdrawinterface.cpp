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


#include <dtEntity/osgdebugdrawinterface.h>

#include <dtEntity/commandmessages.h>
#include <dtEntity/systemmessages.h>
#include <dtEntity/nodemasks.h>
#include <osg/BlendFunc>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/LineWidth>
#include <osg/Point>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/ShapeDrawable>
#include <osgText/Text>
#include <assert.h>
#include <iostream>
#include <stdlib.h>

#define ALL_BITS 0xFFFFFFFF

namespace dtEntity
{

   class TimedGeode : public osg::Geode
   {
   public:
      float mTimeOfDeath;
   };

 

   ////////////////////////////////////////////////////////////////////////////////
   OSGDebugDrawInterface::OSGDebugDrawInterface(dtEntity::EntityManager& em, StringId layerName)
   : mEnabled(false)
   , mGroupDepthTest(new osg::Group)
   , mGroupNoDepthTest(new osg::Group)
   , mCurrentTime(0)
   , mEntityManager(&em)
   , mEnableFunctor(this, &OSGDebugDrawInterface::OnEnable)
   , mTickFunctor(this, &OSGDebugDrawInterface::Tick)
   , mLayerName(layerName)
   {
      mGroupDepthTest->setNodeMask(NodeMasks::VISIBLE);
      mGroupNoDepthTest->setNodeMask(NodeMasks::VISIBLE);

	   {
			osg::ref_ptr<osg::StateSet> ss = mGroupDepthTest->getOrCreateStateSet();
		   ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);    
         ss->setRenderBinDetails(240000, "RenderBin");
			osg::ref_ptr<osg::LineWidth> lw = new osg::LineWidth();
		   lw->setWidth(1); 
		   ss->setAttributeAndModes(lw, osg::StateAttribute::ON); 
		   ss->setAttribute(new osg::PolygonOffset(1.0f,1.0f),osg::StateAttribute::ON);
			osg::ref_ptr<osg::PolygonMode> polyModeObj = new osg::PolygonMode;
		   polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
			ss->setAttribute(polyModeObj);
			osg::ref_ptr<osg::PolygonOffset> polyoffset = new osg::PolygonOffset;
			polyoffset->setFactor(-1.1f);
			polyoffset->setUnits(-1.1f);
			ss->setAttributeAndModes(polyoffset,osg::StateAttribute::OVERRIDE |
				osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
	   }

	   {
			osg::ref_ptr<osg::StateSet> ss = mGroupNoDepthTest->getOrCreateStateSet();
		   ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);    
         ss->setRenderBinDetails(240001, "RenderBin");
			osg::ref_ptr<osg::LineWidth> lw = new osg::LineWidth();
		   lw->setWidth(1); 
		   ss->setAttributeAndModes(lw, osg::StateAttribute::ON); 
         ss->setAttribute(new osg::PolygonOffset(1.0f,1.0f),osg::StateAttribute::ON);
         osg::ref_ptr<osg::PolygonMode> polyModeObj = new osg::PolygonMode;
	      polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
		   ss->setAttribute(polyModeObj);  
			ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

	   }
      LayerAttachPointSystem* ls;
      em.GetEntitySystem(LayerAttachPointComponent::TYPE, ls);
      LayerAttachPointComponent* layer;
      bool found = ls->GetByName(layerName, layer);
      if(!found)
      {
         LOG_ERROR("Error initializing OSGDebugDrawInterface: Layer not found!");
      }
      else
      {
         layer->GetAttachmentGroup()->addChild(mGroupDepthTest);
		   layer->GetAttachmentGroup()->addChild(mGroupNoDepthTest);         
      }

      mEntityManager->RegisterForMessages(EnableDebugDrawingMessage::TYPE, mEnableFunctor);
   }

   ////////////////////////////////////////////////////////////////////////////////
   OSGDebugDrawInterface::~OSGDebugDrawInterface()
   {
      mEntityManager->UnregisterForMessages(EnableDebugDrawingMessage::TYPE, mEnableFunctor);
      SetEnabled(false);
      while(mGroupDepthTest->getNumParents() > 0)
      {
         mGroupDepthTest->getParent(0)->removeChild(mGroupDepthTest);
      }

      while(mGroupNoDepthTest->getNumParents() > 0)
      {
         mGroupNoDepthTest->getParent(0)->removeChild(mGroupNoDepthTest);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::Clear()
   {
      mGroupDepthTest->removeChildren(0, mGroupDepthTest->getNumChildren());
      mGroupNoDepthTest->removeChildren(0, mGroupNoDepthTest->getNumChildren());
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::SetEnabled(bool enabled)
   {
      if(mEnabled == enabled) return;

      if(enabled)
      {
         mEntityManager->RegisterForMessages(dtEntity::PostFrameMessage::TYPE, mTickFunctor, "OSGDebugDrawInterface::Update");
         mGroupDepthTest->setNodeMask(ALL_BITS);
         mGroupNoDepthTest->setNodeMask(ALL_BITS);
      }
      else
      {      
         Clear();
         mGroupDepthTest->setNodeMask(0);
         mGroupNoDepthTest->setNodeMask(0);
         mEntityManager->UnregisterForMessages(dtEntity::PostFrameMessage::TYPE, mTickFunctor);
      }
      mEnabled = enabled;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool OSGDebugDrawInterface::IsEnabled() const
   {
      return mEnabled;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::OnEnable(const Message& m)
   {
      SetEnabled(m.GetBool(EnableDebugDrawingMessage::EnableId));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::Tick(const Message& m)
   {
      const PostFrameMessage& msg = static_cast<const PostFrameMessage&>(m);
      float dt = msg.GetDeltaRealTime();

      mCurrentTime += dt;
      unsigned int i = 0;
      while(i < mGroupDepthTest->getNumChildren())
      {
         TimedGeode* geode = static_cast<TimedGeode*>(mGroupDepthTest->getChild(i));
         if(geode->mTimeOfDeath < mCurrentTime)
         {
            mGroupDepthTest->removeChild(i);
         }
         else
         {
            ++i;
         }
      }    
	  i = 0;
	  while(i < mGroupNoDepthTest->getNumChildren())
      {
         TimedGeode* geode = static_cast<TimedGeode*>(mGroupNoDepthTest->getChild(i));
         if(geode->mTimeOfDeath < mCurrentTime)
         {
            mGroupNoDepthTest->removeChild(i);
         }
         else
         {
            ++i;
         }
      }     
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::AddPoint(const Vec3f& position, const Vec4f& color, int size,
      float duration, bool depthTestEnabled)
   {
      if(!mEnabled)
         return;
      osg::Geometry* geometry = new osg::Geometry();
      if(duration == 0)
      {
         geometry->setUseDisplayList(false);
      }

      osg::Vec3f v[] = { position };
      geometry->setVertexArray(new osg::Vec3Array(1, v));

      osg::Vec4 c[] = { color };
      geometry->setColorArray(new osg::Vec4Array(1, c));
      geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
      
      osg::BoundingBox bb(-10, -10, -10, 10, 10, 10);
      geometry->setInitialBound(bb);

      if(size != 1)
      {
         osg::StateSet* ss = geometry->getOrCreateStateSet();
         osg::Point* p = new osg::Point();
         p->setSize(size);         
         ss->setAttributeAndModes(p, osg::StateAttribute::ON); 
      }
      geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, 1));   
      AddDrawable(geometry, duration, depthTestEnabled);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::AddPoints(const std::vector<osg::Vec3f>& positions,
      const Vec4f& color, int size, float duration, bool depthTestEnabled)
   {
      if(!mEnabled)
         return;
      osg::Geometry* geometry = new osg::Geometry();
      if(duration == 0)
      {
         geometry->setUseDisplayList(false);
      }
      osg::Vec3f* v = const_cast<osg::Vec3f*>(&positions[0]);
      geometry->setVertexArray(new osg::Vec3Array(positions.size(), v));

      osg::Vec4 c[] = { color };
      geometry->setColorArray(new osg::Vec4Array(1, c));
      geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
      
      if(size != 1)
      {
         osg::StateSet* ss = geometry->getOrCreateStateSet();
         osg::Point* p = new osg::Point();
         p->setSize(size);         
         ss->setAttributeAndModes(p, osg::StateAttribute::ON); 
      }
      geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, positions.size()));   
      AddDrawable(geometry, duration, depthTestEnabled);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::AddLine(const Vec3f& start, const Vec3f& end,
      const Vec4f& color, int linewidth, float duration, bool depthTestEnabled)
   {
      if(!mEnabled)
         return;
       osg::Geometry* geometry = new osg::Geometry();
       if(duration == 0)
       {
          geometry->setUseDisplayList(false);
       }

      osg::Vec3 v[] = { start, end };
      geometry->setVertexArray(new osg::Vec3Array(2, v));

      osg::Vec4 c[] = { color };
      geometry->setColorArray(new osg::Vec4Array(1, c));
      geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
      
      if(linewidth != 1)
      {
         osg::StateSet* ss = geometry->getOrCreateStateSet();
         osg::LineWidth* lw = new osg::LineWidth(); 
         lw->setWidth(linewidth); 
         ss->setAttributeAndModes(lw, osg::StateAttribute::ON); 
      }
      geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));   
      AddDrawable(geometry, duration, depthTestEnabled);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::AddLines(const std::vector<Vec3f>& lines,
      const Vec4f& color, int linewidth, float duration, bool depthTestEnabled)
   {
      if(!mEnabled || lines.empty())
         return;
      osg::Geometry* geometry = new osg::Geometry();
      if(duration == 0)
      {
         geometry->setUseDisplayList(false);
      }
      
      osg::Vec3f* v = const_cast<osg::Vec3f*>(&lines[0]);
      osg::Vec3Array* a = new osg::Vec3Array(lines.size(), v);
      
      geometry->setVertexArray(a);

      osg::Vec4 c[] = { color };
      geometry->setColorArray(new osg::Vec4Array(1, c));
      geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
      
      if(linewidth != 1)
      {
         osg::StateSet* ss = geometry->getOrCreateStateSet();
         osg::LineWidth* lw = new osg::LineWidth(); 
         lw->setWidth(linewidth); 
         ss->setAttributeAndModes(lw, osg::StateAttribute::ON); 
      }
      geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, lines.size()));   
      AddDrawable(geometry, duration, depthTestEnabled);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::AddCross(const Vec3f& position, const Vec4f& color,
      float size, float duration, bool depthTestEnabled)
   {
      if(!mEnabled)
         return;
       osg::Geometry* geometry = new osg::Geometry();
       if(duration == 0)
       {
          geometry->setUseDisplayList(false);
       }


      osg::Vec3 v[] = { 
         osg::Vec3(position - osg::Vec3( size, 0, 0)),
         osg::Vec3(position - osg::Vec3(-size, 0, 0)),
         osg::Vec3(position - osg::Vec3( 0,-size, 0)),
         osg::Vec3(position - osg::Vec3( 0, size, 0)),
         osg::Vec3(position - osg::Vec3( 0, 0,-size)),
         osg::Vec3(position - osg::Vec3( 0, 0, size)),
      };
      geometry->setVertexArray(new osg::Vec3Array(6, v));

      osg::Vec4 c[] = { color };
      geometry->setColorArray(new osg::Vec4Array(1, c));
      geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

      geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));   
      geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 2, 2));
      geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 4, 2));
      AddDrawable(geometry, duration, depthTestEnabled);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::AddSphere(const Vec3f& position, float radius,
      const Vec4f& color, float duration, bool depthTestEnabled)
   {
      if(!mEnabled)
         return;
      
      osg::Sphere* sphere = new osg::Sphere(position, radius);

      osg::ShapeDrawable* drawable = new osg::ShapeDrawable(sphere);
      drawable->setColor(color);
      if(duration == 0)
      {
         drawable->setUseDisplayList(false);
      }
      osg::TessellationHints* hints = new osg::TessellationHints;
      hints->setDetailRatio(0.3f);
      drawable->setTessellationHints(hints);

      AddDrawable(drawable, duration, depthTestEnabled);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::AddMesh(osg::Geometry* mesh, const Vec3f& position, 
      const Vec4f& lineColor, int lineWidth, const Vec4f& faceColor, float duration, bool depthTestEnabled)
   {
      if(!mEnabled)
         return;

      // draw wire frame
      if(lineColor[3] > 0 && lineWidth != 0)
      {
         TimedGeode* geode = new TimedGeode;
         geode->mTimeOfDeath = mCurrentTime + duration;

         unsigned int copyops = osg::CopyOp::DEEP_COPY_ALL;
         osg::Geometry* newgeometry = dynamic_cast<osg::Geometry*>(mesh->clone(copyops));
         newgeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
         osg::Vec4Array* v4 = new osg::Vec4Array();
         v4->push_back(lineColor);
         newgeometry->setColorArray(v4);

         osg::StateSet* ss = newgeometry->getStateSet();
         if(ss != NULL)
         {
            for(unsigned int i = 0; i < 2; ++i)
            {
               ss->removeTextureAttribute(i, osg::StateAttribute::TEXTURE);
               newgeometry->setTexCoordArray(i, NULL);
            }

            ss->releaseGLObjects();
            newgeometry->setStateSet(NULL);
         }

         if(position.length2() != 0)
         {
            osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(newgeometry->getVertexArray());
            if(verts)
            {
               for(osg::Vec3Array::size_type i = 0; i < verts->size(); ++i)
               {
                  (*verts)[i] += position;
               }
            }
         }
         geode->addDrawable(newgeometry);


         if(depthTestEnabled)
         {
           mGroupDepthTest->addChild(geode);
         }
        else
        {
           mGroupNoDepthTest->addChild(geode);
        }
      }

      // draw faces
      if(faceColor[3] > 0)
      {
         TimedGeode* geode = new TimedGeode;
         geode->mTimeOfDeath = mCurrentTime + duration;

			osg::StateSet* ss = geode->getOrCreateStateSet();
			osg::PolygonMode* polyModeObj = new osg::PolygonMode;
			polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
			ss->setAttribute(polyModeObj);

			ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
			osg::PolygonOffset* polyoffset = new osg::PolygonOffset;
			polyoffset->setFactor(-1.0f);
			polyoffset->setUnits(-1.0f);
			ss->setAttributeAndModes(polyoffset,osg::StateAttribute::OVERRIDE |
				osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);

			if(faceColor[3] < 1)
			{
				ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
				ss->setMode(GL_BLEND,osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED | osg::StateAttribute::ON);
				// use premultiplied alpha
				osg::BlendFunc* blend = new osg::BlendFunc;
				blend->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
				ss->setAttributeAndModes(blend, osg::StateAttribute::ON);
			}

         unsigned int copyops = osg::CopyOp::DEEP_COPY_ALL;
         osg::Geometry* newgeometry = dynamic_cast<osg::Geometry*>(mesh->clone(copyops));
         newgeometry->setColorBinding(osg::Geometry::BIND_OVERALL);
         osg::Vec4Array* v4 = new osg::Vec4Array();
         v4->push_back(faceColor);
         newgeometry->setColorArray(v4);

         osg::StateSet* nss = newgeometry->getStateSet();
         if(nss != NULL)
         {
            for(unsigned int i = 0; i < 2; ++i)
            {
               nss->removeTextureAttribute(i, osg::StateAttribute::TEXTURE);
               newgeometry->setTexCoordArray(i, NULL);
            }

            nss->releaseGLObjects();
            newgeometry->setStateSet(NULL);
         }

         if(position.length2() != 0)
         {
            osg::Vec3Array* verts = dynamic_cast<osg::Vec3Array*>(newgeometry->getVertexArray());
            if(verts)
            {
               for(osg::Vec3Array::size_type i = 0; i < verts->size(); ++i)
               {
                  (*verts)[i] += position;
               }
            }
         }
         geode->addDrawable(newgeometry);


         if(depthTestEnabled)
         {
           mGroupDepthTest->addChild(geode);
         }
        else
        {
           mGroupNoDepthTest->addChild(geode);
        }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::AddDrawable(osg::Drawable* drawable, float duration, bool depthTestEnabled)
   {
      if(!mEnabled)
         return;
      TimedGeode* geode = new TimedGeode;
      geode->mTimeOfDeath = mCurrentTime + duration;
         
      geode->addDrawable(drawable);
      
      
      if(depthTestEnabled)
      {
         mGroupDepthTest->addChild(geode);
      }
	  else
	  {
		  mGroupNoDepthTest->addChild(geode);
	  }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::AddTriangle(const Vec3f& vert0, const Vec3f& vert1, 
      const Vec3f& vert2, const Vec4f& color, int linewidth, float duration, bool depthTestEnabled)
   {
      if(!mEnabled)
         return;
      osg::Geometry* geometry = new osg::Geometry();
      if(duration == 0)
      {
         geometry->setUseDisplayList(false);
      }

      osg::Vec3 v[] = { vert0, vert1, vert2 };
      geometry->setVertexArray(new osg::Vec3Array(3, v));

      osg::Vec4 c[] = { color };
      geometry->setColorArray(new osg::Vec4Array(1, c));
      geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

      if(linewidth != 1)
      {
         osg::StateSet* ss = geometry->getOrCreateStateSet();
         osg::LineWidth* lw = new osg::LineWidth(); 
         lw->setWidth(linewidth); 
         ss->setAttributeAndModes(lw, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE); 
      }

      geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, 3));   
      AddDrawable(geometry, duration, depthTestEnabled);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::AddTriangles(const std::vector<Vec3f>& tris, const Vec4f& color,
      int lineWidth, float duration, bool depthTestEnabled)
   {
      if(!mEnabled)
         return;
      osg::Geometry* geometry = new osg::Geometry();
      if(duration == 0)
      {
         geometry->setUseDisplayList(false);
      }

      osg::Vec3f* v = const_cast<osg::Vec3f*>(&tris[0]);
      geometry->setVertexArray(new osg::Vec3Array(tris.size(), v));

      osg::Vec4 c[] = { color };
      geometry->setColorArray(new osg::Vec4Array(1, c));
      geometry->setColorBinding(osg::Geometry::BIND_OVERALL);

      if(lineWidth != 1)
      {
         osg::StateSet* ss = geometry->getOrCreateStateSet();
         osg::LineWidth* lw = new osg::LineWidth(); 
         lw->setWidth(lineWidth); 
         ss->setAttributeAndModes(lw, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE); 
      }

      geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES, 0, tris.size()));   
      AddDrawable(geometry, duration, depthTestEnabled);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::AddCircle(const Vec3f& position, const Vec3f& planeNormal, float radius, const Vec4f& color, float duration, bool depthTestEnabled)
   {
      if(!mEnabled)
         return;
       
       int smallestAxis = 0;
       if(abs(planeNormal[1]) < abs(planeNormal[smallestAxis])) smallestAxis = 1;
       if(abs(planeNormal[2]) < abs(planeNormal[smallestAxis])) smallestAxis = 2;
       osg::Vec3 base1(0,0,0);
       base1[smallestAxis] = 1;
       osg::Vec3 ortho1 = planeNormal ^ base1; ortho1.normalize();
       osg::Vec3 ortho2 = planeNormal ^ ortho1;ortho2.normalize();
       
       std::vector<osg::Vec3> lines;
       lines.push_back(osg::Vec3());
       unsigned int numPoints = 16;
       
       for(unsigned int i = 0; i < numPoints; ++i)
       {
          float angle = (static_cast<float>(i) / static_cast<float>(numPoints)) * osg::PI * 2;
          osg::Vec3 p = position + ortho1 * sin(angle) * -1 * radius + ortho2 * cos(angle) * radius;
          lines.push_back(p);
          if(numPoints != i + 1)
          {
            lines.push_back(p);
          }
       }
       lines[0] = lines.back();

       AddLines(lines, color, 1, duration, depthTestEnabled);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::AddString(const Vec3f& position, const std::string& str, const Vec4f& color, float duration, bool depthTestEnabled)
   {
      if(!mEnabled)
         return;
      
      osgText::Text* text = new osgText::Text();
      text->setBackdropType(osgText::Text::NONE);
      text->setCharacterSize(20);
      text->setCharacterSizeMode(osgText::TextBase::SCREEN_COORDS);
      text->setColor(color);
      text->setText(str);
      text->setAlignment(osgText::TextBase::CENTER_CENTER);
      text->setAxisAlignment(osgText::TextBase::SCREEN);
      text->setPosition(position);
      osg::StateSet* ss = text->getOrCreateStateSet();
      osg::PolygonMode* polyModeObj = new osg::PolygonMode;
      polyModeObj->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL);
      ss->setAttribute(polyModeObj);

      AddDrawable(text, duration, depthTestEnabled);
   }      

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::AddAABB(const Vec3f& minCoords, const Vec3f& maxCoords, const Vec4f& color, int lineWidth, float duration, bool depthTestEnabled)
   {
      if(!mEnabled)
         return;
      osg::Vec3 p000(minCoords);
      osg::Vec3 p111(maxCoords);
      osg::Vec3 p001(minCoords[0], minCoords[1], maxCoords[2]);
      osg::Vec3 p010(minCoords[0], maxCoords[1], minCoords[2]);
      osg::Vec3 p100(maxCoords[0], minCoords[1], minCoords[2]);
      osg::Vec3 p011(minCoords[0], maxCoords[1], maxCoords[2]);
      osg::Vec3 p101(maxCoords[0], minCoords[1], maxCoords[2]);
      osg::Vec3 p110(maxCoords[0], maxCoords[1], minCoords[2]);
      
      std::vector<osg::Vec3> lines(24);
      lines[0] = p000; lines[1] = p100;
      lines[2] = p100; lines[3] = p101;
      lines[4] = p101; lines[5] = p001;
      lines[6] = p001; lines[7] = p000;

      lines[8] = p001; lines[9] = p011;
      lines[10] = p101; lines[11] = p111;
      lines[12] = p100; lines[13] = p110;
      lines[14] = p000; lines[15] = p010;

      lines[16] = p011; lines[17] = p111;
      lines[18] = p010; lines[19] = p011;
      lines[20] = p111; lines[21] = p110;
      lines[22] = p010; lines[23] = p110;

      
      AddLines(lines, color, lineWidth, duration, depthTestEnabled);
   }      

   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::AddOBB(const Matrix& m, const Vec3f& minCoords, const Vec3f& maxCoords, const Vec4f& color, int lineWidth, float duration, bool depthTestEnabled)
   {
      if(!mEnabled)
         return;
      osg::Vec3 p000(minCoords);
      osg::Vec3 p111(maxCoords);
      osg::Vec3 p001(minCoords[0], minCoords[0], maxCoords[1]);
      osg::Vec3 p010(minCoords[0], maxCoords[0], minCoords[1]);
      osg::Vec3 p100(maxCoords[0], minCoords[0], minCoords[1]);
      osg::Vec3 p011(minCoords[0], maxCoords[0], maxCoords[1]);
      osg::Vec3 p101(maxCoords[0], minCoords[0], maxCoords[1]);
      osg::Vec3 p110(maxCoords[0], maxCoords[0], minCoords[1]);
      
      p000 = p000 * m;
      p111 = p111 * m;
      p001 = p001 * m;
      p010 = p010 * m;
      p100 = p100 * m;
      p011 = p011 * m;
      p101 = p101 * m;
      p110 = p110 * m;

      std::vector<osg::Vec3> lines(24);
      lines[0] = p000; lines[1] = p100;
      lines[2] = p100; lines[3] = p101;
      lines[4] = p101; lines[5] = p001;
      lines[6] = p001; lines[7] = p000;

      lines[8] = p001; lines[9] = p011;
      lines[10] = p101; lines[11] = p111;
      lines[12] = p100; lines[13] = p110;
      lines[14] = p000; lines[15] = p010;

      lines[16] = p011; lines[17] = p111;
      lines[18] = p010; lines[19] = p011;
      lines[20] = p111; lines[21] = p110;
      lines[22] = p010; lines[23] = p110;

      
      AddLines(lines, color, lineWidth, duration, depthTestEnabled);
   }
      
   ////////////////////////////////////////////////////////////////////////////////
   void OSGDebugDrawInterface::AddAxes(const Matrix& m, const Vec4f& color, float size, float duration, bool depthTestEnabled)
   {
      float arroww = 0.1f;
      osg::Vec3 p0(0, 0, 0);
      osg::Vec3 p1(0, 0, 1);
      osg::Vec3 p2(0, 1, 0);
      osg::Vec3 p3(1, 0, 0);

      osg::Vec3 p1a(arroww, 0, 1 - arroww);
      osg::Vec3 p1b(-arroww, 0, 1 - arroww);

      osg::Vec3 p2a(arroww, 1 - arroww, 0);
      osg::Vec3 p2b(-arroww, 1 - arroww, 0);

      osg::Vec3 p3a(1 - arroww, 0, arroww);
      osg::Vec3 p3b(1 - arroww, 0, -arroww);

      std::vector<osg::Vec3> lines(18);
      lines[0] = m * p0; lines[1] = m * p1;
      lines[2] = m * p0; lines[3] = m * p2;
      lines[4] = m * p0; lines[5] = m * p3;

      lines[6] = m * p1; lines[7] = m * p1a;
      lines[8] = m * p1; lines[9] = m * p1b;

      lines[10] = m * p2; lines[11] = m * p2a;
      lines[12] = m * p2; lines[13] = m * p2b;

      lines[14] = m * p3; lines[15] = m * p3a;
      lines[16] = m * p3; lines[17] = m * p3b;

      AddLines(lines, color, size, duration, depthTestEnabled);
   }
}
