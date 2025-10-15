Better String Library Comparisons
=================================

by Paul Hsieh

The bstring library is an attempt to provide improved string processing
functionality to the C and C++ language. At the heart of the bstring library
is the management of `bstring`s which are a significant improvement over
`'\0'` terminated char buffers. See the accompanying documenation for more
information.

Comparison With Microsoft's CString Class
-----------------------------------------

The bstring library has significant advantages over CString:

1. Bstrlib is a C-library as well as a C++ library (using the C++ wrapper)

   Thus it is compatible with more programming environments and available to
   a wider population of programmers.
2. The internal structure of a bstring is considered exposed

   A single contiguous block of data can be cut into read-only pieces by
   simply creating headers, without allocating additional memory to create
   reference copies of each of these sub-strings.

   In this way, using bstrings in a totally abstracted way becomes a choice
   rather than an imposition. Further this choice can be made differently
   at different layers of applications that use it.
3. Static declaration support precludes the need for constructor invocation

   Allows for static declarations of constant strings that has no additional
   constructor overhead.
4. Bstrlib is not attached to another library

   Bstrlib is designed to be easily plugged into any other library collection,
   without dependencies on other libraries or paradigms (such as "MFC".)

The bstring library also comes with a few additional functions that are not
available in the CString class:

* `bsetstr`
* `bsplit`
* `bread`
* `breplace` (this is different from `CString::Replace()`)
* Writable indexed characters (for example `a[i]='x'`)

Interestingly, although Microsoft did implement `mid`, `left` and `right`
functional analogues (these are functions from GWBASIC) they seem to have
forgotten that `mid` could be also used to write into the middle of a string.
This functionality exists in Bstrlib with the `bsetstr` and `breplace`
functions.

Among the disadvantages of Bstrlib is that there is no special support for
localization or wide characters. Such things are considered beyond the scope
of what bstrings are trying to deliver. CString essentially supports the
older UCS-2 version of Unicode via `widechar_t` as an application-wide compile
time switch.

CString's also use built-in mechanisms for ensuring thread safety under all
situations. While this makes writing thread safe code that much easier, this
built-in safety feature has a price -- the inner loops of each CString method
runs in its own critical section (grabbing and releasing a light weight mutex
on every operation.)  The usual way to decrease the impact of a critical
section performance penalty is to amortize more operations per critical
section. But since the implementation of CStrings is fixed as a one critical
section per-operation cost, there is no way to leverage this common
performance enhancing idea.

The search facilities in Bstrlib are comparable to those in MFC's CString
class, though it is missing locale specific collation. But because Bstrlib
is interoperable with C's char buffers, it will allow programmers to write
their own string searching mechanism (such as Boyer-Moore), or be able to
choose from a variety of available existing string searching libraries (such
as those for regular expressions) without difficulty.

Microsoft used a very non-ANSI conforming trick in its implementation to
allow `printf` to use the `%s` specifier to output a `CString` correctly. This
can be convenient, but it is inherently not portable. Microsoft's own
documentation recommends casting, instead of relying on this feature.

Comparison With C++'s `std::string`
-----------------------------------

This is the C++ language's STL based string class.

1. There is no C implementation.
2. The `[]` operator is not bounds checked.
3. Missing a lot of useful functions like `printf`-like formatting.
4. Some sub-standard `std::string` implementations (SGI) are necessarily
   unsafe to use with multithreading.
5. Limited by STL's std::iostream which in turn is limited by ifstream which
   can only take input from files. (Compare to CBStream's API which can take
   abstracted input.)
6. Extremely uneven performance across implementations.

Comparison With ISO C TR 24731 Proposal
---------------------------------------

Following the ISO C99 standard, Microsoft has proposed a group of C library
extensions which are supposedly "safer and more secure". This proposal is
expected to be adopted by the ISO C standard which follows C99.

