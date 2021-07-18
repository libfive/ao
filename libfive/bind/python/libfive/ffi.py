'''
Python bindings to the libfive CAD kernel
Copyright (C) 2021  Matt Keeter

This Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0. If a copy of the MPL was not distributed with this file,
You can obtain one at http://mozilla.org/MPL/2.0/.
'''

import ctypes
import os
import sys

def try_link(folder, name):
    if sys.platform == "linux" or sys.platform == "linux2":
        suffix = '.so'
    elif sys.platform == "darwin":
        suffix = '.dylib'
    elif sys.platform == "win32":
        suffix = '.dll'
    path = os.path.join(folder, name + suffix)
    try:
        return ctypes.cdll.LoadLibrary(path)
    except OSError:
        return None

def paths_for(folder):
    # In most cases, we are running from the top-level build folder, which
    # contains libfive/{folder}/{library}.{suffix}
    paths = [os.path.join('libfive', folder)]

    # On Windows, we may be running from the studio subfolder if Studio.exe
    # was double-checked, which puts the build directory up one level.
    if sys.platform == 'win32':
        paths.append(os.path.join('..', 'libfive', folder))

    # On linux, the libraries are installed to CMAKE_INSTALL_PREFIX/lib.
    if sys.platform == "linux" or sys.platform == "linux2":
        paths.append(os.path.join('${CMAKE_INSTALL_PREFIX}', 'lib'))
    paths.append("")
    framework_dir = os.environ.get('LIBFIVE_FRAMEWORK_DIR')
    if framework_dir:
        paths.insert(0, framework_dir)
    return paths

def link_lib(folder, name):
    for p in paths_for(folder):
        lib = try_link(p, name)
        if lib is not None:
            return lib
    raise RuntimeError("Could not find {} library".format(name))

lib = link_lib('src', 'libfive')
stdlib = link_lib('stdlib', 'libfive-stdlib')

################################################################################

class libfive_interval_t(ctypes.Structure):
    _fields_ = [("lower", ctypes.c_float), ("upper", ctypes.c_float)]
class libfive_region_t(ctypes.Structure):
    _fields_ = [("X", libfive_interval_t),
                ("Y", libfive_interval_t),
                ("Z", libfive_interval_t)]
class libfive_vec3_t(ctypes.Structure):
    _fields_ = [("x", ctypes.c_float),
                ("y", ctypes.c_float),
                ("z", ctypes.c_float)]

libfive_tree = ctypes.c_void_p

class libfive_tri_t(ctypes.Structure):
    _fields_ = [("a", ctypes.c_uint32),
                ("b", ctypes.c_uint32),
                ("c", ctypes.c_uint32)]

class libfive_mesh_t(ctypes.Structure):
    _fields_ = [("verts", ctypes.POINTER(libfive_vec3_t)),
                ("tris",  ctypes.POINTER(libfive_tri_t)),
                ("tri_count", ctypes.c_uint32),
                ("vert_count", ctypes.c_uint32)]

################################################################################

# Types used in the libfive stdlib
class tvec2(ctypes.Structure):
    _fields_ = [("x", libfive_tree), ("y", libfive_tree)]
    def __init__(self, x, y):
        super().__init__(x, y)

class tvec3(ctypes.Structure):
    _fields_ = [("x", libfive_tree), ("y", libfive_tree), ("z", libfive_tree)]
    def __init__(self, x, y, z):
        super().__init__(x, y, z)
tfloat = libfive_tree

################################################################################
# Function signatures
lib.libfive_tree_delete.argtypes = [libfive_tree]

lib.libfive_tree_const.argtypes = [ctypes.c_float]
lib.libfive_tree_const.restype = libfive_tree

lib.libfive_opcode_enum.argtypes = [ctypes.c_char_p]
lib.libfive_opcode_enum.restype = ctypes.c_int

lib.libfive_tree_is_var.argtypes = [libfive_tree]
lib.libfive_tree_is_var.restype = ctypes.c_uint8

lib.libfive_opcode_args.argtypes = [ctypes.c_int]
lib.libfive_opcode_args.restype = ctypes.c_int

lib.libfive_tree_nullary.argtypes = [ctypes.c_int]
lib.libfive_tree_nullary.restype = libfive_tree

lib.libfive_tree_unary.argtypes = [ctypes.c_int, libfive_tree]
lib.libfive_tree_unary.restype = libfive_tree

lib.libfive_tree_binary.argtypes = [ctypes.c_int, libfive_tree, libfive_tree]
lib.libfive_tree_binary.restype = libfive_tree

lib.libfive_tree_id.argtypes = [libfive_tree]
lib.libfive_tree_id.restype = ctypes.c_void_p

lib.libfive_tree_remap.argtypes = [libfive_tree, libfive_tree, libfive_tree, libfive_tree]
lib.libfive_tree_remap.restype = libfive_tree

lib.libfive_tree_print.argtypes = [libfive_tree]
lib.libfive_tree_print.restype = ctypes.c_void_p # actually a c_char_p,
# but we don't want Python to auto-convert into a bytestring

lib.libfive_free_str.argtypes = [ctypes.c_char_p]

lib.libfive_tree_save_mesh.argtypes = [libfive_tree, libfive_region_t, ctypes.c_float, ctypes.c_char_p]
lib.libfive_tree_save_mesh.restype = ctypes.c_uint8

lib.libfive_tree_save_meshes.argtypes = [libfive_tree, libfive_region_t, ctypes.c_float, ctypes.c_float, ctypes.c_char_p]
lib.libfive_tree_save_meshes.restype = ctypes.c_uint8

lib.libfive_tree_save.argtypes = [libfive_tree, ctypes.c_char_p]
lib.libfive_tree_save.restype = ctypes.c_bool

lib.libfive_tree_load.argtypes = [ctypes.c_char_p]
lib.libfive_tree_load.restype = libfive_tree

lib.libfive_tree_eval_f.argtypes = [libfive_tree, libfive_vec3_t]
lib.libfive_tree_eval_f.restype = ctypes.c_float

lib.libfive_tree_eval_r.argtypes = [libfive_tree, libfive_region_t]
lib.libfive_tree_eval_r.restype = libfive_interval_t

lib.libfive_tree_eval_d.argtypes = [libfive_tree, libfive_vec3_t]
lib.libfive_tree_eval_d.restype = libfive_vec3_t

lib.libfive_tree_optimized.argtypes = [libfive_tree]
lib.libfive_tree_optimized.restype = libfive_tree

lib.libfive_tree_render_mesh.argtypes = [libfive_tree, libfive_region_t, ctypes.c_float]
lib.libfive_tree_render_mesh.restype = ctypes.POINTER(libfive_mesh_t)

lib.libfive_mesh_delete.argtypes = [ctypes.POINTER(libfive_mesh_t)]
