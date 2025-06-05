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

namespace boost {
namespace hana {}
} // namespace boost

namespace filesystem = std::filesystem;
namespace hana = boost::hana;

#define origin2d {0, 0}
#define origin3d {0, 0, 0}
#define origin4d {0, 0, 0, 0}

#define VECTOR_SIZE_BYTES(vec) (vec.size() * sizeof(typename decltype(vec)::value_type))
#define ALL_OF(vec) vec.begin(), vec.end()

#endif /* COMMON_HPP */
