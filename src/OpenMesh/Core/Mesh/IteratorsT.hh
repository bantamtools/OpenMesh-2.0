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
 *   $Revision: 333 $                                                         *
 *   $Date: 2010-07-20 15:43:40 +0200 (Di, 20 Jul 2010) $                   *
 *                                                                           *
\*===========================================================================*/

#ifndef OPENMESH_ITERATORS_HH
#define OPENMESH_ITERATORS_HH

//=============================================================================
//
//  Iterators for PolyMesh/TriMesh
//
//=============================================================================



//== INCLUDES =================================================================

#include <OpenMesh/Core/System/config.h>
#include <OpenMesh/Core/Mesh/Status.hh>
#include <assert.h>
#include <cstddef>



//== NAMESPACES ===============================================================

namespace OpenMesh {
namespace Iterators {


//== FORWARD DECLARATIONS =====================================================


template <class Mesh> class VertexIterT;
template <class Mesh> class ConstVertexIterT;
template <class Mesh> class HalfedgeIterT;
template <class Mesh> class ConstHalfedgeIterT;
template <class Mesh> class EdgeIterT;
template <class Mesh> class ConstEdgeIterT;
template <class Mesh> class FaceIterT;
template <class Mesh> class ConstFaceIterT;




//== CLASS DEFINITION =========================================================


/** \class VertexIterT IteratorsT.hh <OpenMesh/Mesh/Iterators/IteratorsT.hh>
    Linear iterator.
*/

template <class Mesh>
class VertexIterT
{
public:


  //--- Typedefs ---

  typedef typename Mesh::Vertex           value_type;
  typedef typename Mesh::VertexHandle         value_handle;

#if 0
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef std::ptrdiff_t       difference_type;
  typedef const value_type&    reference;
  typedef const value_type*    pointer;
  typedef const Mesh*          mesh_ptr;
  typedef const Mesh&          mesh_ref;
#else
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef std::ptrdiff_t       difference_type;
  typedef value_type&          reference;
  typedef value_type*          pointer;
  typedef Mesh*                mesh_ptr;
  typedef Mesh&                mesh_ref;
#endif


  /// Default constructor.
  VertexIterT()
    : mesh_(0), skip_bits_(0)
  {}


  /// Construct with mesh and a target handle.
  VertexIterT(mesh_ref _mesh, value_handle _hnd, bool _skip=false)
    : mesh_(&_mesh), hnd_(_hnd), skip_bits_(0)
  {
    if (_skip) enable_skipping();

    // Set vertex handle invalid if the mesh contains no vertex
    if(_mesh.n_vertices() == 0) hnd_ = value_handle(-1);
  }


  /// Copy constructor
  VertexIterT(const VertexIterT& _rhs)
    : mesh_(_rhs.mesh_), hnd_(_rhs.hnd_), skip_bits_(_rhs.skip_bits_)
  {}


  /// Assignment operator
  VertexIterT& operator=(const VertexIterT<Mesh>& _rhs)
  {
    mesh_      = _rhs.mesh_;
    hnd_       = _rhs.hnd_;
    skip_bits_ = _rhs.skip_bits_;
    return *this;
  }


#if 0

  /// Construct from a non-const iterator
  VertexIterT(const VertexIterT<Mesh>& _rhs)
    : mesh_(_rhs.mesh_), hnd_(_rhs.hnd_), skip_bits_(_rhs.skip_bits_)
  {}


  /// Assignment from non-const iterator
  VertexIterT& operator=(const VertexIterT<Mesh>& _rhs)
  {
    mesh_      = _rhs.mesh_;
    hnd_       = _rhs.hnd_;
    skip_bits_ = _rhs.skip_bits_;
    return *this;
  }

#else
  friend class ConstVertexIterT<Mesh>;
#endif


  /// Standard dereferencing operator.
  reference operator*()  const { return mesh_->deref(hnd_); }

  /// Standard pointer operator.
  pointer   operator->() const { return &(mesh_->deref(hnd_)); }

  /// Get the handle of the item the iterator refers to.
  value_handle handle() const { return hnd_; }

  /// Cast to the handle of the item the iterator refers to.
  operator value_handle() const { return hnd_; }

  /// Are two iterators equal? Only valid if they refer to the same mesh!
  bool operator==(const VertexIterT& _rhs) const
  { return ((mesh_ == _rhs.mesh_) && (hnd_ == _rhs.hnd_)); }

  /// Not equal?
  bool operator!=(const VertexIterT& _rhs) const
  { return !operator==(_rhs); }

  /// Standard pre-increment operator
  VertexIterT& operator++()
  { hnd_.__increment(); if (skip_bits_) skip_fwd(); return *this; }

  /// Standard pre-decrement operator
  VertexIterT& operator--()
  { hnd_.__decrement(); if (skip_bits_) skip_bwd(); return *this; }


