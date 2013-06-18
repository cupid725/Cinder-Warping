/*
 Copyright (c) 2010-2013, Paul Houx - All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 This file is part of Cinder-Warping.

 Cinder-Warping is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Cinder-Warping is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Cinder-Warping.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "cinder/ImageIo.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

#include "WarpBilinear.h"
#include "WarpPerspective.h"
#include "WarpPerspectiveBilinear.h"

using namespace ci;
using namespace ci::app;
using namespace ph::warping;
using namespace std;

class _TBOX_PREFIX_App : public AppBasic {
public:
	void prepareSettings( Settings *settings );
	
	void setup();
	void shutdown();
	void update();
	void draw();
	
	void resize();
	
	void mouseMove( MouseEvent event );	
	void mouseDown( MouseEvent event );	
	void mouseDrag( MouseEvent event );	
	void mouseUp( MouseEvent event );	
	
	void keyDown( KeyEvent event );
	void keyUp( KeyEvent event );

	void updateWindowTitle();
private:
	gl::Texture	mImage;
	WarpList	mWarps;

	bool		mUseBeginEnd;
};

void _TBOX_PREFIX_App::prepareSettings(Settings *settings)
{
	settings->setTitle("Warping Sample");
}

void _TBOX_PREFIX_App::setup()
{
	mUseBeginEnd = false;
	updateWindowTitle();

	// 
	fs::path settings = getAssetPath("") / "warps.xml";
	if( fs::exists( settings ) )
	{
		// load warp settings from file
		mWarps = Warp::readSettings( loadFile( settings ) );
	}
	else
	{
		// create a warp from scratch
		mWarps.push_back( WarpPerspectiveBilinearRef( new WarpPerspectiveBilinear() ) );
	}

	// load test image
	try
	{ 
		mImage = gl::Texture( loadImage( loadAsset("help.png") ) ); 

		// adjust the content size of the warps
		Warp::setSize( mWarps, mImage.getSize() );
	}
	catch( const std::exception &e )
	{
		console() << e.what() << std::endl;
	}
}

void _TBOX_PREFIX_App::shutdown()
{
	// save warp settings
	fs::path settings = getAssetPath("") / "warps.xml";
	Warp::writeSettings( mWarps, writeFile( settings ) );
}

void _TBOX_PREFIX_App::update()
{
}

void _TBOX_PREFIX_App::draw()
{
	// clear the window
	gl::clear();

	if( mImage ) 
	{
		// iterate over the warps and draw their content
		gl::color( Color::white() );
		for(WarpConstIter itr=mWarps.begin();itr!=mWarps.end();++itr)
		{
			WarpRef warp( *itr );

			// there are two ways you can use the warps:
			if( ! mUseBeginEnd )
			{
				//  a) simply draw a texture on them (ideal for video)
				warp->draw( mImage );
			}
			else
			{
				//  b) issue your draw commands between begin() and end() statements
				warp->begin();
				gl::draw( mImage );
				warp->end();
			}
		}
	}
}

void _TBOX_PREFIX_App::resize()
{
	// resize the warps
	Warp::handleResize( mWarps );
}

void _TBOX_PREFIX_App::mouseMove( MouseEvent event )
{
	if( ! Warp::handleMouseMove( mWarps, event ) )
	{
		// let your application perform its mouseMove handling here
	}
}

void _TBOX_PREFIX_App::mouseDown( MouseEvent event )
{
	if( ! Warp::handleMouseDown( mWarps, event ) )
	{
		// let your application perform its mouseDown handling here
	}
}

void _TBOX_PREFIX_App::mouseDrag( MouseEvent event )
{
	if( ! Warp::handleMouseDrag( mWarps, event ) )
	{
		// let your application perform its mouseDrag handling here
	}
}

void _TBOX_PREFIX_App::mouseUp( MouseEvent event )
{
	if( ! Warp::handleMouseUp( mWarps, event ) )
	{
		// let your application perform its mouseUp handling here
	}
}

void _TBOX_PREFIX_App::keyDown( KeyEvent event )
{
	if( ! Warp::handleKeyDown( mWarps, event ) )
	{
		switch( event.getCode() )
		{
		case KeyEvent::KEY_ESCAPE:
			quit();
			break;
		case KeyEvent::KEY_f:
			setFullScreen( ! isFullScreen() );
			break;
		case KeyEvent::KEY_w:
			// toggle warp edit mode
			Warp::enableEditMode( ! Warp::isEditModeEnabled() );
			break;
		case KeyEvent::KEY_SPACE:
			mUseBeginEnd = !mUseBeginEnd;
			updateWindowTitle();
			break;
		}
	}
}

void _TBOX_PREFIX_App::keyUp( KeyEvent event )
{
	if( ! Warp::handleKeyUp( mWarps, event ) )
	{
		// let your application perform its keyUp handling here
	}
}

void _TBOX_PREFIX_App::updateWindowTitle()
{
#if defined( CINDER_MSW )
	std::wstringstream str;
	str << "Warping Sample - ";

	if(mUseBeginEnd)
		str << "Using begin() and end()";
	else
		str << "Using draw()";

	HWND hWnd = getRenderer()->getHwnd();
	::SetWindowText( hWnd, str.str().c_str() );
#endif
}

CINDER_APP_BASIC( _TBOX_PREFIX_App, RendererGl )
