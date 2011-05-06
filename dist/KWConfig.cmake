# This file is used when other components needs to use something provided by this DCL. 
# Provide any include paths and lib directories. Use /home/kasia/workspace/discode_components/KW/dist
# to point to 'dist' directory of current DCL, it'll be substituted during installation. 

# directory containing header files
SET(KW_INCLUDE_DIR /home/kasia/workspace/discode_components/KW/dist/include)
INCLUDE_DIRECTORIES(${KW_INCLUDE_DIR})

# directory containing libraries
SET(KW_LIB_DIR /home/kasia/workspace/discode_components/KW/dist/lib)
LINK_DIRECTORIES(${KW_LIB_DIR})

# list of libraries to link against when using features of KW
# add all additional libraries built by this dcl (NOT components)
# SET(KW_LIBS lib_1 lib_2)
# SET(ADDITIONAL_LIB_DIRS /home/kasia/workspace/discode_components/KW/dist/lib ${ADDITIONAL_LIB_DIRS})
