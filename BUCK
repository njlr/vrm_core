include_defs('//BUCKAROO_DEPS')

prebuilt_cxx_library(
  name = 'vrm-core',
  header_only = True,
  header_namespace = '',
  exported_headers = subdir_glob([
    ('include', '**/*.hpp'),
    ('include', '**/*.inl'),
    ('include', '**/*.cpp'),
  ]),
  visibility = [
    'PUBLIC',
  ],
  deps = BUCKAROO_DEPS,
)