  /// Turn on skipping: automatically skip deleted/hidden elements
  void enable_skipping()
  {
    if (mesh_ && mesh_->has_vertex_status())
    {
      Attributes::StatusInfo status;
      status.set_deleted(true);
      status.set_hidden(true);
      skip_bits_ = status.bits();
      skip_fwd();
    }
    else skip_bits_ = 0;
  }


  /// Turn on skipping: automatically skip deleted/hidden elements
  void disable_skipping() { skip_bits_ = 0; }



private:

  void skip_fwd()
  {
    assert(mesh_ && skip_bits_);
    while ((hnd_.idx() < (signed) mesh_->n_vertices()) &&
	   (mesh_->status(hnd_).bits() & skip_bits_))
      hnd_.__increment();
  }


  void skip_bwd()
  {
    assert(mesh_ && skip_bits_);
    while ((hnd_.idx() >= 0) &&
	   (mesh_->status(hnd_).bits() & skip_bits_))
      hnd_.__decrement();
  }



private:
  mesh_ptr      mesh_;
  value_handle  hnd_;
  unsigned int  skip_bits_;
};


//== CLASS DEFINITION =========================================================


/** \class ConstVertexIterT IteratorsT.hh <OpenMesh/Mesh/Iterators/IteratorsT.hh>
    Linear iterator.
*/

template <class Mesh>
class ConstVertexIterT
{
public:


  //--- Typedefs ---

  typedef typename Mesh::Vertex           value_type;
  typedef typename Mesh::VertexHandle         value_handle;

#if 1
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef std::ptrdiff_t       difference_type;
  typedef const value_type&    reference;
  typedef const value_type*    pointer;
  typedef const Mesh*          mesh_ptr;
  typedef const Mesh&          mesh_ref;
#else
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef std::ptrdiff_t       difference_type;
  typedef value_type&          reference;
  typedef value_type*          pointer;
  typedef Mesh*                mesh_ptr;
  typedef Mesh&                mesh_ref;
#endif




  /// Default constructor.
  ConstVertexIterT()
    : mesh_(0), skip_bits_(0)
  {}


  /// Construct with mesh and a target handle.
  ConstVertexIterT(mesh_ref _mesh, value_handle _hnd, bool _skip=false)
    : mesh_(&_mesh), hnd_(_hnd), skip_bits_(0)
  {
    if (_skip) enable_skipping();

    // Set vertex handle invalid if the mesh contains no vertex
    if(_mesh.n_vertices() == 0) hnd_ = value_handle(-1);
  }


  /// Copy constructor
  ConstVertexIterT(const ConstVertexIterT& _rhs)
    : mesh_(_rhs.mesh_), hnd_(_rhs.hnd_), skip_bits_(_rhs.skip_bits_)
  {}


  /// Assignment operator
  ConstVertexIterT& operator=(const ConstVertexIterT<Mesh>& _rhs)
  {
    mesh_      = _rhs.mesh_;
    hnd_       = _rhs.hnd_;
    skip_bits_ = _rhs.skip_bits_;
    return *this;
  }


#if 1

  /// Construct from a non-const iterator
  ConstVertexIterT(const VertexIterT<Mesh>& _rhs)
    : mesh_(_rhs.mesh_), hnd_(_rhs.hnd_), skip_bits_(_rhs.skip_bits_)
  {}


  /// Assignment from non-const iterator
  ConstVertexIterT& operator=(const VertexIterT<Mesh>& _rhs)
  {
    mesh_      = _rhs.mesh_;
    hnd_       = _rhs.hnd_;
    skip_bits_ = _rhs.skip_bits_;
    return *this;
  }

#else
  friend class ConstVertexIterT<Mesh>;
#endif


  /// Standard dereferencing operator.
  reference operator*()  const { return mesh_->deref(hnd_); }

  /// Standard pointer operator.
  pointer   operator->() const { return &(mesh_->deref(hnd_)); }

  /// Get the handle of the item the iterator refers to.
  value_handle handle() const { return hnd_; }

  /// Cast to the handle of the item the iterator refers to.
  operator value_handle() const { return hnd_; }

  /// Are two iterators equal? Only valid if they refer to the same mesh!
  bool operator==(const ConstVertexIterT& _rhs) const
  { return ((mesh_ == _rhs.mesh_) && (hnd_ == _rhs.hnd_)); }

  /// Not equal?
  bool operator!=(const ConstVertexIterT& _rhs) const
  { return !operator==(_rhs); }

  /// Standard pre-increment operator
  ConstVertexIterT& operator++()
  { hnd_.__increment(); if (skip_bits_) skip_fwd(); return *this; }

  /// Standard pre-decrement operator
  ConstVertexIterT& operator--()
  { hnd_.__decrement(); if (skip_bits_) skip_bwd(); return *this; }


  /// Turn on skipping: automatically skip deleted/hidden elements
  void enable_skipping()
  {
    if (mesh_ && mesh_->has_vertex_status())
    {
      Attributes::StatusInfo status;
      status.set_deleted(true);
      status.set_hidden(true);
      skip_bits_ = status.bits();
      skip_fwd();
    }
    else skip_bits_ = 0;
  }


