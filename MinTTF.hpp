// MIT License
//
// Copyright(c) 2024 Mihail Mladenov
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#pragma once


#include <cstdint>

#include <vector>
#include <array>
#include <span>
#include <deque>
#include <unordered_map>
#include <unordered_set>

#include <string>
#include <string_view>

#include <functional>

#include <any>
#include <variant>
#include <utility>
#include <type_traits>


namespace MTTF
{
    using U8 = uint8_t;
    using U16 = uint16_t;
    using U32 = uint32_t;
    using U64 = uint64_t;

    using I8 = int8_t;
    using I16 = int16_t;
    using I32 = int32_t;
    using I64 = int64_t;

    using F32 = float;
    using F64 = double;

    using C = char;
    using Byte = uint8_t;
    using B = bool;
    using V = void;
    using Void = V;


    template <typename T>
    using Array = std::vector<T>;

    template <typename T, U64 size>
    using StaticArray = std::array<T, size>;

    template <typename T>
    using Span = std::span<T>;

    template <typename K, typename V>
    using Map = std::unordered_map<K, V>;

    template <typename K>
    using Set = std::unordered_set<K>;


    template<typename F, typename S>
    using Pair = std::pair<F, S>;

    template <typename T, typename U>
    constexpr auto MakePair(const T& f, const U& s) -> Pair<T, U>
    {
        return std::make_pair(f, s);
    }

    using Str = std::string;
    using StrView = std::string_view;


    template <typename T>
    using Function = std::function<T>;


    using Any = std::any;

    template <typename T>
    const T& AnyCast(const Any& a)
    {
        return std::any_cast<const T&>(a);
    }

    template <typename T>
    T& AnyCast(Any& a)
    {
        return std::any_cast<T&>(a);
    }


    template <typename... Ts>
    using Variant = std::variant<Ts...>;

    template<typename TR, class TVisitor, class... TVariants >
    inline constexpr TR Visit(TVisitor&& vis, TVariants&&... vars)
    {
        return std::visit(vis, vars...);
    }

    template <typename T, typename... Ts>
    inline constexpr auto HoldsAlternative(const Variant<Ts...>& v) -> B
    {
        return std::holds_alternative<T>(v);
    }

    template <typename T, typename... Ts>
    constexpr auto Get(Variant<Ts...>& v) -> T&
    {
        return std::get<T>(v);
    }

    template <typename T, typename... Ts>
    constexpr auto Get(const Variant<Ts...>& v) -> const T&
    {
        return std::get<T>(v);
    }


    template <typename T>
    using RemoveReference = std::remove_reference_t<T>;

    template <typename T>
    [[nodiscard]]
    constexpr auto Forward(RemoveReference<T>&& t) noexcept -> T&&
    {
        return std::forward<T>(t);
    }

    template <typename T>
    [[nodiscard]]
    constexpr auto Forward(RemoveReference<T>& t) noexcept -> T&&
    {
        return std::forward<T>(t);
    }

    enum class Error
    {
        Success,
        FileReadError,
        UnsupportedFormat,
        NoCFFTable,
        NoGlyfTable,
        NoNameTable,
        NoLocaTable,
        NoMaxpTable,
        NoCmapTable,
        NoHheaTable,
        NoHeadTable,
        NoHmtxTable,
        UnsupportedCharEncoding,
        UnsupportedLocaTableVersion,
        UnsupportedHheaTableVersion,
        UnsupportedLocaTableIndex,
    };

    struct OffsetTable
    {
        U32 version;
        U16 numTables;
        U16 searchRange;
        U16 entrySelector;
        U16 rangeShift;
    };

    struct TableDirectoryEntry
    {
        U32 tag;
        U32 checksum;
        U32 offset;
        U32 length;
    };

    enum class FontVersion
    {
        AppleTTF,
        OpenType10,
        OpenTypeCCF,
        OldPostScript,
        Unsupported,
    };

    struct Location
    {
        U32 offset;
        U32 length;
    };

    using TTFScalar = I16;

