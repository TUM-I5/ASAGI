/**
 * @file
 *  This file is part of ASAGI.
 *
 *  ASAGI is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of
 *  the License, or  (at your option) any later version.
 *
 *  ASAGI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with ASAGI.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Diese Datei ist Teil von ASAGI.
 *
 *  ASAGI ist Freie Software: Sie koennen es unter den Bedingungen
 *  der GNU Lesser General Public License, wie von der Free Software
 *  Foundation, Version 3 der Lizenz oder (nach Ihrer Option) jeder
 *  spaeteren veroeffentlichten Version, weiterverbreiten und/oder
 *  modifizieren.
 *
 *  ASAGI wird in der Hoffnung, dass es nuetzlich sein wird, aber
 *  OHNE JEDE GEWAEHELEISTUNG, bereitgestellt; sogar ohne die implizite
 *  Gewaehrleistung der MARKTFAEHIGKEIT oder EIGNUNG FUER EINEN BESTIMMTEN
 *  ZWECK. Siehe die GNU Lesser General Public License fuer weitere Details.
 *
 *  Sie sollten eine Kopie der GNU Lesser General Public License zusammen
 *  mit diesem Programm erhalten haben. Wenn nicht, siehe
 *  <http://www.gnu.org/licenses/>.
 *
 * @copyright 2001 Andrei Alexandrescu
 * @copyright 2015 Sebastian Rettenberger <rettenbs@in.tum.de>
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
namespace magic
{

/**
 * A list of types, code taken from
 */
template <class T, class U>
struct Typelist
{
	typedef T Head;
	typedef U Tail;
};

/**
 * Creates a new typelist
 */
template
<
	typename T1  = NullType, typename T2  = NullType, typename T3  = NullType,
	typename T4  = NullType, typename T5  = NullType, typename T6  = NullType,
	typename T7  = NullType, typename T8  = NullType, typename T9  = NullType,
	typename T10 = NullType, typename T11 = NullType, typename T12 = NullType,
	typename T13 = NullType, typename T14 = NullType, typename T15 = NullType,
	typename T16 = NullType, typename T17 = NullType, typename T18 = NullType
>
struct MakeTypelist
{
private:
	typedef typename MakeTypelist
	<
		T2 , T3 , T4 ,
		T5 , T6 , T7 ,
		T8 , T9 , T10,
		T11, T12, T13,
		T14, T15, T16,
		T17, T18
	>
	::result TailResult;

public:
	typedef magic::Typelist<T1, TailResult> result;
};

template<>
struct MakeTypelist<>
{
	typedef magic::NullType result;
};

}

#endif // MAGIC_TYPELIST_H
