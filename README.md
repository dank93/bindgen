# Bindgen
Minimally invasive Python binding generation for C++ source code.
Mainly useful for complicated / nested aggregate data types.

## Features
- Bindings for a type and all its upstream dependencies are triggered
  by tagging that type with a single comment
- All public member variables are exposed
- Template instantiations are supported
- Generated bindings are nested into submodules that mirror namespaces
  in C++ source

## Quirks
- Member functions are not exposed (yet)
- C-style arrays are not supported (yet)
- No Standard Library bindings are generated, though pybind11
  automatically supports many containers
- Exposed containers can only be overwritten in their entirety;
  changes to single elements won't stick
- No idea what happens w.r.t. inheritance
