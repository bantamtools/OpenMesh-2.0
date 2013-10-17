# gp_item_default_embedded_path_override item default_embedded_path_var
#
# Return the path that others should refer to the item by when the item
# is embedded inside a bundle.
#
# This is a project-specific override of BundleUtilities.cmake's
# gp_item_default_embedded_path
#
function(gp_item_default_embedded_path_override item default_embedded_path_var)

  # By default, embed items as set by gp_item_default_embedded_path:
  #
  set(path "${${default_embedded_path_var}}")

  # But for OpenFlipper...
  #
  # ...embed *.dylib in the Libraries folder:
  #
  if(item MATCHES "\\.dylib$")
    set(path "@executable_path/../Libraries")
  endif()

  # ...embed qt plugins in the Libraries folder:
  #
  if(item MATCHES "/Developer/Applications/Qt/plugins" AND item MATCHES "\\.bundle")
    file (RELATIVE_PATH _plugin "/Developer/Applications/Qt/plugins" "${item}")
    get_filename_component(_dir "${_plugin}" PATH)
    set(path "@executable_path/../Resources/QtPlugins/${_dir}")
  endif()

  # ...embed *.so in the Plugins folder:
  #
  if(item MATCHES "\\.so$")
    set(path "@executable_path/../Resources/Plugins")
  endif()

  set(${default_embedded_path_var} "${path}" PARENT_SCOPE)
endfunction(gp_item_default_embedded_path_override)

include (BundleUtilities)

# copy qt plugins to bundle
file (GLOB _plugins "/Users/martin/src/otherfab/data/OpenMesh-2.0/cmake/Build/OpenFlipper.app/Contents/Resources/Plugins/*.so")
file (GLOB_RECURSE _qtplugins "/Developer/Applications/Qt/plugins/*.bundle")
foreach (_qtp ${_qtplugins})
  get_filename_component(_dir "${_qtp}" PATH)
  list(APPEND _qtdirs "${_dir}")
endforeach ()

# Get library paths
get_filename_component(_GlutDir "-framework GLUT" PATH)
get_filename_component(_GlewDir "/usr/lib/libGLEW.dylib" PATH)

# fix all dependencies
fixup_bundle ("/Users/martin/src/otherfab/data/OpenMesh-2.0/cmake/Build/bin/DecimaterGui" "" "${_GlutDir};${_GlewDir}")
fixup_bundle ("/Users/martin/src/otherfab/data/OpenMesh-2.0/cmake/Build/bin/ProgViewer" "" "${_GlutDir};${_GlewDir}")
fixup_bundle ("/Users/martin/src/otherfab/data/OpenMesh-2.0/cmake/Build/bin/QtViewer" "" "${_GlutDir};${_GlewDir}")
fixup_bundle ("/Users/martin/src/otherfab/data/OpenMesh-2.0/cmake/Build/bin/SubdividerGui" "" "${_GlutDir};${_GlewDir}")

fixup_bundle (/Users/martin/src/otherfab/data/OpenMesh-2.0/cmake/Build/bin/DecimaterGui "${_qtplugins}" "/usr/lib;${_qtdirs}")
fixup_bundle (/Users/martin/src/otherfab/data/OpenMesh-2.0/cmake/Build/bin/ProgViewer "${_qtplugins}" "/usr/lib;${_qtdirs}")
fixup_bundle (/Users/martin/src/otherfab/data/OpenMesh-2.0/cmake/Build/bin/QtViewer "${_qtplugins}" "/usr/lib;${_qtdirs}")
fixup_bundle (/Users/martin/src/otherfab/data/OpenMesh-2.0/cmake/Build/bin/SubdividerGui "${_qtplugins}" "/usr/lib;${_qtdirs}")

# create qt plugin configuration file
# file(WRITE "/Users/martin/src/otherfab/data/OpenMesh-2.0/cmake/Build/OpenMesh.app/Contents/Resources/qt.conf" "[Paths]\nPlugins = Resources/QtPlugins")

