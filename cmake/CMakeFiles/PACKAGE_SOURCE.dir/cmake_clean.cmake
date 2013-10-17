FILE(REMOVE_RECURSE
  "CMakeFiles/PACKAGE_SOURCE"
)

# Per-language clean rules from dependency scanning.
FOREACH(lang)
  INCLUDE(CMakeFiles/PACKAGE_SOURCE.dir/cmake_clean_${lang}.cmake OPTIONAL)
ENDFOREACH(lang)
