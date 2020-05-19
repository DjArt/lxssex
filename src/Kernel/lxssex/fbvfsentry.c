#include "default.h"

VFS_ENTRY VfsFbEntries[1] =
{
    {
        VfsNodeEntry,
        RTL_CONSTANT_STRING(L"/dev/fb0"),
        .Node =
        {
            0, 0,
            S_IFCHR | S_IWUSR | S_IRUSR | S_IROTH | S_IWOTH,
            0,
            29
        }
    },
};