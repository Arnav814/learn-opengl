#ifndef COMMON_HPP
#define COMMON_HPP

// this file should be included everywhere

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

// forward declare to avoid including more stuff in this file
namespace std {
namespace filesystem {}
} // namespace std

namespace filesystem = std::filesystem;

#endif /* COMMON_HPP */