  /// Turn on skipping: automatically skip deleted/hidden elements
  void disable_skipping() { skip_bits_ = 0; }



private:

  void skip_fwd()
  {
    assert(mesh_ && skip_bits_);
    while ((hnd_.idx() < (signed) mesh_->n_vertices()) &&
	   (mesh_->status(hnd_).bits() & skip_bits_))
      hnd_.__increment();
  }


  void skip_bwd()
  {
    assert(mesh_ && skip_bits_);
    while ((hnd_.idx() >= 0) &&
	   (mesh_->status(hnd_).bits() & skip_bits_))
      hnd_.__decrement();
  }



private:
  mesh_ptr      mesh_;
  value_handle  hnd_;
  unsigned int  skip_bits_;
};


//== CLASS DEFINITION =========================================================


/** \class HalfedgeIterT IteratorsT.hh <OpenMesh/Mesh/Iterators/IteratorsT.hh>
    Linear iterator.
*/

template <class Mesh>
class HalfedgeIterT
{
public:


  //--- Typedefs ---

  typedef typename Mesh::Halfedge           value_type;
  typedef typename Mesh::HalfedgeHandle         value_handle;

#if 0
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef std::ptrdiff_t       difference_type;
  typedef const value_type&    reference;
  typedef const value_type*    pointer;
  typedef const Mesh*          mesh_ptr;
  typedef const Mesh&          mesh_ref;
#else
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef std::ptrdiff_t       difference_type;
  typedef value_type&          reference;
  typedef value_type*          pointer;
  typedef Mesh*                mesh_ptr;
  typedef Mesh&                mesh_ref;
#endif




  /// Default constructor.
  HalfedgeIterT()
    : mesh_(0), skip_bits_(0)
  {}


  /// Construct with mesh and a target handle.
  HalfedgeIterT(mesh_ref _mesh, value_handle _hnd, bool _skip=false)
    : mesh_(&_mesh), hnd_(_hnd), skip_bits_(0)
  {
    if (_skip) enable_skipping();

    // Set halfedge handle invalid if the mesh contains no edge
    if(_mesh.n_edges() == 0) hnd_ = value_handle(-1);
  }


  /// Copy constructor
  HalfedgeIterT(const HalfedgeIterT& _rhs)
    : mesh_(_rhs.mesh_), hnd_(_rhs.hnd_), skip_bits_(_rhs.skip_bits_)
  {}


  /// Assignment operator
  HalfedgeIterT& operator=(const HalfedgeIterT<Mesh>& _rhs)
  {
    mesh_      = _rhs.mesh_;
    hnd_       = _rhs.hnd_;
    skip_bits_ = _rhs.skip_bits_;
    return *this;
  }


#if 0

  /// Construct from a non-const iterator
  HalfedgeIterT(const HalfedgeIterT<Mesh>& _rhs)
    : mesh_(_rhs.mesh_), hnd_(_rhs.hnd_), skip_bits_(_rhs.skip_bits_)
  {}


  /// Assignment from non-const iterator
  HalfedgeIterT& operator=(const HalfedgeIterT<Mesh>& _rhs)
  {
    mesh_      = _rhs.mesh_;
    hnd_       = _rhs.hnd_;
    skip_bits_ = _rhs.skip_bits_;
    return *this;
  }

#else
  friend class ConstHalfedgeIterT<Mesh>;
#endif


  /// Standard dereferencing operator.
  reference operator*()  const { return mesh_->deref(hnd_); }

  /// Standard pointer operator.
  pointer   operator->() const { return &(mesh_->deref(hnd_)); }

  /// Get the handle of the item the iterator refers to.
  value_handle handle() const { return hnd_; }

  /// Cast to the handle of the item the iterator refers to.
  operator value_handle() const { return hnd_; }

  /// Are two iterators equal? Only valid if they refer to the same mesh!
  bool operator==(const HalfedgeIterT& _rhs) const
  { return ((mesh_ == _rhs.mesh_) && (hnd_ == _rhs.hnd_)); }

  /// Not equal?
  bool operator!=(const HalfedgeIterT& _rhs) const
  { return !operator==(_rhs); }

  /// Standard pre-increment operator
  HalfedgeIterT& operator++()
  { hnd_.__increment(); if (skip_bits_) skip_fwd(); return *this; }

  /// Standard pre-decrement operator
  HalfedgeIterT& operator--()
  { hnd_.__decrement(); if (skip_bits_) skip_bwd(); return *this; }


  /// Turn on skipping: automatically skip deleted/hidden elements
  void enable_skipping()
  {
    if (mesh_ && mesh_->has_halfedge_status())
    {
      Attributes::StatusInfo status;
      status.set_deleted(true);
      status.set_hidden(true);
      skip_bits_ = status.bits();
      skip_fwd();
    }
    else skip_bits_ = 0;
  }


