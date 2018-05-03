/*
MIT License

Copyright (c) 2018 Nicolas Manichon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/**
 * \file memutils.h
 * \brief Wrappers around memory allocators
 */

#ifndef MEMUTILS_H
#define MEMUTILS_H

#include <stdlib.h>

#define xmalloc(size) _xmalloc(size, __FILE__, __LINE__);
#define xcalloc(nmemb, size) _xcalloc(nmemb, size, __FILE__, __LINE__);
#define xrealloc(ptr, size) _xrealloc(ptr, size, __FILE__, __LINE__);

void *_xmalloc(size_t size, char *file, int line);
void *_xcalloc(size_t nmemb, size_t size, char *file, int line);
void *_xrealloc(void *ptr, size_t size, char *file, int line);

#endif
