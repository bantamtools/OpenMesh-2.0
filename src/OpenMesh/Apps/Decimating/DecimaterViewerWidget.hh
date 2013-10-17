/*===========================================================================*\
 *                                                                           *
 *                               OpenMesh                                    *
 *      Copyright (C) 2001-2010 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openmesh.org                                *
 *                                                                           *
 *---------------------------------------------------------------------------* 
 *  This file is part of OpenMesh.                                           *
 *                                                                           *
 *  OpenMesh is free software: you can redistribute it and/or modify         * 
 *  it under the terms of the GNU Lesser General Public License as           *
 *  published by the Free Software Foundation, either version 3 of           *
 *  the License, or (at your option) any later version with the              *
 *  following exceptions:                                                    *
 *                                                                           *
 *  If other files instantiate templates or use macros                       *
 *  or inline functions from this file, or you compile this file and         *
 *  link it with other files to produce an executable, this file does        *
 *  not by itself cause the resulting executable to be covered by the        *
 *  GNU Lesser General Public License. This exception does not however       *
 *  invalidate any other reasons why the executable file might be            *
 *  covered by the GNU Lesser General Public License.                        *
 *                                                                           *
 *  OpenMesh is distributed in the hope that it will be useful,              *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU Lesser General Public License for more details.                      *
 *                                                                           *
 *  You should have received a copy of the GNU LesserGeneral Public          *
 *  License along with OpenMesh.  If not,                                    *
 *  see <http://www.gnu.org/licenses/>.                                      *
 *                                                                           *
\*===========================================================================*/ 

/*===========================================================================*\
 *                                                                           *             
 *   $Revision: 325 $                                                         *
 *   $Date: 2010-06-17 12:45:58 +0200 (Do, 17 Jun 2010) $                   *
 *                                                                           *
\*===========================================================================*/


#ifndef OPENMESHAPPS_DECIMATERVIEWERWIDGET_HH
#define OPENMESHAPPS_DECIMATERVIEWERWIDGET_HH


//== INCLUDES =================================================================

#if !defined(OM_USE_OSG)
#  define OM_USE_OSG 0
#endif


#include <qtimer.h>
#include <string>
#include <memory>

//--------------------
#include <OpenMesh/Core/IO/MeshIO.hh>
//--------------------
#if OM_USE_OSG
#  include <OpenMesh/Tools/Kernel_OSG/TriMesh_OSGArrayKernelT.hh>
#  define DEFAULT_TRAITS Kernel_OSG::Traits
#  define TRIMESH_KERNEL Kernel_OSG::TriMesh_OSGArrayKernelT
#else
//--------------------
#  include <OpenMesh/Core/Mesh/Traits.hh>
#  include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#  define DEFAULT_TRAITS DefaultTraits
#  define TRIMESH_KERNEL TriMesh_ArrayKernelT
#endif

#include <OpenMesh/Apps/QtViewer/MeshViewerWidgetT.hh>

#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModNormalFlippingT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>


//== CLASS DEFINITION =========================================================


using namespace OpenMesh;

struct MyTraits : public DEFAULT_TRAITS
{
  VertexAttributes  ( Attributes::Normal       );
  FaceAttributes    ( Attributes::Normal       );  
};

typedef TRIMESH_KERNEL<MyTraits>                     mesh_t;
typedef MeshViewerWidgetT<mesh_t>                    MeshViewerWidget;

//== CLASS DEFINITION =========================================================


class DecimaterViewerWidget : public MeshViewerWidget
{
  Q_OBJECT
   
public:
  
  typedef MeshViewerWidget inherited_t;

  typedef Decimater::DecimaterT<mesh_t>                decimater_t;  
  typedef Decimater::ModQuadricT< decimater_t >        mod_quadric_t;
  typedef Decimater::ModNormalFlippingT< decimater_t > mod_nf_t;

  // object types
  typedef std::auto_ptr< decimater_t   >   decimater_o;
  typedef std::auto_ptr< mod_quadric_t >   mod_quadric_o;
  typedef std::auto_ptr< mod_nf_t      >   mod_nf_o;

  /// default constructor
  DecimaterViewerWidget(QWidget* _parent=0)
    : MeshViewerWidget(_parent), steps_(1)
  {
    timer_ = new QTimer(this);
    
    connect( timer_, SIGNAL(timeout()), SLOT(animate()) );
  }
  
  /// destructor
  ~DecimaterViewerWidget()
  {
    delete timer_;
  }

public: // inherited

  bool open_mesh(const char* _filename, OpenMesh::IO::Options _opt)
  {
    bool rc;

    if ( (rc = inherited_t::open_mesh( _filename, _opt )) )
    {
      std::cout << "prepare decimater" << std::endl;

      // to be compatible with gcc 2.95.3
      {
	decimater_o  tmp( new decimater_t  ( mesh() ) );
	decimater_ = tmp;
      }
      {
	mod_quadric_o  tmp( new mod_quadric_t( *decimater_ ) );
	mod_quadric_ = tmp;
      }
      {
	mod_nf_o       tmp( new mod_nf_t     ( *decimater_ ) );
	mod_nf_      = tmp;
      }

      decimater_->initialize();
    }
    return rc;
  }
  
protected slots:

  void animate( void );

protected:   

  virtual void keyPressEvent(QKeyEvent* _event);  


private:
  
  bool              animate_;
  QTimer           *timer_;

  decimater_o       decimater_;  
  mod_quadric_o     mod_quadric_;
  mod_nf_o          mod_nf_;

  size_t            steps_;
};


//=============================================================================
#endif // OPENMESHAPPS_DECIMATERVIEWERWIDGET_HH defined
//=============================================================================