  /// Turn on skipping: automatically skip deleted/hidden elements
  void disable_skipping() { skip_bits_ = 0; }



private:

  void skip_fwd()
  {
    assert(mesh_ && skip_bits_);
    while ((hnd_.idx() < (signed) mesh_->n_halfedges()) &&
	   (mesh_->status(hnd_).bits() & skip_bits_))
      hnd_.__increment();
  }


  void skip_bwd()
  {
    assert(mesh_ && skip_bits_);
    while ((hnd_.idx() >= 0) &&
	   (mesh_->status(hnd_).bits() & skip_bits_))
      hnd_.__decrement();
  }



private:
  mesh_ptr      mesh_;
  value_handle  hnd_;
  unsigned int  skip_bits_;
};


//== CLASS DEFINITION =========================================================


/** \class ConstHalfedgeIterT IteratorsT.hh <OpenMesh/Mesh/Iterators/IteratorsT.hh>
    Linear iterator.
*/

template <class Mesh>
class ConstHalfedgeIterT
{
public:


  //--- Typedefs ---

  typedef typename Mesh::Halfedge           value_type;
  typedef typename Mesh::HalfedgeHandle         value_handle;

#if 1
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef std::ptrdiff_t       difference_type;
  typedef const value_type&    reference;
  typedef const value_type*    pointer;
  typedef const Mesh*          mesh_ptr;
  typedef const Mesh&          mesh_ref;
#else
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef std::ptrdiff_t       difference_type;
  typedef value_type&          reference;
  typedef value_type*          pointer;
  typedef Mesh*                mesh_ptr;
  typedef Mesh&                mesh_ref;
#endif




  /// Default constructor.
  ConstHalfedgeIterT()
    : mesh_(0), skip_bits_(0)
  {}


  /// Construct with mesh and a target handle.
  ConstHalfedgeIterT(mesh_ref _mesh, value_handle _hnd, bool _skip=false)
    : mesh_(&_mesh), hnd_(_hnd), skip_bits_(0)
  {
    if (_skip) enable_skipping();

    // Set halfedge handle invalid if the mesh contains no edge
    if(_mesh.n_edges() == 0) hnd_ = value_handle(-1);
  }


  /// Copy constructor
  ConstHalfedgeIterT(const ConstHalfedgeIterT& _rhs)
    : mesh_(_rhs.mesh_), hnd_(_rhs.hnd_), skip_bits_(_rhs.skip_bits_)
  {}


  /// Assignment operator
  ConstHalfedgeIterT& operator=(const ConstHalfedgeIterT<Mesh>& _rhs)
  {
    mesh_      = _rhs.mesh_;
    hnd_       = _rhs.hnd_;
    skip_bits_ = _rhs.skip_bits_;
    return *this;
  }


#if 1

  /// Construct from a non-const iterator
  ConstHalfedgeIterT(const HalfedgeIterT<Mesh>& _rhs)
    : mesh_(_rhs.mesh_), hnd_(_rhs.hnd_), skip_bits_(_rhs.skip_bits_)
  {}


  /// Assignment from non-const iterator
  ConstHalfedgeIterT& operator=(const HalfedgeIterT<Mesh>& _rhs)
  {
    mesh_      = _rhs.mesh_;
    hnd_       = _rhs.hnd_;
    skip_bits_ = _rhs.skip_bits_;
    return *this;
  }

#else
  friend class ConstHalfedgeIterT<Mesh>;
#endif


  /// Standard dereferencing operator.
  reference operator*()  const { return mesh_->deref(hnd_); }

  /// Standard pointer operator.
  pointer   operator->() const { return &(mesh_->deref(hnd_)); }

  /// Get the handle of the item the iterator refers to.
  value_handle handle() const { return hnd_; }

  /// Cast to the handle of the item the iterator refers to.
  operator value_handle() const { return hnd_; }

  /// Are two iterators equal? Only valid if they refer to the same mesh!
  bool operator==(const ConstHalfedgeIterT& _rhs) const
  { return ((mesh_ == _rhs.mesh_) && (hnd_ == _rhs.hnd_)); }

  /// Not equal?
  bool operator!=(const ConstHalfedgeIterT& _rhs) const
  { return !operator==(_rhs); }

  /// Standard pre-increment operator
  ConstHalfedgeIterT& operator++()
  { hnd_.__increment(); if (skip_bits_) skip_fwd(); return *this; }

  /// Standard pre-decrement operator
  ConstHalfedgeIterT& operator--()
  { hnd_.__decrement(); if (skip_bits_) skip_bwd(); return *this; }


  /// Turn on skipping: automatically skip deleted/hidden elements
  void enable_skipping()
  {
    if (mesh_ && mesh_->has_halfedge_status())
    {
      Attributes::StatusInfo status;
      status.set_deleted(true);
      status.set_hidden(true);
      skip_bits_ = status.bits();
      skip_fwd();
    }
    else skip_bits_ = 0;
  }


