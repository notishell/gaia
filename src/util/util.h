/* Copyright 2015 9x6.me. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * author: Notis Hell (notishell@gmail.com)
 */
#ifndef SRC_UTIL_UTIL_H_
#define SRC_UTIL_UTIL_H_

#include <stdint.h>
#include <endian.h>

/*
 * Short for integer types.
 */
typedef uint8_t             u1;
typedef uint16_t            u2;
typedef uint32_t            u4;
typedef uint64_t            u8;
typedef int8_t              s1;
typedef int16_t             s2;
typedef int32_t             s4;
typedef int64_t             s8;

static inline u2 u2_to_n2(u2 v) {
	return (v);
}

static inline u2 n2_to_u2(u2 v) {
	return (v);
}

static inline u4 u4_to_n4(u4 v) {
	return (v);
}

static inline u4 n4_to_u4(u4 v) {
	return (v);
}

static inline u8 u8_to_n8(u8 v) {
	return (v);
}

static inline u8 n8_to_u8(u8 v) {
	return (v);
}

#endif /* SRC_UTIL_UTIL_H_ */
