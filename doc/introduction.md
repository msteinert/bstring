Better String library {#mainpage}
=====================

*by Paul Hsieh*

The bstring library is an attempt to provide improved string processing
functionality to the C language. At the heart of the bstring library
(Bstrlib for short) is the management of `bstring`s which are a significant
improvement over `NULL` terminated char buffers.

Motivation
----------

The standard C string library has serious problems:

1. Its use of `NULL` to denote the end of the string means knowing a string's
   length is O(n) when it could be O(1).
2. It imposes an interpretation for the character value `NULL`.
3. `gets` always exposes the application to a buffer overflow.
4. `strtok` modifies the string its parsing and thus may not be usable in
   programs which are re-entrant or multithreaded.
5. fgets has the unusual semantic of ignoring `NULL`s that occur before
   newline characters are consumed.
6. There is no memory management, and actions performed such as `strcpy`,
   `strcat` and `sprintf` are common places for buffer overflows.
7. `strncpy` doesn't `NULL` terminate the destination in some cases.
8. Passing NULL to C library string functions causes an undefined NULL pointer
   access.
9. Parameter aliasing (overlapping, or self-referencing parameters) within
   most C library functions has undefined behavior.
10. Many C library string function calls take integer parameters with
    restricted legal ranges. Parameters passed outside these ranges are
    not typically detected and cause undefined behavior.

So the desire is to create an alternative string library that does not suffer
from the above problems and adds in the following functionality:

1. Incorporate string functionality seen from other languages.
    * `MID` from BASIC
    * `split`/`join` from Python
    * `string`/`char` `x n` from Perl
2. Implement analogs to functions that combine stream IO and char buffers
   without creating a dependency on stream IO functionality.
3. Implement the basic text editor-style functions `insert`, `delete`, `find`,
   and `replace`.
4. Implement reference based sub-string access (as a generalization of pointer
   arithmetic.)
5. Implement runtime write protection for strings.

There is also a desire to avoid "API-bloat." So functionality that can be
implemented trivially in other functionality is omitted. So there is no
`left` or `right` or `reverse` or anything like that as part of the core
functionality.

Explaining Bstrings
-------------------

A bstring is basically a header which wraps a pointer to a char buffer. Let's
start with the declaration of a struct tagbstring:

    struct tagbstring {
        int mlen;
        int slen;
        unsigned char * data;
    };

This definition is considered exposed, not opaque (though it is neither
necessary nor recommended that low level maintenance of bstrings be performed
whenever the abstract interfaces are sufficient). The `mlen` field (usually)
describes a lower bound for the memory allocated for the data field. The
`slen` field describes the exact length for the bstring. The data field is a
single contiguous buffer of unsigned chars. Note that the existence of a
`NULL` character in the unsigned char buffer pointed to by the data field
does not necessarily denote the end of the bstring.

To be a well formed modifiable bstring the `mlen` field must be at least the
length of the `slen` field, and slen must be non-negative. Furthermore, the
data field must point to a valid buffer in which access to the first `mlen`
characters has been acquired. So the minimal check for correctness is:

    (slen >= 0 && mlen >= slen && data != NULL)

Bstrings returned by bstring functions can be assumed to be either NULL or
satisfy the above property. (When bstrings are only readable, the `mlen >=
slen` restriction is not required; this is discussed later in this section.)
A bstring itself is just a pointer to a `struct tagbstring`:

    typedef struct tagbstring *bstring;

Bstrlib basically manages bstrings allocated as a header and an associated
data-buffer. Since the implementation is exposed, they can also be constructed
manually. Functions which mutate bstrings assume that the header and data
buffer have been malloced; the bstring library may perform `free` or `realloc`
on both the header and data buffer of any bstring parameter. Functions which
return bstring's create new bstrings. The string memory is freed by a
`bdestroy` call (or using the `bstrFree` macro).

Since bstrings maintain interoperability with C library char-buffer style
strings, all functions which modify, update or create bstrings also append a
`NULL` character into the position `slen + 1`. This trailing `NULL` character
is not required for bstrings input to the bstring functions; this is provided
solely as a convenience for interoperability with standard C char-buffer
functionality.

Analogs for the ANSI C string library functions have been created when they
are necessary, but have also been left out when they are not. In particular
there are no functions analogous to `fwrite`, or `puts` just for the purposes
of bstring. The `data` member of any string is exposed, and therefore can be
used just as easily as char buffers for C functions which read strings.

For those that wish to hand construct bstrings, the following should be kept
in mind:

1. While bstrlib can accept constructed bstrings without terminating `NULL`
   characters, the rest of the C language string library will not function
   properly on such non-terminated strings. This is obvious but must be kept
   in mind.
2. If it is intended that a constructed bstring be written to by the bstring
   library functions then the data portion should be allocated by the malloc
   function and the `slen` and `mlen` fields should be entered properly. The
   struct tagbstring header is not reallocated, and only freed by bdestroy.
3. Writing arbitrary `NULL` characters at various places in the string will
   not modify its length as perceived by the bstring library functions. In
   fact, `NULL` is a legitimate non-terminating character for a bstring to
   contain.
4. For read only parameters bstring functions do not check the `mlen`, i.e.,
   the minimal correctness requirements are reduced to
   `(slen >= 0 && data != NULL)`.

Better Pointer Arithmetic
-------------------------

One built-in feature of `NULL` terminated `char *` strings, is that its very
easy and fast to obtain a reference to the tail of any string using pointer
arithmetic. Bstrlib does one better by providing a way to get a reference to
any substring of a bstring (or any other length delimited block of memory).
So rather than just having pointer arithmetic, with bstrlib one essentially
has segment arithmetic. This is achieved using the macro `blk2tbstr` which
builds a reference to a block of memory and the macro `bmid2tbstr` which
builds a reference to a segment of a bstring. Bstrlib also includes functions
for direct consumption of memory blocks into bstrings, namely `bcatblk` and
`blk2bstr`.

One scenario where this can be extremely useful is when string contains many
substrings which one would like to pass as read-only reference parameters to
some string consuming function without the need to allocate entire new
containers for the string data. More concretely, imagine parsing a command
line string whose parameters are space delimited. This can only be done for
tails of the string with `NULL` terminated `char *` strings.

Improved NULL Semantics and Error Handling
------------------------------------------

Unless otherwise noted, if a NULL pointer is passed as a bstring or any other
detectably illegal parameter, the called function will return with an error
indicator (either `NULL` or `BSTR_ERR`) rather than simply performing a `NULL`
pointer access, or having undefined behavior.

To illustrate the value of this, consider the following example:

    strcpy(p = malloc (13 * sizeof(char)), "Hello,");
    strcat(p, " World");

This is not correct because `malloc` may return NULL (due to an out of memory
condition), and the behaviour of `strcpy` is undefined if either of its
parameters are NULL. The following however, is well defined:

    bstrcat(p = bfromcstr("Hello,"), q = bfromcstr(" World"));
    bdestroy(q);

If either p or q are assigned NULL (indicating a failure to allocate memory)
both bstrcat and bdestroy will recognize it and perform no detrimental action.

Note that it is not necessary to check any of the members of a returned
bstring for internal correctness (in particular the data member does not need
to be checked against `NULL` when the header is non-`NULL`), since this is
assured by the bstring library itself.

bStreams
--------

In addition to the `bgets` and `bread` functions, bstrlib can abstract streams
with a high performance read only stream called a bStream. In general, the
idea is to open a core stream (with something like `fopen`) then pass its
handle as well as a `bNread` function pointer (like `fread`) to the `bsopen`
function which will return a handle to an open bStream. Then the functions
`bsread`, `bsreadln` or `bsreadlns` can be called to read portions of the
stream. Finally, the `bsclose` function is called to close the `bStream` -- it
will return a handle to the original (core) stream. So bStreams, essentially,
wrap other streams.

The bStreams have two main advantages over the `bgets` and `bread` (as well as
`fgets`/`ungetc`) paradigms:

1. Improved functionality via the `bunread` function (which allows a stream to
   unread characters) and giving the bStream stack-like functionality if so
   desired.

2. A very high performance `bsreadln` function. The C library function `fgets`
   (and the `bgets` function) can typically be written as a loop on top of
   `fgetc`, thus paying all of the overhead costs of calling `fgetc` on a per
   character basis. `bsreadln` will read blocks at a time, thus amortizing the
   overhead of `fread` calls over many characters at once.

However, clearly bStreams are suboptimal or unusable for certain kinds of
streams, e.g. `stdin`, or certain usage patterns (a few spotty, or
non-sequential reads from a slow stream). For those situations, using `bgets`
is appropriate.

The semantics of bStreams allows practical construction of layerable data
streams. What this means is that by writing a `bNread` compatible function on
top of a bStream, one can construct a new bStream on top of it. This can be
useful for writing multi-pass parsers that don't actually read the entire
input more than once and don't require the use of intermediate storage.

Aliasing
--------

Aliasing occurs when a function is given two parameters which point to data
structures which overlap in the memory they occupy. While this does not
disturb read only functions, for many libraries this can make functions that
write to these memory locations malfunction. This is a common problem of the
C standard library and especially the string functions in the C standard
library.

The C standard string library is entirely chararacter by character oriented
(as is bstring) which makes conforming implementations alias safe for some
scenarios. However no actual detection of aliasing is typically performed,
so it is easy to find cases where the aliasing will cause anomolous or
undesirable behaviour (consider: `strcat(p, p)`). The C99 standard includes
the `restrict` pointer modifier which allows the compiler to document and
assume a no-alias condition on usage. However, only the most trivial cases
can be caught (if at all) by the compiler at compile time, and thus there is
no actual enforcement of non-aliasing.

Bstrlib, by contrast, permits aliasing and is completely aliasing safe, in
the C99 sense of aliasing. That is to say, under the assumption that pointers
of incompatible types from distinct objects can never alias, bstrlib is
completely aliasing safe. (In practice this means that the data buffer portion
of any bstring and header of any bstring are assumed to never alias). With the
exception of the reference building macros, the library behaves as if all
read-only parameters are first copied and replaced by temporary non-aliased
parameters before any writing to any output bstring is performed (though
actual copying is extremely rarely ever done).

Besides being a useful safety feature, bstring searching/comparison functions
can improve to O(1) execution when aliasing is detected.

Note that aliasing detection and handling code in Bstrlib is generally
extremely cheap. There is almost never any appreciable performance penalty
for using aliased parameters.

Reenterancy
-----------

Nearly every function in Bstrlib is a leaf function, and is completely
reenterable with the exception of writing to common bstrings. The split
functions which use a callback mechanism requires only that the source string
not be destroyed by the callback function unless the callback function returns
with an error status (note that Bstrlib functions which return an error do
not modify the string in any way.)  The string can in fact be modified by the
callback and the behaviour is deterministic. See the documentation of the
various split functions for more details.

Undefined Scenarios
-------------------

One of the basic important premises for Bstrlib is to not to increase the
propogation of undefined situations from parameters that are otherwise legal
in of themselves. In particular, except for extremely marginal cases, usages
of bstrings that use the bstring library functions alone cannot lead to any
undefined action. But due to C language and library limitations, there is no
way to define a non-trivial library that is completely without undefined
operations. All such possible undefined operations are described below:

1. Bstrings or struct tagbstrings that are not explicitely initialized cannot
   be passed as a parameter to any bstring function.
2. The members of the `NULL` bstring cannot be accessed directly. (Though all
   APIs and macros detect the `NULL` bstring.)
3. A bstring whose data member has not been obtained from a `malloc` or
   compatible call and which is write accessible passed as a writable
   parameter will lead to undefined results, i.e., do not `writeAllow` any
   constructed bstrings unless the data portion has been obtained from the
   heap.
4. If the headers of two strings alias but are not identical (which can only
   happen via a defective manual construction) then passing them to a bstring
   function in which one is writable is not defined.
5. If the `mlen` member is larger than the actual accessible length of the
   data member for a writable bstring, or if the `slen` member is larger than
   the readable length of the data member for a readable bstring, then the
   corresponding bstring operations are undefined.
6. Any bstring definition whose header or accessible data portion has been
   assigned to inaccessible or otherwise illegal memory clearly cannot be
   acted upon by the bstring library in any way.
7. Destroying the source of an incremental split from within the callback
   and not returning with a negative value (indicating that it should abort)
   will lead to undefined behaviour, although *modifying* or adjusting the
   state of the source data (even if those modifications fail within the
   bstrlib API) has well defined behavior.
8. Modifying a bstring which is write protected by direct access has undefined
   behavior.

While this may seem like a long list (with the exception of invalid uses of
the `writeAllow` macro) and source destruction during an iterative split
without an accompanying abort, no usage of the bstring API alone can cause
any undefined scenario to occur. The policy of restricting usage of bstrings
to the bstring API can significantly reduce the risk of runtime errors (in
practice it should eliminate them) related to string manipulation due to
undefined action.

Multithreading
--------------

A mutable bstring is kind of analogous to a small (two entry) linked list
allocated by `malloc`, with all aliasing completely under programmer control.
Manipulation of one bstring will never affect any other distinct bstring
unless explicitely constructed to do so by the programmer via hand
construction or via building a reference. Bstrlib also does not use any
static or global storage, so there are no hidden unremovable race conditions.
Bstrings are also clearly not inherently thread local. So just like
`char *`, bstrings can be passed around from thread to thread and shared and
so on, so long as modifications to a bstring correspond to some kind of
exclusive access lock as should be expected (or if the bstring is read-only,
which can be enforced by bstring write protection) for any sort of shared
object in a multithreaded environment.

Problems Not Solved
-------------------

Bstrlib is written for the C languages, which have inherent weaknesses that
cannot be easily solved:

1. Memory leaks: Forgetting to call `bdestroy` on a bstring that is about to
   be unreferenced, just as forgetting to call free on a heap buffer that is
   about to be dereferenced. Though bstrlib itself is leak free.
2. Read before write usage: In C, declaring an auto bstring does not
   automatically fill it with legal/valid contents. (The `bstrDeclare` and
   `bstrFree` macros from bstraux can be used to help mitigate this problem).

Other problems not addressed:

1. Built-in mutex usage to automatically avoid all bstring internal race
   conditions in multitasking environments: The problem with trying to
   implement such things at this low a level is that it is typically more
   efficient to use locks in higher level primitives. There is also no
   platform independent way to implement locks or mutexes.
2. Unicode/wide character support.

> Note: except for spotty support of wide characters, the default C standard
  library does not address any of these problems either.

The `bstest` Module
-------------------

The bstest module is just a unit test for the bstrlib module. For correct
implementations of bstrlib, it should execute with 0 failures being reported.
This test should be utilized if modifications/customizations to bstrlib have
been performed. It tests each core bstrlib function with bstrings of every
mode (read-only, NULL, static and mutable) and ensures that the expected
semantics are observed (including results that should indicate an error). It
also tests for aliasing support. Passing bstest is a necessary but not a
sufficient condition for ensuring the correctness of the bstrlib module.


Using Bstring an Alternative to the C Library
---------------------------------------------

First let us give a table of C library functions and the alternative bstring
functions that should be used instead of them.

<table>
<tr><th>C-library</th>  <th>Bstring Alternative</th>
<tr><td>gets</td>       <td>bgets</td>
<tr><td>strcpy</td>     <td>bassign</td>
<tr><td>strncpy</td>    <td>bassignmidstr</td>
<tr><td>strcat</td>     <td>bconcat</td>
<tr><td>strncat</td>    <td>bconcat+btrunc</td>
<tr><td>strtok</td>     <td>bsplit,bsplits</td>
<tr><td>sprintf</td>    <td>bformat</td>
<tr><td>snprintf</td>   <td>bformat + btrunc</td>
<tr><td>vsprintf</td>   <td>bvformata</td>
<tr><td>vsnprintf</td>  <td>bvformata + btrunc</td>
<tr><td>vfprintf</td>   <td>bvformata + fputs</td>
<tr><td>strcmp</td>     <td>biseq, bstrcmp</td>
<tr><td>strncmp</td>    <td>bstrncmp, memcmp</td>
<tr><td>strlen</td>     <td>slen, blength</td>
<tr><td>strdup</td>     <td>bstrcpy</td>
<tr><td>strset</td>     <td>bpattern</td>
<tr><td>strstr</td>     <td>binstr</td>
<tr><td>strpbrk</td>    <td>binchr</td>
<tr><td>stricmp</td>    <td>bstricmp</td>
<tr><td>strlwr</td>     <td>btolower</td>
<tr><td>strupr</td>     <td>btoupper</td>
<tr><td>strrev</td>     <td>bReverse (aux module)</td>
<tr><td>strchr</td>     <td>bstrchr</td>
<tr><td>strspnp</td>    <td>usestrspn</td>
<tr><td>ungetc</td>     <td>bsunread</td>
</table>

The top 9 C functions listed here are troublesome in that they impose memory
management in the calling function. The Bstring and CBstring interfaces have
built-in memory management, so there is far less code with far less potential
for buffer overrun problems. strtok can only be reliably called as a "leaf"
calculation, since it (quite bizarrely) maintains hidden internal state. And
gets is well known to be broken no matter what. The Bstrlib alternatives do
not suffer from those sorts of problems.

The substitute for `strncat` can be performed with higher performance by
using the `blk2tbstr` macro to create a presized second operand for `bconcat`.

<table>
<tr><th>C-library</th>  <th>Bstring alternative</th>
<tr><td>strspn</td>     <td>strspn acceptable</td>
<tr><td>strcspn</td>    <td>strcspn acceptable</td>
<tr><td>strnset</td>    <td>strnset acceptable</td>
<tr><td>printf</td>     <td>printf acceptable</td>
<tr><td>puts</td>       <td>puts acceptable</td>
<tr><td>fprintf</td>    <td>fprintf acceptable</td>
<tr><td>fputs</td>      <td>fputs acceptable</td>
<tr><td>memcmp</td>     <td>memcmp acceptable</td>
</table>

Remember that Bstring functions will automatically append the `NULL`
character to the character data buffer. So by simply accessing the data
buffer directly, ordinary C string library functions can be called directly
on them. Note that `bstrcmp` is not the same as `memcmp` in exactly the same
way that `strcmp` is not the same as `memcmp`.

<table>
<tr><th>C-library</th>  <th>Bstring alternative</th>
<tr><td>fread</td>      <td>balloc + fread</td>
<tr><td>fgets</td>      <td>balloc + fgets</td>
</table>

These are odd ones because of the exact sizing of the buffer required. The
Bstring alternatives requires that the buffers are forced to hold at least
the prescribed length, then just use `fread` or `fgets` directly.  However,
typically the automatic memory management of Bstring will make the typical
use of `fgets` and `fread` to read specifically sized strings unnecessary.

Implementation Choices
----------------------

### Overhead

The bstring library has more overhead versus straight char buffers for most
functions. This overhead is essentially just the memory management and string
header allocation. This overhead usually only shows up for small string
manipulations. The performance loss has to be considered in light of the
following:

1. What would be the performance loss of trying to write this management code
   in one's own application?
2. Since the bstring library source code is given, a sufficiently powerful
   modern inlining globally optimizing compiler can remove function call
   overhead.

Since the data type is exposed, a developer can replace any unsatisfactory
function with their own inline implementation. And that is besides the main
point of what the better string library is mainly meant to provide. Any
overhead lost has to be compared against the value of the safe abstraction
for coupling memory management and string functionality.

### Performance of the C Interface

The algorithms used have performance advantages versus the analogous C library
functions. For example:

1. `bfromcstr`/`blk2str`/`bstrcpy` versus `strcpy`/`strdup`

    By using `memmove` instead of `strcpy`, the break condition of the copy
    loop is based on an independent counter (that should be allocated in a
    register) rather than having to check the results of the load. Modern
    out-of-order executing CPUs can parallelize the final branch mis-predict
    penality with the loading of the source string. Some CPUs will also tend
    to have better built-in hardware support for counted memory moves than
    load-compare-store. This is a minor, but non-zero gain.
2. `biseq` versus `strcmp`

    If the strings are unequal in length, `bsiseq` will return in O(1) time.
    If the strings are aliased, or have aliased data buffers, biseq will return
    in O(1) time. strcmp will always be O(k), where k is the length of the
    common prefix or the whole string if they are identical.
3. `slen` versus `strlen`

    `slen` is obviously always O(1), while `strlen` is always O(n) where *n* is
    the length of the string.
4. `bconcat` versus `strcat`

    Both rely on precomputing the length of the destination string argument,
    which will favor the bstring library. On iterated concatenations the
    performance difference can be enormous.
5. `bsreadln` versus `fgets`

    The `bsreadln` function reads large blocks at a time from the given stream,
    then parses out lines from the buffers directly. Some C libraries will
    implement fgets as a loop over single fgetc calls. Testing indicates that
    the bsreadln approach can be several times faster for fast stream devices
    (such as a file that has been entirely cached.)
6. `bsplits`/`bsplitscb` versus `strspn`

    Accelerators for the set of match characters are generated only once.
7. `binstr` versus `strstr`

    The `binstr` implementation unrolls the loops to help reduce loop overhead.
    This will matter if the target string is long and source string is not
    found very early in the target string. With `strstr`, while it is possible
    to unroll the source contents, it is not possible to do so with the
    destination contents in a way that is effective because every destination
    character must be tested against `NULL` before proceeding to the next
    character.
8. `bReverse` versus `strrev`

    The C function must find the end of the string first before swaping
    character pairs.
9. `bstrrchr` versus no comparable C function

    Its not hard to write some C code to search for a character from the end
    going backwards. But there is no way to do this without computing the
    length of the string with `strlen`.

Practical testing indicates that in general Bstrlib is never signifcantly
slower than the C library for common operations, while very often having a
performance advantage that ranges from significant to massive. Even for
functions like `bninchr` versus `strspn` (where, in theory, there is no
advantage for the Bstrlib architecture) the performance of Bstrlib is vastly
superior to most tested C library implementations.

Some of Bstrlib's extra functionality also lead to inevitable performance
advantages over typical C solutions. For example, using the `blk2tbstr` macro,
one can (in O(1) time) generate an internal substring by reference while not
disturbing the original string. If disturbing the original string is not an
option, typically, a comparable `char *` solution would have to make a copy of
the substring to provide similar functionality. Another example is reverse
character set scanning -- the `strspn` functions only scan in a forward
direction which can complicate some parsing algorithms.

Where high performance `char *` based algorithms are available, Bstrlib can
still leverage them by accessing the `data` field on bstrings. So
realistically Bstrlib can never be significantly slower than any standard
`NULL` terminated `char *` based solutions.

### Memory Management

The bstring functions which write and modify bstrings will automatically
reallocate the backing memory for the char buffer whenever it is required to
grow. The algorithm for resizing chosen is to snap up to sizes that are a
power of two which are sufficient to hold the intended new size. Memory
reallocation is not performed when the required size of the buffer is
decreased. This behavior can be relied on, and is necessary to make the
behaviour of balloc deterministic. This trades off additional memory usage
for decreasing the frequency for required reallocations:

1. For any bstring whose size never exceeds n, its buffer is not ever
   reallocated more than `log2(n)` times for its lifetime.
2. For any bstring whose size never exceeds *n*, its buffer is never more than
   `2 * (n + 1)` in length. (The extra characters beyond `2 * n` are to allow
   for the implicit `NULL` which is always added by the bstring modifying
   functions).

Decreasing the buffer size when the string decreases in size would violate 1.
above and in real world case lead to pathological heap thrashing. Similarly,
allocating more tightly than "least power of 2 greater than necessary" would
lead to a violation of 1. and have the same potential for heap thrashing.

Property 2. needs emphasizing. Although the memory allocated is always a
power of 2, for a bstring that grows linearly in size, its buffer memory also
grows linearly, not exponentially. The reason is that the amount of extra
space increases with each reallocation, which decreases the frequency of
future reallocations.

Obviously, given that bstring writing functions may reallocate the data
buffer backing the target bstring, one should not attempt to cache the data
buffer address and use it after such bstring functions have been called.
This includes making reference struct tagbstrings which alias to a writable
bstring.

`balloc` or `bfromcstralloc` can be used to preallocate the minimum amount of
space used for a given bstring. This will reduce even further the number of
times the data portion is reallocated. If the length of the string is never
more than one less than the memory length then there will be no further
reallocations.

Note that invoking the `bwriteallow` macro may increase the number of
`realloc`s by one more than necessary for every call to bwriteallow
interleaved with any bstring API which writes to this bstring.

The library does not use any mechanism for automatic clean up for the C API.
Thus explicit clean up via calls to `bdestroy` are required to avoid memory
leaks.

### Constant and Static `tagbstrings`

A `struct tagbstring` can be write protected from any bstrlib function using
the `bwriteprotect` macro. A write protected `struct tagbstring` can then be
reset to being writable via the `bwriteallow` macro. There is, of course, no
protection from attempts to directly access the bstring members. Modifying a
bstring which is write protected by direct access has undefined behavior.

static `struct tagbstrings` can be declared via the `bsStatic` macro. They are
considered permanently unwritable. Such struct tagbstrings's are declared
such that attempts to write to it are not well defined. Invoking either
`bwriteallow` or `bwriteprotect` on static struct tagbstrings has no effect.

`struct tagbstring`s initialized via `btfromcstr` or `blk2tbstr` are
protected by default but can be made writeable via the bwriteallow macro. If
`bwriteallow` is called on such `struct tagbstring`s, it is the programmer's
responsibility to ensure that:

1. The buffer supplied was allocated from the heap.
2. `bdestroy` is not called on this `tagbstring` unless the header itself has
   also been allocated from the heap.
3. `free` is called on the buffer to reclaim its memory.

`bwriteallow` and `bwriteprotect` can be invoked on ordinary bstrings (they
have to be dereferenced with the `*` operator to get the levels of indirection
correct) to give them write protection.

### Buffer Declaration

The memory buffer is actually declared `unsigned char *` instead of `char *`.
The reason for this is to trigger compiler warnings whenever uncasted `char`
buffers are assigned to the data portion of a bstring. This will draw more
diligent programmers into taking a second look at the code where they
have carelessly left off the typically required cast. (Research from
AT&T/Lucent indicates that additional programmer eyeballs is one of the most
effective mechanisms at ferreting out bugs).

### Function Pointers

The `bgets`, `bread` and `bStream` functions use function pointers to obtain
strings from data streams. The function pointer declarations have been
specifically chosen to be compatible with the `fgetc` and `fread` functions.
While this may seem to be a convoluted way of implementing `fgets` and `fread`
style functionality, it has been specifically designed this way to ensure
that there is no dependency on a single narrowly defined set of device
interfaces, such as just stream I/O. In the embedded world, its quite
possible to have environments where such interfaces may not exist in the
standard C library form. Furthermore, the generalization that this opens up
allows for more sophisticated uses for these functions (performing an `fgets`
like function on a socket, for example.) By using function pointers, it also
allows such abstract stream interfaces to be created using the bstring library
itself while not creating a circular dependency.

### Use of `int` For Sizes

This is just a recognition that 16bit platforms with requirements for strings
that are larger than 64K and 32bit+ platforms with requirements for strings
that are larger than 4GB are pretty marginal. The main focus is for 32bit
platforms, and emerging 64bit platforms with reasonable < 4GB string
requirements. Using ints allows for negative values which has meaning
internally to bstrlib.

### Semantic Consideration

Certain care needs to be taken when copying and aliasing bstrings. A bstring
is essentially a pointer type which points to a multipart abstract data
structure. Thus usage, and lifetime of bstrings have semantics that follow
these considerations. For example:

    bstring a, b;
    struct tagbstring t;

    a = bfromcstr("Hello"); /* Create new bstring and copy "Hello" into it. */
    b = a;                  /* Alias b to the contents of a. */
    t = *a;                 /* Create a current instance pseudo-alias of a. */
    bconcat(a, b);          /* Double a and b, t is now undefined. */
    bdestroy(a);            /* Destroy the contents of both a and b. */

Variables of type bstring are really just references that point to real
bstring objects. The equal operator creates aliases, and the asterisk
dereference operator creates a kind of alias to the current instance (which
is generally not useful for any purpose.)  Using `bstrcpy` is the correct way
of creating duplicate instances. The ampersand operator is useful for
creating aliases to `struct tagbstrings` (remembering that constructed struct
tagbstrings are not writable by default).

Security
--------

Bstrlib does not come with explicit security features outside of its fairly
comprehensive error detection, coupled with its strict semantic support.
That is to say that certain common security problems, such as buffer overrun,
constant overwrite, arbitrary truncation etc, are far less likely to happen
inadvertently. Where it does help, Bstrlib maximizes its advantage by
providing developers a simple adoption path that lets them leave less secure
string mechanisms behind. The library will not leave developers wanting, so
they will be less likely to add new code using a less secure string library
to add functionality that might be missing from Bstrlib.

That said there are a number of security ideas not addressed by Bstrlib:

1. Race condition exploitation (i.e., verifying a string's contents, then
   raising the privilege level and execute it as a shell command as two
   non-atomic steps) is well beyond the scope of what Bstrlib can provide. It
   should be noted that MFC's built-in string mutex actually does not solve
   this problem either -- it just removes immediate data corruption as a
   possible outcome of such exploit attempts (it can be argued that this is
   worse, since it will leave no trace of the exploitation). In general race
   conditions have to be dealt with by careful design and implementation; it
   cannot be assisted by a string library.
2. Any kind of access control or security attributes to prevent usage in
   dangerous interfaces such as `system`. Perl includes a "trust" attribute
   which can be endowed upon strings that are intended to be passed to such
   dangerous interfaces. However, Perl's solution reflects its own limitations
   -- notably that it is not a strongly typed language. In the example code for
   Bstrlib, there is a module called taint.cpp. It demonstrates how to write a
   simple wrapper class for managing "untainted" or trusted strings using the
   type system to prevent questionable mixing of ordinary untrusted strings
   with untainted ones then passing them to dangerous interfaces. In this way
   the security correctness of the code reduces to auditing the direct usages
   of dangerous interfaces or promotions of tainted strings to untainted ones.
3. Encryption of string contents is way beyond the scope of Bstrlib.
   Maintaining encrypted string contents in the futile hopes of thwarting
   things like using system-level debuggers to examine sensitive string data
   is likely to be a wasted effort (imagine a debugger that runs at a higher
   level than a virtual processor where the application runs). For more
   standard encryption usages, since the bstring contents are simply binary
   blocks of data, this should pose no problem for usage with other standard
   encryption libraries.

Compatibility
-------------

The Better String Library is known to compile and function correctly with the
following compilers:

* GNU C/C++
* Microsoft Visual C++
* Watcom C/C++
* Intel's C/C++ compiler (Windows)
* The GNU C/C++ compiler (cygwin and Linux on PPC64)
* Borland C
* Turbo C

Setting of configuration options should be unnecessary for these compilers
(unless exceptions are being disabled or STLport has been added to WATCOM
C/C++). Bstrlib has been developed with an emphasis on portability. As such
porting it to other compilers should be straight forward. This package
includes a porting guide (called porting.txt) which explains what issues may
exist for porting Bstrlib to different compilers and environments.

ANSI issues
-----------

1. The function pointer types `bNgetc` and `bNread` have prototypes which
   are very similar to, but not exactly the same as `fgetc` and `fread`
   respectively.

   Basically the `FILE *` parameter is replaced by `void *`. The purpose of
   this was to allow one to create other functions with `fgetc` and `fread`
   like semantics without being tied to ANSI C's file streaming mechanism,
   i.e., one could very easily adapt it to sockets, or simply reading a block
   of memory, or procedurally generated strings (for fractal generation, for
   example.)

   The problem is that invoking the functions `bNgetc`, `fgetc`, `bNread`,
   and `fread` is not technically legal in ANSI C. The reason being that the
   compiler is only able to coerce the function pointers themselves into the
   target type, however are unable to perform any cast (implicit or otherwise)
   on the parameters passed once invoked, i.e., if internally `void *` and
   `FILE *` need some kind of mechanical coercion, the compiler will not
   properly perform this conversion and thus lead to undefined behavior.

   Apparently a platform from Data General called "Eclipse" and another from
   Tandem called "NonStop" have a different representation for pointers to
   bytes and pointers to words, for example, where coercion via casting is
   necessary. (Actual confirmation of the existence of such machines is hard
   to come by, so it is prudent to be skeptical about this information).
   However, this is not an issue for any known contemporary platforms. One may
   conclude that such platforms are effectively apocryphal even if they do
   exist.

   To correctly work around this problem to the satisfaction of the ANSI
   limitations, one needs to create wrapper functions for fgets and/or fread
   with the prototypes of `bNgetc` and/or `bNread` respectively which performs
   no other action other than to explicitely cast the `void *` parameter to a
   `FILE *`, and simply pass the remaining parameters straight to the function
   pointer call.

   The wrappers themselves are trivial:

       size_t freadWrap(void *buff, size_t esz, size_t eqty, void *parm) {
           return fread(buff, esz, eqty, (FILE *) parm);
       }

       int fgetcWrap(void *parm) {
           return fgetc((FILE *)parm);
       }

   These have not been supplied in bstrlib or bstraux to prevent unnecessary
   linking with file I/O functions.
2. The `bstrlib` function names are not unique in the first 6 characters.

   This is only an issue for older C compiler environments which do not
   store more than 6 characters for function names.

Examples
--------

Dumping a line numbered file:

    FILE *fp;
    int i, ret;
    struct bstrList * lines;
    struct tagbstring prefix = bsStatic("-> ");

    if (NULL != (fp = fopen("bstrlib.txt", "rb"))) {
        bstring b = bread((bNread) fread, fp);
        fclose (fp);
        if (NULL != (lines = bsplit(b, '\n'))) {
            for (i=0; i < lines->qty; i++) {
                binsert(lines->entry[i], 0, &prefix, '?');
                printf("%04d: %s\n", i, bdatae(lines->entry[i], "NULL"));
            }
            bstrListDestroy(lines);
        }
        bdestroy(b);
    }

For numerous other examples, see bstraux.c, bstraux.h and the example archive.

License
-------

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of bstrlib nor the names of its contributors may be
   used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

Alternatively, the contents of this file may be used under the terms of
GNU General Public License Version 2 (the "GPL").

Acknowledgements
----------------

The following individuals have made significant contributions to the design
and testing of the Better String Library:

* Bjorn Augestad
* Clint Olsen
* Darryl Bleau
* Fabian Cenedese
* Graham Wideman
* Ignacio Burgueno
* International Business Machines Corporation
* Ira Mica
* John Kortink
* Manuel Woelker
* Marcel van Kervinck
* Michael Hsieh
* Richard A. Smith
* Simon Ekstrom
* Wayne Scott