  /// Turn on skipping: automatically skip deleted/hidden elements
  void disable_skipping() { skip_bits_ = 0; }



private:

  void skip_fwd()
  {
    assert(mesh_ && skip_bits_);
    while ((hnd_.idx() < (signed) mesh_->n_halfedges()) &&
	   (mesh_->status(hnd_).bits() & skip_bits_))
      hnd_.__increment();
  }


  void skip_bwd()
  {
    assert(mesh_ && skip_bits_);
    while ((hnd_.idx() >= 0) &&
	   (mesh_->status(hnd_).bits() & skip_bits_))
      hnd_.__decrement();
  }



private:
  mesh_ptr      mesh_;
  value_handle  hnd_;
  unsigned int  skip_bits_;
};


//== CLASS DEFINITION =========================================================


/** \class EdgeIterT IteratorsT.hh <OpenMesh/Mesh/Iterators/IteratorsT.hh>
    Linear iterator.
*/

template <class Mesh>
class EdgeIterT
{
public:


  //--- Typedefs ---

  typedef typename Mesh::Edge           value_type;
  typedef typename Mesh::EdgeHandle         value_handle;

#if 0
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef std::ptrdiff_t       difference_type;
  typedef const value_type&    reference;
  typedef const value_type*    pointer;
  typedef const Mesh*          mesh_ptr;
  typedef const Mesh&          mesh_ref;
#else
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef std::ptrdiff_t       difference_type;
  typedef value_type&          reference;
  typedef value_type*          pointer;
  typedef Mesh*                mesh_ptr;
  typedef Mesh&                mesh_ref;
#endif




  /// Default constructor.
  EdgeIterT()
    : mesh_(0), skip_bits_(0)
  {}


  /// Construct with mesh and a target handle.
  EdgeIterT(mesh_ref _mesh, value_handle _hnd, bool _skip=false)
    : mesh_(&_mesh), hnd_(_hnd), skip_bits_(0)
  {
    if (_skip) enable_skipping();

    // Set halfedge handle invalid if the mesh contains no edge
    if(_mesh.n_edges() == 0) hnd_ = value_handle(-1);
  }


  /// Copy constructor
  EdgeIterT(const EdgeIterT& _rhs)
    : mesh_(_rhs.mesh_), hnd_(_rhs.hnd_), skip_bits_(_rhs.skip_bits_)
  {}


  /// Assignment operator
  EdgeIterT& operator=(const EdgeIterT<Mesh>& _rhs)
  {
    mesh_      = _rhs.mesh_;
    hnd_       = _rhs.hnd_;
    skip_bits_ = _rhs.skip_bits_;
    return *this;
  }


#if 0

  /// Construct from a non-const iterator
  EdgeIterT(const EdgeIterT<Mesh>& _rhs)
    : mesh_(_rhs.mesh_), hnd_(_rhs.hnd_), skip_bits_(_rhs.skip_bits_)
  {}


  /// Assignment from non-const iterator
  EdgeIterT& operator=(const EdgeIterT<Mesh>& _rhs)
  {
    mesh_      = _rhs.mesh_;
    hnd_       = _rhs.hnd_;
    skip_bits_ = _rhs.skip_bits_;
    return *this;
  }

#else
  friend class ConstEdgeIterT<Mesh>;
#endif


  /// Standard dereferencing operator.
  reference operator*()  const { return mesh_->deref(hnd_); }

  /// Standard pointer operator.
  pointer   operator->() const { return &(mesh_->deref(hnd_)); }

  /// Get the handle of the item the iterator refers to.
  value_handle handle() const { return hnd_; }

  /// Cast to the handle of the item the iterator refers to.
  operator value_handle() const { return hnd_; }

  /// Are two iterators equal? Only valid if they refer to the same mesh!
  bool operator==(const EdgeIterT& _rhs) const
  { return ((mesh_ == _rhs.mesh_) && (hnd_ == _rhs.hnd_)); }

  /// Not equal?
  bool operator!=(const EdgeIterT& _rhs) const
  { return !operator==(_rhs); }

  /// Standard pre-increment operator
  EdgeIterT& operator++()
  { hnd_.__increment(); if (skip_bits_) skip_fwd(); return *this; }

  /// Standard pre-decrement operator
  EdgeIterT& operator--()
  { hnd_.__decrement(); if (skip_bits_) skip_bwd(); return *this; }


  /// Turn on skipping: automatically skip deleted/hidden elements
  void enable_skipping()
  {
    if (mesh_ && mesh_->has_edge_status())
    {
      Attributes::StatusInfo status;
      status.set_deleted(true);
      status.set_hidden(true);
      skip_bits_ = status.bits();
      skip_fwd();
    }
    else skip_bits_ = 0;
  }


  /// Turn on skipping: automatically skip deleted/hidden elements
  void disable_skipping() { skip_bits_ = 0; }



private:

  void skip_fwd()
  {
    assert(mesh_ && skip_bits_);
    while ((hnd_.idx() < (signed) mesh_->n_edges()) &&
	   (mesh_->status(hnd_).bits() & skip_bits_))
      hnd_.__increment();
  }


