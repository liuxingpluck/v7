/*
 * Copyright (c) 2013-2014 Cesanta Software Limited
 * All rights reserved
 *
 * This software is dual-licensed: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation. For the terms of this
 * license, see <http: *www.gnu.org/licenses/>.
 *
 * You are free to use this software under the terms of the GNU General
 * Public License, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * Alternatively, you can license this software under a commercial
 * license, as set out in <http://cesanta.com/products.html>.
 */

/*
 * === C/C++ API
 *
 * V7 uses 64-bit `v7_val_t` type to store JavaScript values. There are
 * several families of functions V7 provides:
 *
 * - `v7_exec_*()` execute a piece of JavaScript code, put result in `v7_val_t`
 * - `v7_create_*()` convert C/C++ values into JavaScript `v7_val_t` values
 * - `v7_to_*()` convert JavaScript `v7_val_t` values into C/C++ values
 * - `v7_is_*()` test whether JavaScript `v7_val_t` value is of given type
 * - misc functions that throw exceptions, operate on arrays & objects,
 *   call JS functions, etc
 *
 * NOTE: V7 instance is single threaded. It does not protect
 * it's data structures by mutexes. If V7 instance is shared between several
 * threads, a care should be taken to serialize accesses.
 */

#ifndef V7_HEADER_INCLUDED
#define V7_HEADER_INCLUDED

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stddef.h> /* For size_t */
#include <stdio.h>  /* For FILE */

#define V7_VERSION "1.0"

#if (defined(_WIN32) && !defined(__MINGW32__) && !defined(__MINGW64__)) || \
    (defined(_MSC_VER) && _MSC_VER <= 1200)
#define V7_WINDOWS
#endif

#ifdef V7_WINDOWS
typedef unsigned __int64 uint64_t;
#else
#include <inttypes.h>
#endif
typedef uint64_t v7_val_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct v7; /* Opaque structure. V7 engine handler. */
typedef v7_val_t (*v7_cfunction_t)(struct v7 *, v7_val_t, v7_val_t);

/* Create V7 instance */
struct v7 *v7_create(void);

/* Destroy V7 instance */
void v7_destroy(struct v7 *);

enum v7_err { V7_OK, V7_SYNTAX_ERROR, V7_EXEC_EXCEPTION };

/*
 * Execute JavaScript `js_code`, store result in `result` variable.
 * Return:
 *
 *  - V7_OK on success. `result` contains the result of execution.
 *  - V7_SYNTAX_ERROR if `js_code` in not a valid code. `result` is undefined.
 *  - V7_EXEC_EXCEPTION if `js_code` threw an exception. `result` stores
 *    an exception object.
 */
enum v7_err v7_exec(struct v7 *, v7_val_t *result, const char *js_code);

/*
 * Same as `v7_exec()`, but loads source code from `path` file.
 */
enum v7_err v7_exec_file(struct v7 *, v7_val_t *result, const char *path);

/*
 * Same as `v7_exec()`, but passes `this_obj` as `this` to the execution
 * context.
 */
enum v7_err v7_exec_with(struct v7 *, v7_val_t *result, const char *js_code,
                         v7_val_t this_obj);

/*
 * Compile JavaScript code `js_code` into the byte code and write generated
 * byte code into opened file stream `fp`. If `generate_binary_output` is 0,
 * then generated byte code is in human-readable text format. Otherwise, it is
 * in the binary format, suitable for execution by V7 instance.
 * NOTE: `fp` must be a valid, opened, writable file stream.
 */
void v7_compile(const char *js_code, int generate_binary_output, FILE *fp);

/* Call garbage collector */
void v7_gc(struct v7 *);

/* Create an empty object */
v7_val_t v7_create_object(struct v7 *v7);

/* Create an empty array object */
v7_val_t v7_create_array(struct v7 *v7);

/*
 * Create function object. `func` is a C callback, `nargs` is a number of
 * arguments
 */
v7_val_t v7_create_function(struct v7 *, v7_cfunction_t func, int nargs);

/* Create JavaScript value that holds C/C++ callback pointer. */
v7_val_t v7_create_cfunction(v7_cfunction_t func);

/* Create numeric primitive value */
v7_val_t v7_create_number(double num);

/* Create boolean primitive value (either `true` or `false`) */
v7_val_t v7_create_boolean(int is_true);

/* Create `null` primitive value */
v7_val_t v7_create_null(void);

/* Create `undefined` primitive value */
v7_val_t v7_create_undefined(void);

/*
 * Create string primitive value.
 * `str` must point to the utf8 string of length `len`.
 */
v7_val_t v7_create_string(struct v7 *, const char *str, size_t len, int copy);

/*
 * Create RegExp object.
 * `regex`, `regex_len` specify a pattern, `flags` and `flags_len` specify
 * flags. Both utf8 encoded. For example, `regex` is `(.+)`, `flags` is `gi`.
 */
v7_val_t v7_create_regexp(struct v7 *, const char *regex, size_t regex_len,
                          const char *flags, size_t flags_len);

/*
 * Create JavaScript value that holds C/C++ `void *` pointer.
 */
