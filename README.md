# Commonlib.h

Standard Library for C

### Prefix

All functions/variables/macros/structs have the prefix **c_/C_** to them. If you know their won't be any name collisions if you remove them, you can do so by defining COMMONLIB\_REMOVE\_PREFIX before including the header like:
```c
#define COMMONLIB_REMOVE_PREFIX
#include <commonlib.h>
```


### Type-defs
| C type           | typedef |
| ---------------- | ------- |
| unsigned int     | uint    |
| uint8\_t         | uint8   |
| uint16\_t        | uint16  |
| uint32\_t        | uint32  |
| uint64\_t        | uint64  |
| int8\_t          | int8    |
| int16\_t         | int16   |
| int32\_t         | int32   |
| int64\_t         | int64   |
| float            | float32 |
| double           | float64 |
| wchar\_t         | wchar   |
| const char*      | cstr    |
| const wchar*     | wstr    |

---

### Dynamic array
Dynamic array functions are implemented using macros and except a struct of your own with like:
```C
typedef struct {
   <item-type> *items;
   size_t count;
   size_t capacity;
   [extra fields...];
}
```

#### Operations on dynamic arrays:
- **c\_darr\_append(da, elm)**
> Appends the element `elm` to the dynamic array `da`
- **c\_darr\_shift(da)**
> Shifts the elements of the dynamic array `da` to the left by one

> [!WARNING]
> **c_darr_shift will make the da loose its ptr, so store the ptr elsewhere if you want to free it later!!!**

- **c\_darr\_delete(da, type, idx)**
> Deletes the element at index `idx` from the dynamic array `da`; `type` is needed because we swap the element with the last element in the array (*SEE <a href="./commonlib.h#L257">IMPLEMENTATION</a> FOR MORE INFO*).

- **c\_darr\_remove(da, type, elm_ptr, idx)**
> Same as **c\_darr\_delete()** but user can provide a pointer `elm_ptr` that will be filled with the removed element.


# WIP: Documentation
