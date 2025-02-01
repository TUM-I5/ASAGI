/**
 * SPDX-License-Identifier: LGPL-3.0-or-later
 *
 * SPDX-FileCopyrightText: 2001 Andrei Alexandrescu
 * SPDX-FileCopyrightText: 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
 */

// These parts are taken from
// http://loki-lib.sourceforge.net/html/a00681.html

// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design
//	 Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any
//	 purpose is hereby granted without fee, provided that the above copyright
//	 notice appear in all copies and that both that copyright notice and this
//	 permission notice appear in supporting documentation.
// The author or Addison-Welsey Longman make no representations about the
//	 suitability of this software for any purpose. It is provided "as is"
//	 without express or implied warranty.

#ifndef MAGIC_TYPELIST_H
#define MAGIC_TYPELIST_H

#include "nulltype.h"

/**
 * Template magic
 */
namespace magic {

/**
 * A list of types, code taken from
 */
template <class T, class U>
struct Typelist {
  typedef T Head;
  typedef U Tail;
};

/**
 * Creates a new type list
 */
template <typename T1 = NullType,
          typename T2 = NullType,
          typename T3 = NullType,
          typename T4 = NullType,
          typename T5 = NullType,
          typename T6 = NullType,
          typename T7 = NullType,
          typename T8 = NullType,
          typename T9 = NullType,
          typename T10 = NullType,
          typename T11 = NullType,
          typename T12 = NullType,
          typename T13 = NullType,
          typename T14 = NullType,
          typename T15 = NullType,
          typename T16 = NullType,
          typename T17 = NullType,
          typename T18 = NullType>
struct MakeTypelist {
  private:
  /** The tail of the type list (everything except the first element) */
  typedef typename MakeTypelist<T2,
                                T3,
                                T4,
                                T5,
                                T6,
                                T7,
                                T8,
                                T9,
                                T10,
                                T11,
                                T12,
                                T13,
                                T14,
                                T15,
                                T16,
                                T17,
                                T18>::result TailResult;

  public:
  /** The type list (the first element + tail) */
  typedef magic::Typelist<T1, TailResult> result;
};

/**
 * The tail of the type list (equivalent to a null pointer)
 */
template <>
struct MakeTypelist<> {
  typedef magic::NullType result;
};

} // namespace magic

#endif // MAGIC_TYPELIST_H
