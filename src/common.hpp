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

#define origin2d {0, 0}
#define origin3d {0, 0, 0}
#define origin4d {0, 0, 0, 0}

#define VECTOR_SIZE_BYTES(vec) (vec.size() * sizeof(decltype(vec)::value_type))

#endif /* COMMON_HPP */