  void skip_bwd()
  {
    assert(mesh_ && skip_bits_);
    while ((hnd_.idx() >= 0) &&
	   (mesh_->status(hnd_).bits() & skip_bits_))
      hnd_.__decrement();
  }



private:
  mesh_ptr      mesh_;
  value_handle  hnd_;
  unsigned int  skip_bits_;
};


//== CLASS DEFINITION =========================================================


/** \class ConstEdgeIterT IteratorsT.hh <OpenMesh/Mesh/Iterators/IteratorsT.hh>
    Linear iterator.
*/

template <class Mesh>
class ConstEdgeIterT
{
public:


  //--- Typedefs ---

  typedef typename Mesh::Edge           value_type;
  typedef typename Mesh::EdgeHandle         value_handle;

#if 1
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef std::ptrdiff_t       difference_type;
  typedef const value_type&    reference;
  typedef const value_type*    pointer;
  typedef const Mesh*          mesh_ptr;
  typedef const Mesh&          mesh_ref;
#else
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef std::ptrdiff_t       difference_type;
  typedef value_type&          reference;
  typedef value_type*          pointer;
  typedef Mesh*                mesh_ptr;
  typedef Mesh&                mesh_ref;
#endif




  /// Default constructor.
  ConstEdgeIterT()
    : mesh_(0), skip_bits_(0)
  {}


  /// Construct with mesh and a target handle.
  ConstEdgeIterT(mesh_ref _mesh, value_handle _hnd, bool _skip=false)
    : mesh_(&_mesh), hnd_(_hnd), skip_bits_(0)
  {
    if (_skip) enable_skipping();

    // Set halfedge handle invalid if the mesh contains no edge
    if(_mesh.n_edges() == 0) hnd_ = value_handle(-1);
  }


  /// Copy constructor
  ConstEdgeIterT(const ConstEdgeIterT& _rhs)
    : mesh_(_rhs.mesh_), hnd_(_rhs.hnd_), skip_bits_(_rhs.skip_bits_)
  {}


  /// Assignment operator
  ConstEdgeIterT& operator=(const ConstEdgeIterT<Mesh>& _rhs)
  {
    mesh_      = _rhs.mesh_;
    hnd_       = _rhs.hnd_;
    skip_bits_ = _rhs.skip_bits_;
    return *this;
  }


#if 1

  /// Construct from a non-const iterator
  ConstEdgeIterT(const EdgeIterT<Mesh>& _rhs)
    : mesh_(_rhs.mesh_), hnd_(_rhs.hnd_), skip_bits_(_rhs.skip_bits_)
  {}


  /// Assignment from non-const iterator
  ConstEdgeIterT& operator=(const EdgeIterT<Mesh>& _rhs)
  {
    mesh_      = _rhs.mesh_;
    hnd_       = _rhs.hnd_;
    skip_bits_ = _rhs.skip_bits_;
    return *this;
  }

#else
  friend class ConstEdgeIterT<Mesh>;
#endif


  /// Standard dereferencing operator.
  reference operator*()  const { return mesh_->deref(hnd_); }

  /// Standard pointer operator.
  pointer   operator->() const { return &(mesh_->deref(hnd_)); }

  /// Get the handle of the item the iterator refers to.
  value_handle handle() const { return hnd_; }

  /// Cast to the handle of the item the iterator refers to.
  operator value_handle() const { return hnd_; }

  /// Are two iterators equal? Only valid if they refer to the same mesh!
  bool operator==(const ConstEdgeIterT& _rhs) const
  { return ((mesh_ == _rhs.mesh_) && (hnd_ == _rhs.hnd_)); }

  /// Not equal?
  bool operator!=(const ConstEdgeIterT& _rhs) const
  { return !operator==(_rhs); }

  /// Standard pre-increment operator
  ConstEdgeIterT& operator++()
  { hnd_.__increment(); if (skip_bits_) skip_fwd(); return *this; }

  /// Standard pre-decrement operator
  ConstEdgeIterT& operator--()
  { hnd_.__decrement(); if (skip_bits_) skip_bwd(); return *this; }


  /// Turn on skipping: automatically skip deleted/hidden elements
  void enable_skipping()
  {
    if (mesh_ && mesh_->has_edge_status())
    {
      Attributes::StatusInfo status;
      status.set_deleted(true);
      status.set_hidden(true);
      skip_bits_ = status.bits();
      skip_fwd();
    }
    else skip_bits_ = 0;
  }


  /// Turn on skipping: automatically skip deleted/hidden elements
  void disable_skipping() { skip_bits_ = 0; }



private:

  void skip_fwd()
  {
    assert(mesh_ && skip_bits_);
    while ((hnd_.idx() < (signed) mesh_->n_edges()) &&
	   (mesh_->status(hnd_).bits() & skip_bits_))
      hnd_.__increment();
  }