The proposal reveals itself to be very similar to Microsoft's "StrSafe"
library. The functions are basically the same as other standard C library
string functions except that destination parameters are paired with an
additional length parameter of type `rsize_t`. `rsize_t` is the same as
`size_t`, however, the range is checked to make sure its between `1` and
`RSIZE_MAX`. Like Bstrlib, the functions perform a "parameter check". Unlike
Bstrlib, when a parameter check fails, rather than simply outputing
accumulatable error statuses, they call a user settable global error function
handler, and upon return of control performs no (additional) detrimental
action. The proposal covers basic string functions as well as a
few non-reenterable functions (`asctime`, `ctime`, and `strtok`).

1. Still based solely on `char *` buffers (and therefore `strlen` and `strcat`
   is still O(n), and there are no faster `streq` comparison functions.)
2. No growable string semantics.
3. Requires manual buffer length synchronization in the source code.
4. No attempt to enhance functionality of the C library.
5. Introduces a new error scenario (strings exceeding `RSIZE_MAX` length).

The hope is that by exposing the buffer length requirements there will be
fewer buffer overrun errors. However, the error modes are really just
transformed, rather than removed. The real problem of buffer overflows is
that they all happen as a result of erroneous programming. So forcing
programmers to manually deal with buffer limits, will make them more aware of
the problem but doesn't remove the possibility of erroneous programming. So
a programmer that erroneously mixes up the `rsize_t` parameters is no better
off from a programmer that introduces potential buffer overflows through other
more typical lapses. So at best this may reduce the rate of erroneous
programming, rather than making any attempt at removing failure modes.

The error handler can discriminate between types of failures, but does not
take into account any callsite context. So the problem is that the error is
going to be manifest in a piece of code, but there is no pointer to that
code. It would seem that passing in the call site `__FILE__`, `__LINE__` as
parameters would be very useful, but the API clearly doesn't support such a
thing (it would increase code bloat even more than the extra length
parameter does, and would require macro tricks to implement).

The Bstrlib C API takes the position that error handling needs to be done at
the callsite, and just tries to make it as painless as possible. Furthermore,
error modes are removed by supporting auto-growing strings and aliasing. For
capturing errors in more central code fragments, Bstrlib's C++ API uses
exception handling extensively, which is superior to the leaf-only error
handler approach.

Comparison With Managed String Library CERT Proposal
----------------------------------------------------

