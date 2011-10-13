/* osgLibRocket, an interface for OpenSceneGraph to use LibRocket
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/
//
// This code is copyright (c) 2011 Martin Scheffler martin.scheffler@googlemail.com
//

#include <osgLibRocket/RenderInterface>
#include <osg/BlendFunc>
#include <osg/MatrixTransform>
#include <assert.h>
#include <osg/Geode>
#include <osgDB/FileUtils>
#include <osgDB/ReadFile>

namespace osgLibRocket
{

	RenderInterface::RenderInterface()
		: _scissorsEnabled(false)
		, _nextTextureId(1)
		, _nextGeometryId(1)		
	{
	}

	void RenderInterface::setRenderTarget(osg::Group* grp, int w, int h)
	{
		_screenWidth = w;
		_screenHeight = h;
		if(_renderTarget != grp)
		{
			 _renderTarget = grp;
			 _renderTargetStateSet = grp->getOrCreateStateSet();
			 _renderTargetStateSet->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
			 _renderTargetStateSet->setMode(GL_BLEND,osg::StateAttribute::ON);
			 _renderTargetStateSet->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
		}
	}

	osg::Group* RenderInterface::getRenderTarget() const
	{
		return _renderTarget;
	}

	osg::Node* RenderInterface::createGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture, bool useDisplayList)
	{

		osg::Geometry* geometry = new osg::Geometry();
		geometry->setUseDisplayList(useDisplayList);
		geometry->setDataVariance(osg::Object::DYNAMIC);

		osg::Vec3Array* vertarray = new osg::Vec3Array(num_vertices);
		osg::Vec4Array* colorarray = new osg::Vec4Array(num_vertices);
		osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array(num_vertices);

		for(int i = 0; i < num_vertices; ++i)
		{
			Rocket::Core::Vertex* vert = &vertices[i];
			Rocket::Core::Colourb c = vert->colour;
			(*vertarray)[i].set(vert->position.x, vert->position.y, 0);
			(*colorarray)[i].set(c.red / 255.0f, c.green / 255.0f, c.blue / 255.0f, c.alpha / 255.0f);
			(*texcoords)[i].set(vert->tex_coord.x, vert->tex_coord.y);
		}

		osg::DrawElementsUInt* elements = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, num_indices);
		for(int i = 0; i < num_indices; ++i)
		{
			elements->setElement(i, indices[i]);
		}

		geometry->setVertexArray(vertarray);
		geometry->setColorArray(colorarray);
		geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

		geometry->addPrimitiveSet(elements);

		if(texture != 0)
		{
			geometry->setTexCoordArray(0, texcoords);
			TextureMap::iterator i = _textureMap.find(texture);
			assert(i != _textureMap.end());
			osg::StateSet* ss = geometry->getOrCreateStateSet();
			ss->setTextureAttributeAndModes(0, i->second, osg::StateAttribute::ON);
		}


		osg::Geode* geode = new osg::Geode();
		geode->setDataVariance(osg::Object::DYNAMIC);
		geode->addDrawable(geometry);
		return geode;
	}


	/// Called by Rocket when it wants to render geometry that the application does not wish to optimise. Note that
	/// Rocket renders everything as triangles.
	/// @param[in] vertices The geometry's vertex data.
	/// @param[in] num_vertices The number of vertices passed to the function.
	/// @param[in] indices The geometry's index data.
	/// @param[in] num_indices The number of indices passed to the function. This will always be a multiple of three.
	/// @param[in] texture The texture to be applied to the geometry. This may be NULL, in which case the geometry is untextured.
	/// @param[in] translation The translation to apply to the geometry.
	void RenderInterface::RenderGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture, const Rocket::Core::Vector2f& translation)
	{

		osg::Node* node = createGeometry(vertices, num_vertices, indices, num_indices, texture, false);

		osg::MatrixTransform* trans = new osg::MatrixTransform();
		trans->setMatrix(osg::Matrix::translate(osg::Vec3(translation.x, translation.y, 0)));
		trans->addChild(node);
		trans->setDataVariance(osg::Object::DYNAMIC);

      if(_scissorsEnabled)
      {
         node->getOrCreateStateSet()->setAttributeAndModes(_scissorTest, osg::StateAttribute::ON);
      }
		_renderTarget->addChild(trans);
		_renderTarget->dirtyBound();

		_instantGeometryMap.push_back(trans);
	}

	/// Called by Rocket when it wants to compile geometry it believes will be static for the forseeable future.
	/// If supported, this should be return a pointer to an optimised, application-specific version of the data. If
	/// not, do not override the function or return NULL; the simpler RenderGeometry() will be called instead.
	/// @param[in] vertices The geometry's vertex data.
	/// @param[in] num_vertices The number of vertices passed to the function.
	/// @param[in] indices The geometry's index data.
	/// @param[in] num_indices The number of indices passed to the function. This will always be a multiple of three.
	/// @param[in] texture The texture to be applied to the geometry. This may be NULL, in which case the geometry is untextured.
	/// @return The application-specific compiled geometry. Compiled geometry will be stored and rendered using RenderCompiledGeometry() in future calls, and released with ReleaseCompiledGeometry() when it is no longer needed.
	Rocket::Core::CompiledGeometryHandle RenderInterface::CompileGeometry(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rocket::Core::TextureHandle texture)
	{
		osg::Node* node = createGeometry(vertices, num_vertices, indices, num_indices, texture, true);

		_compiledGeometryMap[_nextGeometryId] = node;
		return _nextGeometryId++;
	}

	/// Called by Rocket when it wants to render application-compiled geometry.
	/// @param[in] geometry The application-specific compiled geometry to render.
	/// @param[in] translation The translation to apply to the geometry.
	void RenderInterface::RenderCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry, const Rocket::Core::Vector2f& translation)
	{
		CompiledGeometryMap::iterator i = _compiledGeometryMap.find(geometry);
		if(i != _compiledGeometryMap.end())
		{
			osg::MatrixTransform* trans = new osg::MatrixTransform();
			trans->setMatrix(osg::Matrix::translate(osg::Vec3(translation.x, translation.y, 0)));
			trans->addChild(i->second);

			if(_scissorsEnabled)
			{
				i->second->getOrCreateStateSet()->setAttributeAndModes(_scissorTest, osg::StateAttribute::ON);
			}
			else
			{
				osg::StateSet* ss = i->second->getStateSet();
				if(ss)
				{
					i->second->getOrCreateStateSet()->removeAttribute(_scissorTest);
				}
			}

			_renderTarget->addChild(trans);
			_renderTarget->dirtyBound();
		}
	}

	/// Called by Rocket when it wants to release application-compiled geometry.
	/// @param[in] geometry The application-specific compiled geometry to release.
	void RenderInterface::ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry)
	{
		CompiledGeometryMap::iterator i = _compiledGeometryMap.find(geometry);
		if(i != _compiledGeometryMap.end())
		{
			_compiledGeometryMap.erase(i);
		}
	}

	/// Called by Rocket when it wants to enable or disable scissoring to clip content.
	/// @param[in] enable True if scissoring is to enabled, false if it is to be disabled.
	void RenderInterface::EnableScissorRegion(bool enable)
	{
		_scissorsEnabled = enable;
	}

	/// Called by Rocket when it wants to change the scissor region.
	/// @param[in] x The left-most pixel to be rendered. All pixels to the left of this should be clipped.
	/// @param[in] y The top-most pixel to be rendered. All pixels to the top of this should be clipped.
	/// @param[in] width The width of the scissored region. All pixels to the right of (x + width) should be clipped.
	/// @param[in] height The height of the scissored region. All pixels to below (y + height) should be clipped.
	void RenderInterface::SetScissorRegion(int x, int y, int width, int height)
	{
		_scissorTest = new osg::Scissor(x, _screenHeight - y - height, width, height);
	}

	void RenderInterface::AddTexture(Rocket::Core::TextureHandle& texture_handle, osg::Image* image)
	{
		texture_handle = _nextTextureId++;
		osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
		texture->setImage(image);
		texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
		texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
      texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
		texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
		_textureMap[texture_handle] = texture;
	}

	/// Called by Rocket when a texture is required by the library.
	/// @param[out] texture_handle The handle to write the texture handle for the loaded texture to.
	/// @param[out] texture_dimensions The variable to write the dimensions of the loaded texture.
	/// @param[in] source The application-defined image source, joined with the path of the referencing document.
	/// @return True if the load attempt succeeded and the handle and dimensions are valid, false if not.
	bool RenderInterface::LoadTexture(Rocket::Core::TextureHandle& texture_handle, Rocket::Core::Vector2i& texture_dimensions, const Rocket::Core::String& source)
	{

		std::string src = source.CString();
		std::string path = osgDB::findDataFile(src);
		if(path.empty())
		{
			return false;
		}
		osg::ref_ptr<osg::Image> img = osgDB::readImageFile(path);
		if(!img.valid())
		{
			return false;
		}
		img->flipVertical();

		if(img == NULL) return false;

		texture_dimensions.x = img->s();
		texture_dimensions.y = img->t();

		AddTexture(texture_handle, img);

		return true;
	}

	/// Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
	/// @param[out] texture_handle The handle to write the texture handle for the generated texture to.
	/// @param[in] source The raw 8-bit texture data. Each pixel is made up of four 8-bit values, indicating red, green, blue and alpha in that order.
	/// @param[in] source_dimensions The dimensions, in pixels, of the source data.
	/// @return True if the texture generation succeeded and the handle is valid, false if not.
	bool RenderInterface::GenerateTexture(Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, const Rocket::Core::Vector2i& source_dimensions)
	{

		osg::ref_ptr<osg::Image> img = new osg::Image();
		int w = source_dimensions.x;
		int h = source_dimensions.y;
		img->allocateImage(w, h, 1, GL_RGBA, GL_UNSIGNED_BYTE);
		memcpy(img->data(), source, w * h * 4 * sizeof(Rocket::Core::byte));

		AddTexture(texture_handle, img);
		return true;
	}



	/// Called by Rocket when a loaded texture is no longer required.
	/// @param texture The texture handle to release.
	void RenderInterface::ReleaseTexture(Rocket::Core::TextureHandle texture)
	{
			TextureMap::iterator i = _textureMap.find(texture);
			if(i != _textureMap.end())
			{
				_textureMap.erase(i);
			}
	}

	/// Called when this render interface is released.
	void RenderInterface::Release()
	{

	}

}