  void skip_bwd()
  {
    assert(mesh_ && skip_bits_);
    while ((hnd_.idx() >= 0) &&
	   (mesh_->status(hnd_).bits() & skip_bits_))
      hnd_.__decrement();
  }



private:
  mesh_ptr      mesh_;
  value_handle  hnd_;
  unsigned int  skip_bits_;
};


//== CLASS DEFINITION =========================================================


/** \class FaceIterT IteratorsT.hh <OpenMesh/Mesh/Iterators/IteratorsT.hh>
    Linear iterator.
*/

template <class Mesh>
class FaceIterT
{
public:


  //--- Typedefs ---

  typedef typename Mesh::Face           value_type;
  typedef typename Mesh::FaceHandle         value_handle;

#if 0
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef std::ptrdiff_t       difference_type;
  typedef const value_type&    reference;
  typedef const value_type*    pointer;
  typedef const Mesh*          mesh_ptr;
  typedef const Mesh&          mesh_ref;
#else
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef std::ptrdiff_t       difference_type;
  typedef value_type&          reference;
  typedef value_type*          pointer;
  typedef Mesh*                mesh_ptr;
  typedef Mesh&                mesh_ref;
#endif




  /// Default constructor.
  FaceIterT()
    : mesh_(0), skip_bits_(0)
  {}


  /// Construct with mesh and a target handle.
  FaceIterT(mesh_ref _mesh, value_handle _hnd, bool _skip=false)
    : mesh_(&_mesh), hnd_(_hnd), skip_bits_(0)
  {
    if (_skip) enable_skipping();

    // Set face handle invalid if the mesh contains no faces
    if(_mesh.n_faces() == 0) hnd_ = value_handle(-1);
  }


  /// Copy constructor
  FaceIterT(const FaceIterT& _rhs)
    : mesh_(_rhs.mesh_), hnd_(_rhs.hnd_), skip_bits_(_rhs.skip_bits_)
  {}


  /// Assignment operator
  FaceIterT& operator=(const FaceIterT<Mesh>& _rhs)
  {
    mesh_      = _rhs.mesh_;
    hnd_       = _rhs.hnd_;
    skip_bits_ = _rhs.skip_bits_;
    return *this;
  }


#if 0

  /// Construct from a non-const iterator
  FaceIterT(const FaceIterT<Mesh>& _rhs)
    : mesh_(_rhs.mesh_), hnd_(_rhs.hnd_), skip_bits_(_rhs.skip_bits_)
  {}


  /// Assignment from non-const iterator
  FaceIterT& operator=(const FaceIterT<Mesh>& _rhs)
  {
    mesh_      = _rhs.mesh_;
    hnd_       = _rhs.hnd_;
    skip_bits_ = _rhs.skip_bits_;
    return *this;
  }

#else
  friend class ConstFaceIterT<Mesh>;
#endif


  /// Standard dereferencing operator.
  reference operator*()  const { return mesh_->deref(hnd_); }

  /// Standard pointer operator.
  pointer   operator->() const { return &(mesh_->deref(hnd_)); }

  /// Get the handle of the item the iterator refers to.
  value_handle handle() const { return hnd_; }

  /// Cast to the handle of the item the iterator refers to.
  operator value_handle() const { return hnd_; }

  /// Are two iterators equal? Only valid if they refer to the same mesh!
  bool operator==(const FaceIterT& _rhs) const
  { return ((mesh_ == _rhs.mesh_) && (hnd_ == _rhs.hnd_)); }

  /// Not equal?
  bool operator!=(const FaceIterT& _rhs) const
  { return !operator==(_rhs); }

  /// Standard pre-increment operator
  FaceIterT& operator++()
  { hnd_.__increment(); if (skip_bits_) skip_fwd(); return *this; }

  /// Standard pre-decrement operator
  FaceIterT& operator--()
  { hnd_.__decrement(); if (skip_bits_) skip_bwd(); return *this; }


  /// Turn on skipping: automatically skip deleted/hidden elements
  void enable_skipping()
  {
    if (mesh_ && mesh_->has_face_status())
    {
      Attributes::StatusInfo status;
      status.set_deleted(true);
      status.set_hidden(true);
      skip_bits_ = status.bits();
      skip_fwd();
    }
    else skip_bits_ = 0;
  }


  /// Turn on skipping: automatically skip deleted/hidden elements
  void disable_skipping() { skip_bits_ = 0; }



private:

  void skip_fwd()
  {
    assert(mesh_ && skip_bits_);
    while ((hnd_.idx() < (signed) mesh_->n_faces()) &&
	   (mesh_->status(hnd_).bits() & skip_bits_))
      hnd_.__increment();
  }


