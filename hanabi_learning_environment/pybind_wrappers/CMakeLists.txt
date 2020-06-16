# target_link_libraries (PyHanabi LINK_PUBLIC hanabi)
pybind11_add_module(PyHanabi pyhanabi.cc)
target_link_libraries(PyHanabi PRIVATE hanabi)

install(TARGETS PyHanabi LIBRARY DESTINATION hanabi_learning_environment)
