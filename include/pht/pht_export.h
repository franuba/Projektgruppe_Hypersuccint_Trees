#ifndef PROJECTGROUP_HYPERSUCCINCT_TREES_PHT_EXPORT_H_
#define PROJECTGROUP_HYPERSUCCINCT_TREES_PHT_EXPORT_H_

// __declspec(dllexport) is an MSVC-specific attribute used to mark a symbol
// for export from a Windows DLL. It is not understood by GCC/Clang and is a
// hard compile error there (unlike e.g. an unrecognized #pragma, which only
// warns). On non-Windows/non-MSVC builds, pht_lib is an ordinary shared
// library where symbols are visible by default, so no export attribute is
// needed at all. This macro compiles to the real attribute under MSVC and
// to nothing everywhere else.
#if defined(_MSC_VER)
#define PHT_EXPORT __declspec(dllexport)
#else
#define PHT_EXPORT
#endif

#endif//PROJECTGROUP_HYPERSUCCINCT_TREES_PHT_EXPORT_H_