  void skip_bwd()
  {
    assert(mesh_ && skip_bits_);
    while ((hnd_.idx() >= 0) &&
	   (mesh_->status(hnd_).bits() & skip_bits_))
      hnd_.__decrement();
  }



private:
  mesh_ptr      mesh_;
  value_handle  hnd_;
  unsigned int  skip_bits_;
};


//== CLASS DEFINITION =========================================================


/** \class ConstFaceIterT IteratorsT.hh <OpenMesh/Mesh/Iterators/IteratorsT.hh>
    Linear iterator.
*/

template <class Mesh>
class ConstFaceIterT
{
public:


  //--- Typedefs ---

  typedef typename Mesh::Face           value_type;
  typedef typename Mesh::FaceHandle         value_handle;

#if 1
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef std::ptrdiff_t       difference_type;
  typedef const value_type&    reference;
  typedef const value_type*    pointer;
  typedef const Mesh*          mesh_ptr;
  typedef const Mesh&          mesh_ref;
#else
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef std::ptrdiff_t       difference_type;
  typedef value_type&          reference;
  typedef value_type*          pointer;
  typedef Mesh*                mesh_ptr;
  typedef Mesh&                mesh_ref;
#endif




  /// Default constructor.
  ConstFaceIterT()
    : mesh_(0), skip_bits_(0)
  {}


  /// Construct with mesh and a target handle.
  ConstFaceIterT(mesh_ref _mesh, value_handle _hnd, bool _skip=false)
    : mesh_(&_mesh), hnd_(_hnd), skip_bits_(0)
  {
    if (_skip) enable_skipping();

    // Set face handle invalid if the mesh contains no faces
    if(_mesh.n_faces() == 0) hnd_ = value_handle(-1);
  }


  /// Copy constructor
  ConstFaceIterT(const ConstFaceIterT& _rhs)
    : mesh_(_rhs.mesh_), hnd_(_rhs.hnd_), skip_bits_(_rhs.skip_bits_)
  {}


  /// Assignment operator
  ConstFaceIterT& operator=(const ConstFaceIterT<Mesh>& _rhs)
  {
    mesh_      = _rhs.mesh_;
    hnd_       = _rhs.hnd_;
    skip_bits_ = _rhs.skip_bits_;
    return *this;
  }


#if 1

  /// Construct from a non-const iterator
  ConstFaceIterT(const FaceIterT<Mesh>& _rhs)
    : mesh_(_rhs.mesh_), hnd_(_rhs.hnd_), skip_bits_(_rhs.skip_bits_)
  {}


  /// Assignment from non-const iterator
  ConstFaceIterT& operator=(const FaceIterT<Mesh>& _rhs)
  {
    mesh_      = _rhs.mesh_;
    hnd_       = _rhs.hnd_;
    skip_bits_ = _rhs.skip_bits_;
    return *this;
  }

#else
  friend class ConstFaceIterT<Mesh>;
#endif


  /// Standard dereferencing operator.
  reference operator*()  const { return mesh_->deref(hnd_); }

  /// Standard pointer operator.
  pointer   operator->() const { return &(mesh_->deref(hnd_)); }

  /// Get the handle of the item the iterator refers to.
  value_handle handle() const { return hnd_; }

  /// Cast to the handle of the item the iterator refers to.
  operator value_handle() const { return hnd_; }

  /// Are two iterators equal? Only valid if they refer to the same mesh!
  bool operator==(const ConstFaceIterT& _rhs) const
  { return ((mesh_ == _rhs.mesh_) && (hnd_ == _rhs.hnd_)); }

  /// Not equal?
  bool operator!=(const ConstFaceIterT& _rhs) const
  { return !operator==(_rhs); }

  /// Standard pre-increment operator
  ConstFaceIterT& operator++()
  { hnd_.__increment(); if (skip_bits_) skip_fwd(); return *this; }

  /// Standard pre-decrement operator
  ConstFaceIterT& operator--()
  { hnd_.__decrement(); if (skip_bits_) skip_bwd(); return *this; }


  /// Turn on skipping: automatically skip deleted/hidden elements
  void enable_skipping()
  {
    if (mesh_ && mesh_->has_face_status())
    {
      Attributes::StatusInfo status;
      status.set_deleted(true);
      status.set_hidden(true);
      skip_bits_ = status.bits();
      skip_fwd();
    }
    else skip_bits_ = 0;
  }


  /// Turn on skipping: automatically skip deleted/hidden elements
  void disable_skipping() { skip_bits_ = 0; }



private:

  void skip_fwd()
  {
    assert(mesh_ && skip_bits_);
    while ((hnd_.idx() < (signed) mesh_->n_faces()) &&
	   (mesh_->status(hnd_).bits() & skip_bits_))
      hnd_.__increment();
  }


  void skip_bwd()
  {
    assert(mesh_ && skip_bits_);
    while ((hnd_.idx() >= 0) &&
	   (mesh_->status(hnd_).bits() & skip_bits_))
      hnd_.__decrement();
  }



private:
  mesh_ptr      mesh_;
  value_handle  hnd_;
  unsigned int  skip_bits_;
};


//=============================================================================
} // namespace Iterators
} // namespace OpenMesh
//=============================================================================
#endif
//=============================================================================