v7_val_t v7_create_foreign(void *ptr);

/* Return true if given value is a JavaScript object */
int v7_is_object(v7_val_t);

/* Return true if given value is a JavaScript function object */
int v7_is_function(v7_val_t);

/* Return true if given value is a primitive string value */
int v7_is_string(v7_val_t);

/* Return true if given value is a primitive boolean value */
int v7_is_boolean(v7_val_t);

/* Return true if given value is a primitive number value */
int v7_is_double(v7_val_t);

/* Return true if given value is a primitive `null` value */
int v7_is_null(v7_val_t);

/* Return true if given value is a primitive `undefined` value */
int v7_is_undefined(v7_val_t);

/* Return true if given value is a JavaScript RegExp object*/
int v7_is_regexp(v7_val_t);

/* Return true if given value holds C callback */
int v7_is_cfunction(v7_val_t);

/* Return true if given value holds `void *` pointer */
int v7_is_foreign(v7_val_t);

/* Return `void *` pointer stored in `v7_val_t` */
void *v7_to_foreign(v7_val_t);

/* Return boolean stored in `v7_val_t`: 0 for `false`, non-0 for `true` */
int v7_to_boolean(v7_val_t);

/* Return `double` value stored in `v7_val_t` */
double v7_to_double(v7_val_t);

/* Return `v7_cfunction_t` callback pointer stored in `v7_val_t` */
v7_cfunction_t v7_to_cfunction(v7_val_t);

/*
 * Return pointer to string stored in `v7_val_t`.
 * String length returned in `string_len`. Returned string pointer is
 * guaranteed to be 0-terminated, suitable for standard C library string API.
 *
 * CAUTION: creating new JavaScript object, array, or string may kick in a
 * garbage collector, which in turn may relocate string data and invalidate
 * pointer returned by `v7_to_string()`.
 */
const char *v7_to_string(struct v7 *, v7_val_t *value, size_t *string_len);

/* Return root level (`global`) object of the given V7 instance. */
v7_val_t v7_get_global_object(struct v7 *);

/*
 * Lookup property `name`, `len` in object `obj`. If `obj` holds no such
 * property, an `undefined` value is returned.
 */
v7_val_t v7_get(struct v7 *v7, v7_val_t obj, const char *name, size_t len);

/*
 * Generate JSON representation of the JavaScript value `val` into a buffer
 * `buf`, `buf_len`. If `buf_len` is too small to hold generated JSON string,
 * `v7_to_json()` allocates required memory. In that case, it is caller's
 * responsibility to free the allocated buffer. Generated JSON string is
 * guaranteed to be 0-terminated.
 *
 * Example code:
 *
 *     char buf[100], *p;
 *     p = v7_to_json(v7, obj, buf, sizeof(buf));
 *     printf("JSON string: [%s]\n", p);
 *     if (p != buf) {
 *       free(p);
 *     }
 */
char *v7_to_json(struct v7 *, v7_val_t val, char *buf, size_t buf_len);

/* Return true if given value is `true`, as in JavaScript `if (v)` statement */
int v7_is_true(struct v7 *v7, v7_val_t v);

/*
 * Call function `func` with arguments `args`, using `this_obj` as `this`.
 * `args` could be either undefined value, or be an array with arguments.
 */
v7_val_t v7_apply(struct v7 *, v7_val_t func, v7_val_t this_obj, v7_val_t args);

/* Throw an exception (Error object) with given formatted message. */
void v7_throw(struct v7 *, const char *msg_fmt, ...);

#define V7_PROPERTY_READ_ONLY 1
#define V7_PROPERTY_DONT_ENUM 2
#define V7_PROPERTY_DONT_DELETE 4
#define V7_PROPERTY_HIDDEN 8
#define V7_PROPERTY_GETTER 16
#define V7_PROPERTY_SETTER 32

/*
 * Set object property. `name`, `name_len` specify property name, `attrs`
 * specify property attributes, `val` is a property value. Return non-zero
 * on success, 0 on error (e.g. out of memory).
 */
int v7_set(struct v7 *v7, v7_val_t obj, const char *name, size_t name_len,
           unsigned int attrs, v7_val_t val);
#define v7_set_method(v7, obj, name, func)                         \
  v7_set((v7), (obj), (name), strlen(name), V7_PROPERTY_DONT_ENUM, \
         v7_create_function((v7), (func), -1))

/* Return array length */
unsigned long v7_array_length(struct v7 *v7, v7_val_t arr);

/* Insert value `v` in array `arr` at index `index`. */
int v7_array_set(struct v7 *v7, v7_val_t arr, unsigned long index, v7_val_t v);

/* Insert value `v` in array `arr` at the end of the array. */
int v7_array_push(struct v7 *, v7_val_t arr, v7_val_t v);

/*
 * Return array member at index `index`. If `index` is out of bounds, undefined
 * is returned.
 */
v7_val_t v7_array_get(struct v7 *, v7_val_t arr, unsigned long index);

int v7_main(int argc, char *argv[], void (*init_func)(struct v7 *));

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* V7_HEADER_INCLUDED */