    struct TTFPoint
    {
        TTFScalar x;
        TTFScalar y;
    };

    struct QuadraticBezierCurve
    {
        TTFPoint startPoint;
        TTFPoint controlPoint;
        TTFPoint endPoint;

        QuadraticBezierCurve(TTFPoint s, TTFPoint c, TTFPoint e)
            : startPoint(s), controlPoint(c), endPoint(e)
        {

        }
    };

    struct Line
    {
        TTFPoint startPoint;
        TTFPoint endPoint;

        Line(TTFPoint s, TTFPoint e)
            : startPoint(s), endPoint(e)
        {

        }
    };

    using TTFCurve = Variant<QuadraticBezierCurve, Line>;

    struct GlyphData
    {
        Array<TTFCurve> components;
        Line boundingBoxDiagonal;
    };

    // The contents of the .ttf file;
    class FontData
    {
        Span<const U8> data;

        U32 tableCount;
        Location headTable;
        Location locaTable;
        Location hheaTable;
        Location cmapTable;
        Location maxpTable;
        Location glyfTable;
        Location hmtxTable;
        Location kernTable;
        Location nameTable;
        Location cffTable;

        U32 indexMapOffset;
        U32 charEncodingFormat;
        B longLocaIndex;

    public:
        U16 unitsPerEm;
        // The bounding box
        I16 xMin;
        I16 yMin;
        I16 xMax;
        I16 yMax;

        U16 numberOfLongHorizontalMetrics;
        I16 ascent;
        I16 descent;
        I16 line_gap;
        U16 advance_width_max;

        auto Load(StrView path) -> Error;
        auto Load(Span<const U8>) -> Error;
        auto GetCharIndex(I32 codepoint) const -> U32;
        auto FetchGlyphFataForCodepoint(I32 codepoint) -> GlyphData;
    private:

        auto ParseContents() -> Error;
        auto ParseTtOutlinesFont() -> Error;
        auto ParseCffOutlinesFont()->Error;
        auto FindTable(U32 tag) -> Location;
        auto ParseTtfContainedFont() -> Error;
        auto FetchGlobalInfoFromHead() -> Error;
        auto FetchGlobalInfoFromHhea() -> Error;
        auto GetIdxDataTableFromCmap() -> Error;


        auto GetCharIndexFmt4(U32 codepoint) const -> U32;
        auto GetCharIndexFmt6(U32 codepoint) const -> U32;
        auto GetCharIndexFmt12(U32 codepoint) const -> U32;
        auto GetGlyphOffset(U32 glyphIndex) const -> U32;
        auto FetchGlyphData(U32 glyphIndex) -> GlyphData;
        auto LoadContour
        (
            GlyphData& data,
            Span<const TTFPoint> vertices,
            Span<const U8> flags,
            U64 sidx,
            U64 eidx
        ) -> U64;
    };


    // These should work on little endian machine hence we need to
    // use conversion in order to abstract endianess.
    constexpr U32 CFF_TAG_LE = 0x20464643;
    constexpr U32 GLYF_TAG_LE = 0x66796C67;
    constexpr U32 NAME_TAG_LE = 0x656D616E;
    constexpr U32 LOCA_TAG_LE = 0x61636F6C;
    constexpr U32 MAXP_TAG_LE = 0x7078616D;
    constexpr U32 CMAP_TAG_LE = 0x70616D63;
    constexpr U32 HEAD_TAG_LE = 0x64616568;
    constexpr U32 HHEA_TAG_LE = 0x61656868;
    constexpr U32 HMTX_TAG_LE = 0x78746D68;

    constexpr U16 PLATFORM_ID_UNICODE = 0;
    constexpr U16 PLATFORM_ID_MICROSOFT = 3;

    constexpr U16 PLATFORM_SPECIFIC_ID_MS_UCS2 = 1;
    constexpr U16 PLATFORM_SPECIFIC_ID_MS_UCS4 = 10;
}

#ifdef MIN_TTF_IMPLEMENTATION

namespace WL
{

}

#endif