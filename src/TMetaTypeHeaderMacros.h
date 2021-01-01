#ifndef __TMETATYPEMACROS_H__
#define __TMETATYPEMACROS_H__

#include "TMetaTypeMacrosInternal.h"

#define TMETA_CLASS(class_name)              \
  TMETA_CLASS_ACCESSORS_INTERNAL(class_name) \
  TMETA_CLASS_INTERNAL(class_name, __FILE__, __LINE__)

#endif  // __TMETATYPEMACROS_H__