Robert Seacord at CERT has [proposed a C string library](http://www.cert.org/secure-coding/managedstring.html)
that he calls the "Managed String Library" for C. Like Bstrlib, it introduces
a new type which is called a managed string. The structure of a managed string
(`string_m`) is like a `struct tagbstring` but missing the length field. This
internal structure is considered opaque. The length is, like the C standard
library, always computed on the fly by searching for a terminating NUL on
every operation that requires it. So it suffers from every performance
problem that the C standard library suffers from. Interoperating with C
string APIs (like `printf`, `fopen`, or anything else that takes a string
parameter) requires copying to additionally allocating buffers that have to
be manually freed -- this makes this library probably slower and more
cumbersome than any other string library in existence.

The library gives a fully populated error status as the return value of every
string function. The hope is to be able to diagnose all problems
specifically from the return code alone. Comparing this to Bstrlib, which
aways returns one consistent error message, might make it seem that Bstrlib
would be harder to debug; but this is not true. With Bstrlib, if an error
occurs there is always enough information from just knowing there was an error
and examining the parameters to deduce exactly what kind of error has
happened. The managed string library thus gives up nested function calls
while achieving little benefit, while Bstrlib does not.

One interesting feature that "managed strings" has is the idea of data
sanitization via character set whitelisting. That is to say, a globally
definable filter that makes any attempt to put invalid characters into strings
lead to an error and not modify the string. The author gives the following
example:

    /* create valid char set */
    if (retValue = strcreate_m(&str1, "abc") ) {
      fprintf(
        stderr,
        "Error %d from strcreate_m.\n",
        retValue
      );
    }
    if (retValue = setcharset(str1)) {
      fprintf(
        stderr,
        "Error %d from  setcharset().\n",
        retValue
      );
    }
    if (retValue = strcreate_m(&str1, "aabbccabc")) {
      fprintf(
        stderr,
        "Error %d from strcreate_m.\n",
        retValue
      );
    }
    /* create string with invalid char set */
    if (retValue = strcreate_m(&str1, "abbccdabc")) {
      fprintf(
        stderr,
        "Error %d from strcreate_m.\n",
        retValue
      );
    }

Which we can compare with a more Bstrlib way of doing things:

    bstring bCreateWithFilter (const char * cstr, const bstring filter) {
      bstring b = bfromcstr (cstr);
      if (BSTR_ERR != bninchr (b, filter) && NULL != b) {
        fprintf (stderr, "Filter violation.\n");
        bdestroy (b);
        b = NULL;
      }
      return b;
    }

    struct tagbstring charFilter = bsStatic ("abc");
    bstring str1 = bCreateWithFilter ("aabbccabc", &charFilter);
    bstring str2 = bCreateWithFilter ("aabbccdabc", &charFilter);

The first thing we should notice is that with the Bstrlib approach you can
have different filters for different strings if necessary. Furthermore,
selecting a charset filter in the Managed String Library is uni-contextual.
That is to say, there can only be one such filter active for the entire
program, which means its usage is not well defined for intermediate library
usage (a library that uses it will interfere with user code that uses it, and
vice versa). It is also likely to be poorly defined in multi-threading
environments.

There is also a question as to whether the data sanitization filter is checked
on every operation, or just on creation operations. Since the charset can be
set arbitrarily at run time, it might be set *after* some managed strings have
been created. This would seem to imply that all functions should run this
additional check every time if there is an attempt to enforce this. This
would make things tremendously slow. On the other hand, if it is assumed that
only creates and other operations that take `char *`s as input need be checked
because the charset was only supposed to be called once at and before any
other managed string was created, then one can see that its easy to cover
Bstrlib with equivalent functionality via a few wrapper calls such as the
example given above.

And finally we have to question the value of sanitation in the first place.
For example, for httpd servers, there is generally a requirement that the
URLs parsed have some form that avoids undesirable translation to local file
system filenames or resources. The problem is that the way URLs can be
encoded, it must be completely parsed and translated to know if it is using
certain invalid character combinations. That is to say, merely filtering
each character one at a time is not necessarily the right way to ensure that
a string has safe contents.

In the article that describes this proposal, it is claimed that it fairly
closely approximates the existing C API semantics. On this point we should
compare this "closeness" with Bstrlib:

<table>
<tr><th></th>                   <th>Bstrlib</th>                <th>Managed String Library</th></tr>
<tr><td>Pointer arithmetic</td> <td>Segment arithmetic</td>     <td>N/A</td></tr>
<tr><td>Use in C Std lib</td>   <td>data, or bdata{e}</td>      <td>getstr_m(x,*) ... free(x)</td></tr>
<tr><td>String literals</td>    <td>bsStatic, bsStaticBlk</td>  <td>strcreate_m()</td></tr>
<tr><td>Transparency</td>       <td>Complete</td>               <td>None</td></tr>
</table>

It's pretty clear that the semantic mapping from C strings to Bstrlib is
fairly straightforward, and that in general semantic capabilities are the
same or superior in Bstrlib. On the other hand the Managed String Library
is either missing semantics or changes things fairly significantly.

Comparison With Annexia's c2lib Library
---------------------------------------

This library is [available online](http://www.annexia.org/freeware/c2lib).

1. Still based solely on `char *` buffers (and therefore `strlen` and `strcat`
   is still O(n), and there are no faster `streq` comparison functions.)
   Their suggestion that alternatives which wrap the string data type (such as
   bstring does) imposes a difficulty in interoperating with the C langauge's
   ordinary C string library is not founded.
2. Introduction of memory (and vector?) abstractions imposes a learning
   curve, and some kind of memory usage policy that is outside of the strings
   themselves (and therefore must be maintained by the developer.)
3. The API is massive, and filled with all sorts of trivial (`pjoin`) and
   controvertial (`pmatch` -- regular expression are not sufficiently
   standardized, and there is a very large difference in performance between
   compiled and non-compiled, REs) functions. Bstrlib takes a decidely
   minimal approach -- none of the functionality in c2lib is difficult or
   challenging to implement on top of Bstrlib (except the regex stuff, which
   is going to be difficult, and controvertial no matter what.)
4. Understanding why c2lib is the way it is pretty much requires a working
   knowledge of Perl. bstrlib requires only knowledge of the C string library
   while providing just a very select few worthwhile extras.
5. It is attached to a lot of cruft like a matrix math library (that doesn't
   include any functions for getting the determinant, eigenvectors,
   eigenvalues, the matrix inverse, test for singularity, test for
   orthogonality, a grahm schmit orthogonlization, LU decomposition... I
   mean why bother?)

Convincing a development house to use c2lib is likely quite difficult. It
introduces too much, while not being part of any kind of standards body. The
code must therefore be trusted, or maintained by those that use it. While
bstring offers nothing more on this front, since its so much smaller, covers
far less in terms of scope, and will typically improve string performance,
the barrier to usage should be much smaller.

Comparison With stralloc/qmail
------------------------------

More information about this library can be [found](http://www.canonical.org/~kragen/stralloc.html) [online](http://cr.yp.to/lib/stralloc.html).

1. Library is very very minimal. A little too minimal.
2. Untargetted source parameters are not declared const.
3. Slightly different expected emphasis (like `_cats` function which takes an
   ordinary C string char buffer as a parameter). It's clear that the
   remainder of the C string library is still required to perform more
   useful string operations.

The `struct` declaration for their string header is essentially the same as
that for bstring. But it's clear that this was a quickly written hack whose
goals are clearly a subset of what Bstrlib supplies. For anyone who is served
by `stralloc`, Bstrlib is complete substitute that just adds more
functionality.

`stralloc` actually uses the interesting policy that a NULL data pointer
indicates an empty string. In this way, non-static empty strings can be
declared without construction. This advantage is minimal, since static empty
bstrings can be declared inline without construction, and if the string needs
to be written to it should be constructed from an empty string (or its first
initializer) in any event.

wxString Class
--------------

This is the string class used in the [wxWindows project](http://www.wxwindows.org/manuals/2.4.2/wx368.htm#wxstring).

1. There is no C implementation.
2. The memory management strategy is to allocate a bounded fixed amount of
   additional space on each resize, meaning that it does not have the
   `log2(n)` property that Bstrlib has (it will thrash very easily, cause
   massive fragmentation in common heap implementations, and can easily be a
   common source of performance problems).
3. The library uses a "copy on write" strategy, meaning that it has to deal
   with multithreading problems.

Vstr
----

This is a highly orthogonal C string library with an emphasis on
networking/realtime programming. It can be found [online](http://www.and.org/vstr/).

1. The convoluted internal structure does not contain a `'\0'` `char *`
   compatible buffer, so interoperability with the C library a non-starter.
2. The API and implementation is very large (owing to its orthogonality) and
   can lead to difficulty in understanding its exact functionality.
3. An obvious dependency on gnu tools (confusing make configure step)
4. Uses a reference counting system, meaning that it is not likely to be
   thread safe.

The implementation has an extreme emphasis on performance for nontrivial
actions (adds, inserts and deletes are all constant or roughly O(\#operations)
time) following the "zero copy" principle. This trades off performance of
trivial functions (character access, char buffer access/coersion, alias
detection) which becomes significantly slower, as well as incremental
accumulative costs for its searching/parsing functions. Whether or not Vstr
wins any particular performance benchmark will depend a lot on the benchmark,
but it should handily win on some, while losing dreadfully on others.

The learning curve for Vstr is very steep, and it doesn't come with any
obvious way to build for Windows or other platforms without gnu tools. At
least one mechanism (the iterator) introduces a new undefined scenario
(writing to a Vstr while iterating through it.)  Vstr has a very large
footprint, and is very ambitious in its total functionality. Vstr has no C++
API.

Vstr usage requires context initialization via `vstr_init` which must be run
in a thread-local context. Given the totally reference based architecture
this means that sharing Vstrings across threads is not well defined, or at
least not safe from race conditions. This API is clearly geared to the older
standard of `fork` style multitasking in UNIX, and is not safely transportable
to modern shared memory multithreading available in Linux and Windows. There
is no portable external solution making the library thread safe (since it
requires a mutex around each Vstr context -- not each string.)

In the documentation for this library, a big deal is made of its self hosted
`sprintf`-like function. This is an issue for older compilers that don't
include `vsnprintf`, but also an issue because Vstr has a slow conversion to
`'\0'` terminated `char *` mechanism. That is to say, using `%s` to format
data that originates from Vstr would be slow without some sort of native
function to do so. Bstrlib sidesteps the issue by relying on what
`snprintf`-like functionality does exist and having a high performance
conversion to a `char *` compatible string so that `%s` can be used directly.

Str Library
-----------

This is a fairly [extensive string library](http://www.utilitycode.com/str/default.aspx),
that includes full unicode support and targetted at the goal of out performing
MFC and STL. The architecture, similarly to MFC's CStrings, is a copy on write
reference counting mechanism.

1. Commercial
2. C++ only

This library, like Vstr, uses a reference counting system. There is only so
deeply I can analyze it, since I don't have a license for it. However,
performance improvements over MFC's and STL, doesn't seem like a sufficient
reason to move your source base to it. For example, in the future, Microsoft
may improve the performance CString.

It should be pointed out that performance testing of Bstrlib has indicated
that its relative performance advantage versus MFC's CString and STL's
`std::string` is at least as high as that for the Str library.

libmib astrings
---------------

This library provides [handful of functional extensions](http://www.mibsoftware.com/libmib/astring/)
to the C library that add dynamic string functionality.

This package basically references strings through `char **` pointers and
assumes they are pointing to the top of an allocated heap entry (or `NULL`, in
which case memory will be newly allocated from the heap). It is still up to
user to mix and match the older C string functions with these functions
whenever pointer arithmetic is used. There is no leveraging of the type
system to assert semantic differences between references and base strings as
Bstrlib does since no new types are introduced. Unlike Bstrlib, exact string
length meta data is not stored, thus requiring a `strlen` call on *every*
string writing operation. The library is very small, covering only a handful
of C's functions.

While this is better than nothing, it is clearly slower than even the
standard C library, less safe and less functional than Bstrlib.

To explain the advantage of using libmib, their website shows an example of
how dangerous C code:

    char buf[256];
    char *pszExtraPath = ";/usr/local/bin";

    strcpy(buf,getenv("PATH")); /* oops! could overrun! */
    strcat(buf,pszExtraPath); /* Could overrun as well! */

    printf("Checking...%s\n",buf); /* Some printfs overrun too! */

is avoided using libmib:

    char *pasz = 0;      /* Must initialize to 0 */
    char *paszOut = 0;
    char *pszExtraPath = ";/usr/local/bin";

    if (!astrcpy(&pasz,getenv("PATH"))) /* malloc error */ exit(-1);
    if (!astrcat(&pasz,pszExtraPath)) /* malloc error */ exit(-1);

    /* Finally, a "limitless" printf! we can use */
    asprintf(&paszOut,"Checking...%s\n",pasz);fputs(paszOut,stdout);

    astrfree(&pasz); /* Can use free(pasz) also. */
    astrfree(&paszOut);

However, compare this to Bstrlib:

    bstring b, out;

    bcatcstr (b = bfromcstr (getenv ("PATH")), ";/usr/local/bin");
    out = bformat ("Checking...%s\n", bdatae (b, "<Out of memory>"));
    /* if (out && b) */ fputs (bdatae (out, "<Out of memory>"), stdout);
    bdestroy (b);
    bdestroy (out);

Besides being shorter, we can see that error handling can be deferred right
to the very end. Also, unlike the above two versions, if `getenv` returns
with `NULL`, the Bstrlib version will not exhibit undefined behavior.
Initialization starts with the relevant content rather than an extra
autoinitialization step.

libclc
------

[Libclc](http://libclc.sourceforge.net/) attempt to add to the standard C
library with a number of common useful functions, including additional string
functions.

1. Uses standard `char *` buffer, and adopts C 99's usage of `restrict` to
   pass the responsibility to guard against aliasing to the programmer.
2. Adds no safety or memory management whatsoever.
3. Most of the supplied string functions are completely trivial.

The goals of libclc and Bstrlib are clearly quite different.

fireString
----------

More information is [available online](http://firestuff.org/).

1. Uses standard `char *` buffer, and adopts C 99's usage of `restrict` to
   pass the responsibility to guard against aliasing to the programmer.
2. Mixes `char *` and length wrapped buffers (estr) functions, doubling the
   API size, with safety limited to only half of the functions.

Firestring was originally just a wrapper of `char *` functionality with extra
length parameters. However, it has been augmented with the inclusion of the
estr type which has similar functionality to `stralloc`. But fireString does
not nearly cover the functional scope of Bstrlib.

Safe C String Library
---------------------

A [library](http://www.zork.org/safestr/safestr.html) written for the purpose
of increasing safety and power to C's string handling capabilities.

1. While the `safestr_*` functions are safe in of themselves, interoperating
   with `char *` string has dangerous unsafe modes of operation.
2. The architecture of safestr's causes the base pointer to change. Thus,
   its not practical/safe to store a safestr in multiple locations if any
   single instance can be manipulated.
3. Dependent on an additional error handling library.
4. Uses reference counting, meaning that it is either not thread safe or
   slow and not portable.

I think the idea of reallocating (and hence potentially changing) the base
pointer is a serious design flaw that is fatal to this architecture. True
safety is obtained by having automatic handling of all common scenarios
without creating implicit constraints on the user.

Because of its automatic temporary clean up system, it cannot use "const"
semantics on input arguments. Interesting anomolies such as:

    safestr_t s, t;
    s = safestr_replace (t = SAFESTR_TEMP ("This is a test"),
                         SAFESTR_TEMP (" "), SAFESTR_TEMP ("."));
    /* t is now undefined. */

are possible. If one defines a function which takes a `safestr_t` as a
parameter, then the function would not know whether or not the `safestr_t` is
defined after it passes it to a safestr library function. The author
recommended method for working around this problem is to examine the
attributes of the `safestr_t` within the function which is to modify any of
its parameters and play games with its reference count. I think, therefore,
that the whole `SAFESTR_TEMP` idea is also fatally broken.

The library implements immutability, optional non-resizability, and a "trust"
flag. This trust flag is interesting, and suggests that applying any
arbitrary sequence of `safestr_*` function calls on any set of trusted strings
will result in a trusted string. It seems to me, however, that if one wanted
to implement a trusted string semantic, one might do so by actually creating
a different *type* and only implement the subset of string functions that are
deemed safe (i.e., user input would be excluded, for example). This, in
essence, would allow the compiler to enforce trust propogation at compile
time rather than run time. Non-resizability is also interesting, however,
it seems marginal, i.e., to want a string that cannot be resized, yet can be
modified and yet where a fixed sized buffer is undesirable.

Libsrt
------

This is a length based string library based on a slightly different strategy.
The string contents are appended to the end of the header directly so strings
only require a single allocation.  However, whenever a reallocation occurs,
the header is replicated and the base pointer for the string is changed.
That means references to the string are only valid so long as they are not
resized after any such reference is cached.  The internal structure maintains
a lot some state used to accelerate unicode manipulation.  This state is
dynamically updated according to usage (so, like Bstrlib, it supports both
a binary mode and a Unicode mode basically all the time).  But this makes
sustainable usage of the library essentially opaque.  This also creates a
bottleneck for whatever extensions to the library one desires (write all
extensions on top of the base library, put in a request to the author, or
dedicate an expert to learn the internals of the library).

SDS
---

Sds uses a strategy very similar to Libsrt.  However, it uses some dynamic
headers to decrease the overhead for very small strings.  This requires an
extra switch statement for access to each string attribute.  The source code
appears to use gcc/clang extensions, and thus it is not portable.
