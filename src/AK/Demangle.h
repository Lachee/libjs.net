/*
 * Copyright (c) 2018-2020, Andreas Kling <andreas@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/ByteString.h>
#include <AK/StringView.h>

namespace AK {

inline ByteString demangle(StringView name)
{
    return name.to_byte_string();
}

}

#if USING_AK_GLOBALLY
using AK::demangle;
#endif
