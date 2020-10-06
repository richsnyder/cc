# TODO

## Throughout

* Documentation!

* Check preconditions.  For example, what happens if `self` equals `other`?

* Write macros to allow users to specialize containers so that functions that
  have void pointers as inputs and/or outputs instead accept typed values.

* Look through the C++ algorithms header for additional functions that ought to
  be supported by this library.

* Add shorthand methods for working with iterators.  Perhaps use `_Generic`
  for C and overloading for C++?  Lack of support for `_Generic` in C++ is a
  problem for a single solution strategy.

* Perhaps switch from `memcpy()` to `memcpy_s()`-type (C11 standard function).
  These newer functions provide better error handling, but don't appear to be
  supported by the C++ standard!

## Vectors

* Replace calls to `_cc_vector_grow_as_needed()` to avoid unnecessary copying.
  For example, in `cc_vector_insert`, a new data array is allocated to hold one
  more element than the old data array, then the elements are copied from the
  old array to the new array, then the new elements after the insertion point
  are shifted to the right, then the new value is finally copied into place.
  Copying straight from the old data array to the new array, with appropriate
  shifting, would avoid a lot of copy operations.

## Lists

* Refactor the `sort()` function so that the back doesn't need to be found
  by iterating through the list.

* Add unit tests for the list operations `merge()`, `splice()`, `remove()`,
  `remove_if()`, `reverse()`, `unique()`, and `sort()` when the list elements
  are structures with either shallow and deep copy requirements.

## Maps

* Add unit tests for the `merge()` operation when the map keys and/or values
  are structures with either shallow or deep copy requirements.
