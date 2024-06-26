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

#include <algorithm>

#include <bit>

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


    template <typename T>
    auto ByteSwap(T x) -> T
    {
        T result;
        auto bytes = (Byte*)&x;

        for (auto i = 0u; i < sizeof(T); ++i)
        {
            result[i] = bytes[sizeof(T) - 1 - i];
        }

        return result;
    }

    template <typename T>
        requires (sizeof(T) == 2)
    auto ByteSwap(T x) -> T
    {
        T result;
        auto uXPtr = (U16*)&x;
        *((U16*)&result) = (*uXPtr << 8) | ((*uXPtr) >> 8);
        return result;
    }

    template <typename T>
        requires (sizeof(T) == 4)
    auto ByteSwap(T x) -> T
    {
        T result;
        auto uXPtr = (U32*)&x;
        *((U32*)&result) =
            ((*uXPtr & 0xFF000000u) >> 24) |
            ((*uXPtr & 0x00FF0000u) >> 8) |
            ((*uXPtr & 0x0000FF00u) << 8) |
            ((*uXPtr & 0x000000FFu) << 24);
        return result;
    }

    template <typename T>
        requires (sizeof(T) == 8)
    auto ByteSwap(T x) -> T
    {
        T result;
        auto uXPtr = (U64*)&x;
        *((U64*)&result) =
            ((*uXPtr & 0xFF00000000000000ull) >> 56) |
            ((*uXPtr & 0x00FF000000000000ull) >> 40) |
            ((*uXPtr & 0x0000FF0000000000ull) >> 24) |
            ((*uXPtr & 0x000000FF00000000ull) >> 8) |
            ((*uXPtr & 0x00000000FF000000ull) << 8) |
            ((*uXPtr & 0x0000000000FF0000ull) << 24) |
            ((*uXPtr & 0x000000000000FF00ull) << 40) |
            ((*uXPtr & 0x00000000000000FFull) << 56);
        return result;
    }

    template <typename T>
    auto FromBE(T x) -> T
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            return ByteSwap(x);
        }
        return x;
    }

    template <typename T>
    auto FromLE(T x) -> T
    {
        if constexpr (std::endian::native == std::endian::big)
        {
            return ByteSwap(x);
        }
        return x;
    }

    template <typename T>
    auto Max(T a, T b) -> T
    {
        return std::max(a, b);
    }

    template <typename T>
    auto Min(T a, T b) -> T
    {
        return std::min(a, b);
    }

    template <typename T>
    auto Swap(T& a, T& b) -> V
    {
        std::swap(a, b);
    }

    template <typename T>
    auto Floor(T x) -> T
    {
        return std::floor(x);
    }

    template <typename T>
    auto Ceil(T x) -> T
    {
        return std::ceil(x);
    }

    template <typename T>
    auto Sqrt(T x) -> T
    {
        return std::sqrt(x);
    }

    template <typename T>
    auto Square(T x) -> T
    {
        return x * x;
    }

    template <typename T>
    auto Round(T x) -> T
    {
        return std::round(x);
    }

    template <typename TContainer>
    auto Sort(TContainer& container)
    {
        std::sort(container.begin(), container.end());
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

        TTFPoint() {}
        TTFPoint(TTFScalar x, TTFScalar y): x(x), y(y) {}
    };

    struct QuadraticBezierCurve
    {
        TTFPoint startPoint;
        TTFPoint controlPoint;
        TTFPoint endPoint;

        QuadraticBezierCurve() {}
        QuadraticBezierCurve(TTFPoint s, TTFPoint c, TTFPoint e)
            : startPoint(s), controlPoint(c), endPoint(e)
        {

        }
    };

    struct Line
    {
        TTFPoint startPoint;
        TTFPoint endPoint;
        Line() {}
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
        I16 lineGap;
        U16 advanceWidthMax;

        auto Load(Span<const Byte>) -> Error;
        auto GetCharIndex(I32 codepoint) const -> U32;
        auto FetchGlyphDataForCodepoint(I32 codepoint) const -> GlyphData;
    private:

        auto CheckFontVersion(U32 v) const -> FontVersion;
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
        auto FetchGlyphData(U32 glyphIndex) const -> GlyphData;
        auto LoadContour
        (
            GlyphData& data,
            Span<const TTFPoint> vertices,
            Span<const U8> flags,
            U64 sidx,
            U64 eidx
        ) const -> U64;
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

    struct GrayScaleSurface
    {
        Array<Byte> data;
        U32 width;
        U32 height;
    };

    auto RasterizeGlyph(const FontData& fontData, I32 codepoint, I32 height) -> GrayScaleSurface;
}

#ifdef MIN_TTF_IMPLEMENTATION

namespace MTTF
{
    auto FontData::LoadContour(GlyphData& data, Span<const TTFPoint> vertices, Span<const U8> flags, U64 sidx, U64 eidx) const -> U64
    {
        auto cidx = sidx;

        if ((flags[cidx] & 1u) == 0)
            // The first point is control point
        {
            auto curve = QuadraticBezierCurve();

            if ((flags[eidx] & 1u) == 0)
            {
                curve.startPoint.x = ((I32(vertices[cidx].x) + I32(vertices[eidx].x)) / 2);
                curve.startPoint.y = ((I32(vertices[cidx].y) + I32(vertices[eidx].y)) / 2);
            }
            else
            {
                curve.startPoint = vertices[eidx];
            }

            curve.controlPoint = vertices[cidx];
            // No bound checking at this level.
            cidx += 1;

            if ((flags[cidx] & 1u) == 0)
            {
                curve.endPoint.x = ((I32(curve.controlPoint.x) + I32(vertices[cidx].x)) / 2);
                curve.endPoint.y = ((I32(curve.controlPoint.y) + I32(vertices[cidx].y)) / 2);
            }
            else
            {
                curve.endPoint = vertices[cidx];
            }

            data.components.emplace_back(curve);
        }

        while (cidx < eidx)
        {
            // At least the first point has been added already so we can safely check the
            // previous point.

            if ((flags[cidx] & 1u) == 0)
            {
                // The previous point must be a control point because else we would've
                // parsed the whole curve in the previous iteration
                auto startPoint =
                    TTFPoint
                    (
                        ((I32(vertices[cidx - 1].x) + I32(vertices[cidx].x)) / 2),
                        ((I32(vertices[cidx - 1].y) + I32(vertices[cidx].y)) / 2)
                    );

                if ((flags[cidx + 1] & 1u) == 0)
                {
                    auto endPoint =
                        TTFPoint
                        (
                            ((I32(vertices[cidx].x) + I32(vertices[cidx + 1].x)) / 2),
                            ((I32(vertices[cidx].y) + I32(vertices[cidx + 1].y)) / 2)
                        );

                    auto curve = QuadraticBezierCurve(startPoint, vertices[cidx], endPoint);
                    data.components.emplace_back(curve);
                }
                else
                {
                    auto curve =
                        QuadraticBezierCurve(startPoint, vertices[cidx], vertices[cidx + 1]);
                    data.components.emplace_back(curve);
                }
            }
            else
            {
                if ((flags[cidx + 1] & 1u) == 0)
                {
                    TTFPoint endPoint;

                    if (cidx + 1 == eidx)
                    {
                        if ((flags[sidx] & 1u) == 0)
                        {
                            endPoint.x = ((I32(vertices[sidx].x) + I32(vertices[eidx].x)) / 2);
                            endPoint.y = ((I32(vertices[sidx].y) + I32(vertices[eidx].y)) / 2);
                        }
                        else
                        {
                            endPoint = vertices[sidx];
                        }

                        auto curve = QuadraticBezierCurve(vertices[cidx], vertices[cidx + 1], endPoint);
                        data.components.emplace_back(curve);
                    }
                    else
                    {
                        // The next index is not the last so we can use at least one more point.
                        if ((flags[cidx + 2] & 1u) == 0)
                        {
                            endPoint.x = ((I32(vertices[cidx + 1].x) + I32(vertices[cidx + 2].x)) / 2);
                            endPoint.y = ((I32(vertices[cidx + 1].y) + I32(vertices[cidx + 2].y)) / 2);
                        }
                        else
                        {
                            endPoint = vertices[cidx + 2];
                        }

                        auto curve = QuadraticBezierCurve(vertices[cidx], vertices[cidx + 1], endPoint);
                        data.components.emplace_back(curve);
                    }

                    // We used one more point here.
                    cidx += 1;
                }
                else
                {
                    data.components.emplace_back(Line(vertices[cidx], vertices[cidx + 1]));
                }
            }

            cidx += 1;
        }

        if (cidx == eidx)
        {
            if ((flags[eidx] & 1u) == 0)
            {
                // The previous point must be a control point because else we would've
                // parsed the whole curve before this part of the code
                auto startPoint =
                    TTFPoint
                    (
                        ((I32(vertices[eidx - 1].x) + I32(vertices[eidx].x)) / 2),
                        ((I32(vertices[eidx - 1].y) + I32(vertices[eidx].y)) / 2)
                    );

                TTFPoint endPoint;

                if ((flags[sidx] & 1u) == 0)
                {
                    endPoint.x = ((I32(vertices[sidx].x) + I32(vertices[eidx].x)) / 2);
                    endPoint.y = ((I32(vertices[sidx].y) + I32(vertices[eidx].y)) / 2);
                }
                else
                {
                    endPoint = vertices[sidx];
                }

                auto curve = QuadraticBezierCurve(startPoint, vertices[eidx], endPoint);
                data.components.emplace_back(QuadraticBezierCurve(curve));
            }
            else
            {
                // If the start was control point we have already pushed this curve in the
                // beginning.
                if ((flags[sidx] & 1u) > 0)
                {
                    data.components.emplace_back(Line(vertices[eidx], vertices[sidx]));
                }
            }
        }

        return eidx + 1;
    }


    auto FontData::Load(Span<const U8> data) -> Error
    {
        this->data = data;
        return ParseContents();
    }


    auto FontData::GetCharIndex(I32 codepoint) const -> U32
    {
        switch (charEncodingFormat)
        {
            case 4:
                return GetCharIndexFmt4(codepoint);
            case 6:
                return GetCharIndexFmt6(codepoint);
            case 12:
                GetCharIndexFmt12(codepoint);
            // This is impossible since we already checked for those formats
            default:
                return 0;
        }
    }


    auto FontData::FetchGlyphDataForCodepoint(I32 codepoint) const -> GlyphData
    {
        return FetchGlyphData(GetCharIndex(codepoint));
    }


    inline auto FontData::CheckFontVersion(U32 v) const -> FontVersion
    {
        if (v == FromLE(0x00000100u))
        {
            return FontVersion::OpenType10;
        }
        else if (v == FromLE(0x65757274u))
        {
            return FontVersion::AppleTTF;
        }
        else if (v == 0x4F54544F)
        {
            return FontVersion::OpenTypeCCF;
        }
        else if (v == 0x31707974)
        {
            return FontVersion::OldPostScript;
        }
        else
        {
            return FontVersion::Unsupported;
        }
    }


    auto FontData::ParseContents() -> Error
    {
        auto offsetTable = (OffsetTable*)data.data();

        auto version = CheckFontVersion(offsetTable->version);

        tableCount = FromBE(offsetTable->numTables);

        switch (version)
        {
            case FontVersion::OpenType10:
            case FontVersion::AppleTTF:
                return ParseTtOutlinesFont();
            case FontVersion::OpenTypeCCF:
                return ParseCffOutlinesFont();
            default:
                return Error::UnsupportedFormat;
        }

        return Error::Success;
    }


    auto FontData::ParseTtOutlinesFont() -> Error
    {
        auto status = ParseTtfContainedFont();

        if (status != Error::Success)
        {
            return status;
        }

        glyfTable = FindTable(FromLE(GLYF_TAG_LE));
        locaTable = FindTable(FromLE(LOCA_TAG_LE));

        if (glyfTable.offset == 0)
        {
            return Error::NoGlyfTable;
        }

        if (locaTable.offset == 0)
        {
            return Error::NoLocaTable;
        }

        return Error::Success;
    }


    auto FontData::ParseCffOutlinesFont() -> Error
    {
        auto status = ParseTtfContainedFont();

        if (status != Error::Success)
        {
            return status;
        }

        // TODO: Add CFF outlines support.
        return Error::UnsupportedFormat;
    }


    auto FontData::FindTable(U32 tag) -> Location
    {
        static constexpr U32 initOffset = sizeof(OffsetTable);
        static constexpr U32 stride = sizeof(TableDirectoryEntry);

        for(auto k = 0u; k < tableCount; ++k)
        {
            auto currentOffset  = initOffset + k * stride;

            if (*(const U32*)((const Byte*) data.data() + currentOffset) == tag)
            {
                // TODO: provide support for computing table checksums
                auto tdePtr = (const TableDirectoryEntry*) ((const Byte*)data.data() + currentOffset);

                return Location
                {
                    .offset = FromBE(tdePtr->offset),
                    .length = FromBE(tdePtr->length)
                };
            }
        }

        return Location{ .offset = 0, .length = 0 };
    }


    auto FontData::ParseTtfContainedFont() -> Error
    {
        #define MTTF_GET_TABLES(VAR_NAME, TAG, ERROR) \
            VAR_NAME = FindTable(FromLE(TAG)); \
            if (VAR_NAME.offset == 0) \
            { \
                return ERROR; \
            } \
        
        MTTF_GET_TABLES(cmapTable, CMAP_TAG_LE, Error::NoCmapTable);
        MTTF_GET_TABLES(nameTable, NAME_TAG_LE, Error::NoNameTable);
        MTTF_GET_TABLES(maxpTable, MAXP_TAG_LE, Error::NoMaxpTable);
        MTTF_GET_TABLES(hheaTable, HHEA_TAG_LE, Error::NoHheaTable);
        MTTF_GET_TABLES(headTable, HEAD_TAG_LE, Error::NoHeadTable);
        MTTF_GET_TABLES(hmtxTable, HMTX_TAG_LE, Error::NoHmtxTable);

        auto status = GetIdxDataTableFromCmap();

        if (status != Error::Success)
        {
            return status;
        }

        status = FetchGlobalInfoFromHead();

        if (status != Error::Success)
        {
            return status;
        }

        return FetchGlobalInfoFromHhea();
    }


    auto FontData::FetchGlobalInfoFromHead() -> Error
    {
        struct HeadTable1 
        {
            U32 version;
            U32 fontRevision;
            U32 checksumAdjustment;
            U32 magicNumber;
            U16 flags;
            U16 unitsPerEm;
        };

        struct HeadTable2 
        {
            I64 created;
            I64 modified;
            I16 xMin;
            I16 yMin;
            I16 xMax;
            I16 yMax;
            U16 macStyle;
            U16 unused;
            I16 fontDirectionJint;
            I16 indexToLocaFormat;
            I16 glyphDataFormat;
        };

        auto headTable1Ptr = (const HeadTable1*)(data.data() + headTable.offset);
        auto headTable2Ptr = (const HeadTable2*)(data.data() + headTable.offset + sizeof(HeadTable1));

        if (FromBE(headTable1Ptr->version) != 0x00010000) 
        {
            return Error::UnsupportedLocaTableVersion;
        }
        else
        {
            xMin = FromBE(headTable2Ptr->xMin);
            yMin = FromBE(headTable2Ptr->yMin);
            xMax = FromBE(headTable2Ptr->xMax);
            yMax = FromBE(headTable2Ptr->yMax);
            unitsPerEm = FromBE(headTable1Ptr->unitsPerEm);

            switch(FromBE(headTable2Ptr->indexToLocaFormat))
            {
                case 0:
                    longLocaIndex = false;
                    return Error::Success;
                case 1:
                    longLocaIndex = true;
                    return Error::Success;
                default:
                    return Error::UnsupportedLocaTableIndex;
            }
        }
    }


    auto FontData::FetchGlobalInfoFromHhea() -> Error
    {
        struct HheaTable
        {
            U32 version;
            I16 ascent;
            I16 descent;
            I16 lineGap;
            U16 advanceWidthMax;
            I16 minLeftSideBearing;
            I16 minRightSideBearing;
            // "The extent is the distance from the left side bearing to the right most positions 
            // in the glyph outline." 
            //    - https://developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6hhea.html
            I16 xMaxExtent;
            I16 caretSlopeRise;
            I16 caretSoloRun;
            I16 caretOffset;
            I16 reserved0;
            I16 reserved1;
            I16 reserved2;
            I16 reserved3;
            I16 metricDataFormat;
            U16 numberOfLongHorizontalMetrics;
        };


        auto hheaTablePtr  = (const HheaTable*)(data.data() + hheaTable.offset);

        if (FromBE(hheaTablePtr->version) != 0x00010000)
        {
            return Error::UnsupportedHheaTableVersion;
        }

        ascent = FromBE(hheaTablePtr->ascent);
        descent = FromBE(hheaTablePtr->descent);
        lineGap = FromBE(hheaTablePtr->lineGap);
        advanceWidthMax = FromBE(hheaTablePtr->advanceWidthMax);
        numberOfLongHorizontalMetrics = FromBE(hheaTablePtr->numberOfLongHorizontalMetrics);

        return Error::Success;
    }


    auto FontData::GetIdxDataTableFromCmap() -> Error
    {
        struct CmapTableHeader
        {
            U16 version;
            U16 subtableCount;
        };

        struct CmapSubtable
        {
            U16 platformId;
            U16 platformSpecificId;
            U32 offset;
        };

        auto cmapHeaderPtr = (const CmapTableHeader*)(data.data() + cmapTable.offset);

        if (cmapHeaderPtr->version != 0)
        {
            return Error::UnsupportedFormat;
        }

        auto subtableCount = FromBE(cmapHeaderPtr->subtableCount);

        this->indexMapOffset = 0;

        for (auto k = 0; k < subtableCount; ++k)
        {
            auto offset = this->cmapTable.offset + sizeof(CmapTableHeader) + k * sizeof(CmapSubtable);

            auto subtablePtr = (const CmapSubtable*)(this->data.data() + offset);

            auto platformId = FromBE(subtablePtr->platformId);
            auto platformSpecificId = FromBE(subtablePtr->platformSpecificId);

            // We support only unicode encodings.

            if (platformId == PLATFORM_ID_UNICODE)
            {
                this->indexMapOffset = this->cmapTable.offset + FromBE(subtablePtr->offset);
                break;
            }

            if (
                    platformId == PLATFORM_ID_MICROSOFT &&
                    (
                        platformSpecificId == PLATFORM_SPECIFIC_ID_MS_UCS2 ||
                        platformSpecificId == PLATFORM_SPECIFIC_ID_MS_UCS4
                    )
               )
            {
                this->indexMapOffset = this->cmapTable.offset + FromBE(subtablePtr->offset);
            }
        }

        if (this->indexMapOffset == 0)
        {
            return Error::UnsupportedCharEncoding;
        }

        // We only support format 4, 6 and 12
        this->charEncodingFormat = FromBE(*(const U16*)(this->data.data() + this->indexMapOffset));

        if (this->charEncodingFormat != 4 && this->charEncodingFormat != 6 && this->charEncodingFormat != 12)
        {
            return Error::UnsupportedFormat;
        }

        this->indexMapOffset += 2;
        return Error::Success;
    }


    auto FontData::GetCharIndexFmt4(U32 codepoint) const -> U32
    {
        struct Table4
        {
            U16 length;
            U16 unused;
            U16 segCountX2;
            U16 searchRange;
            U16 entrySelector;
            U16 rangeShift;
        };

        auto t4Ptr = (const Table4*) (this->data.data() + this->indexMapOffset);
        auto segCountX2 = FromBE(t4Ptr->segCountX2);
        auto searchRange = FromBE(t4Ptr->searchRange);
        auto entrySelector = FromBE(t4Ptr->entrySelector);
        auto rangeShift = FromBE(t4Ptr->rangeShift);

        auto dataBeginning = this->indexMapOffset + sizeof(Table4);

        auto searchOffset = dataBeginning;

        // Binary search can be performed since segments are sorted by end codepoint
        if (codepoint >= FromBE(*(const U16*)(this->data.data() + searchOffset + rangeShift)))
        {
            searchOffset += rangeShift;
        }

        searchOffset -= 2;

        for (auto i = 0u; i < entrySelector; ++i)
        {
            searchRange /= 2;
            auto endCodepoint = FromBE(*(const U16*)(this->data.data() + searchOffset + searchRange));

            if (codepoint > endCodepoint)
            {
                searchOffset += searchRange;
            }
        }

        searchOffset += 2;

        // Now the search_offset should be what we need.
        auto segment = (searchOffset - dataBeginning) / 2;

        // Two bytes pad after the end codes
        auto startCodesOffset = dataBeginning + segCountX2 + 2;
        auto deltasOffset = startCodesOffset + segCountX2;
        auto rangesOffset = deltasOffset + segCountX2;

        auto segmentStartCode = FromBE(*(const U16*)(this->data.data() + startCodesOffset + 2 * segment));
        auto segmentRangeOffset = FromBE(*(const U16*)(this->data.data() + rangesOffset + 2 * segment));
        auto segmentDeltaOffset = FromBE(*(const U16*)(this->data.data() + deltasOffset + 2 * segment));

        if (codepoint < segmentStartCode)
        {
            return 0;
        }
        else if (segmentRangeOffset == 0)
        {
            return (codepoint + segmentDeltaOffset) & 0xFFFFu;
        }
        else
        {
            // According to the specification we need to use this obscure indexing trick
            auto glyphIndexOffset = U32(segmentRangeOffset);
            glyphIndexOffset += 2 * (codepoint - segmentStartCode);
            glyphIndexOffset += rangesOffset + 2 * segment;

            auto glyphIndex = FromBE(*(const U16*)(this->data.data() + glyphIndexOffset));

            if (glyphIndex != 0)
            {
                return glyphIndex + segmentDeltaOffset;
            }
            else
            {
                return glyphIndex;
            }
        }
    }


    auto FontData::GetCharIndexFmt6(U32 codepoint) const -> U32
    {
        // Skip the first two entries.
        auto dataBeginning = this->indexMapOffset + 4;

        auto firstCode = FromBE(*(const U16*)(this->data.data() + dataBeginning));
        dataBeginning += 2;
        auto code_count = FromBE(*(const U16*)(this->data.data() + dataBeginning));
        dataBeginning += 2;

        if (codepoint >= firstCode + code_count)
        {
            return 0;
        }
        else
        {
            auto indexOffset = Max(U32(dataBeginning + (U16(codepoint) - firstCode)), U32(data.size() - 2));
            return FromBE(*(const U16*)(this->data.data() + indexOffset));
        }

    }


    auto FontData::GetCharIndexFmt12(U32 codepoint) const -> U32
    {
        struct Header
        {
            U32 format;
            U32 length;
            U32 unused;
            U32 groupCount;
        };

        struct Group
        {
            U32 startCodepoint;
            U32 endCodepoint;
            U32 startCodepointIdx;
        };

        auto headerPtr = (const Header*)(this->data.data() + this->indexMapOffset - 2);

        auto groupCount = FromBE(headerPtr->groupCount);

        auto groupsOffset = this->indexMapOffset - 2 + sizeof(Header);

        auto searchStart = 0u;
        auto searchEnd = groupCount;

        while (searchStart < searchEnd)
        {
            auto mid = searchStart + (searchEnd - searchEnd) / 2;

            auto currentSegmentPtr = (const Group*) (this->data.data() + groupsOffset + mid * sizeof(Group));

            auto startCodepoint = FromBE(currentSegmentPtr->startCodepoint);
            auto endCodepoint = FromBE(currentSegmentPtr->endCodepoint);

            if (startCodepoint > codepoint)
            {
                searchEnd = mid;
            }
            else if (endCodepoint < codepoint)
            {
                searchStart = mid + 1;
            }
            else
            {
                return FromBE(currentSegmentPtr->startCodepointIdx) + mid;
            }
        }

        return 0;
    }


    auto FontData::GetGlyphOffset(U32 glyphIndex) const -> U32
    {
        auto offset = this->glyfTable.offset;

        if (this->longLocaIndex)
        {
            offset += FromBE(*(const U32*)(this->data.data() + this->locaTable.offset + 4 * glyphIndex));
        }
        else
        {
            offset += FromBE(*(const U16*)(this->data.data() + this->locaTable.offset + 2 * glyphIndex)) * 2;
        }

        return offset;
    }


    auto FontData::FetchGlyphData(U32 glyphIndex) const -> GlyphData
    {
        GlyphData glyphData;

        auto glyphOffset = this->GetGlyphOffset(glyphIndex);

        struct GlyfHeader
        {
            I16 numberOfContours;
            I16 xMin;
            I16 yMin;
            I16 xMax;
            I16 yMax;
        };

        auto glyfHeaderPtr = (const GlyfHeader*)(this->data.data() + glyphOffset);

        auto d0 = TTFPoint(FromBE(glyfHeaderPtr->xMin), FromBE(glyfHeaderPtr->yMin));
        auto d1 = TTFPoint(FromBE(glyfHeaderPtr->xMax), FromBE(glyfHeaderPtr->yMax));

        glyphData.boundingBoxDiagonal = Line(d0, d1);

        auto currentOffset = glyphOffset + sizeof(GlyfHeader);

        auto numberOfContours = FromBE(glyfHeaderPtr->numberOfContours);

        // Simple glyph
        if (numberOfContours >= 0)
        {
            Array<U16> endPointsOfContours;

            auto endPointsOfContoursPtr = (const U16*)(this->data.data() + currentOffset);

            for (auto i = 0; i < numberOfContours; ++i)
            {
                endPointsOfContours.push_back(FromBE(endPointsOfContoursPtr[i]));
            }

            currentOffset += 2 * numberOfContours;

            auto instructionLength = FromBE(*(const U16*)(this->data.data() + currentOffset));

            // Skip instructions;
            currentOffset += instructionLength + 2;

            auto numberOfVertices = endPointsOfContours[(numberOfContours - 1)] + 1;

            Array<U8> flags;
            flags.reserve(numberOfVertices);
            U8 repeatFlags = 0;
            U8 flag  = 0;

            // Loading the flags first storing the repeating ones as well.
            for (auto i = 0u; i < numberOfVertices; ++i)
            {
                if (repeatFlags == 0)
                {
                    flag = this->data.data()[currentOffset];
                    currentOffset += 1;

                    if ((flag & 0b00001000) > 0)
                    {
                        repeatFlags = this->data.data()[currentOffset];
                        currentOffset += 1;
                    }
                }
                else
                {
                    repeatFlags -= 1;
                }

                flags.push_back(flag);
            }

            Array<TTFPoint> vertices;
            vertices.resize(numberOfVertices);

            auto prevCoord = 0;
            // Reading the first coordinates
            for (auto i = 0u; i < numberOfVertices; ++i)
            {
               // Coordinate is encoded as u8
               if ((flags[i] & 0b00000010u) > 0)
               {
                   auto tmpCoord = this->data[currentOffset];
                   currentOffset += 1;

                   // Holy shit!!! The documentation was so misleading here. I had so much trouble
                   // finding this bug...
                   vertices[i].x = I32(prevCoord) + (((flags[i] & 0b00010000u) > 0) ? I32(tmpCoord) : -I32(tmpCoord));
                   prevCoord = vertices[i].x;
               }
               // coordinate is encoded as i16
               else
               {
                   // checks if the coordinate is not repeated
                   if ((flags[i] & 0b00010000u) == 0)
                   {
                       auto tmpCoord = FromBE(*(const I16*)(this->data.data() + currentOffset));
                       currentOffset += 2;
                       vertices[i].x = prevCoord + I32(tmpCoord);
                       prevCoord = vertices[i].x;
                   }
                   else
                   {
                       vertices[i].x = prevCoord;
                   }
               }
            }

            prevCoord = 0;
            // Reading the second coordinates
            for (auto i = 0u; i < numberOfVertices; ++i)
            {
                // Coordinate is encoded as u8
                if ((flags[i] & 0b00000100u) > 0)
                {
                    auto tmpCoord = this->data[currentOffset];
                    currentOffset += 1;

                    // Holy mother of fuck why nobody mentioned that those were deltas as well?!?
                    // I'm looking at you
                    // developer.apple.com/fonts/TrueType-Reference-Manual/RM06/Chap6glyf.html
                    vertices[i].y = I32(prevCoord) + (((flags[i] & 0b00100000u) > 0) ? I32(tmpCoord) : -I32(tmpCoord));
                    prevCoord = vertices[i].y;
                }
                // coordinate is encoded as i16
                else
                {
                    // checks if the coordinate is not repeated
                    if ((flags[i] & 0b00100000u) == 0)
                    {
                        auto tmpCoord = FromBE(*(const I16*)(this->data.data() + currentOffset));
                        currentOffset += 2;
                        vertices[i].y = prevCoord + I32(tmpCoord);
                        prevCoord = vertices[i].y;
                    }
                    else
                    {
                        vertices[i].y = prevCoord;
                    }
                }
            }

            auto startIndex = 0u;

            for (auto i = 0; i < endPointsOfContours.size(); ++i)
            {
                startIndex =
                    LoadContour
                    (
                        glyphData,
                        Span<const TTFPoint>{vertices},
                        Span<const U8>{flags},
                        startIndex,
                        endPointsOfContours[i]
                    );
            }
        }
        // Compound glyph
        else
        {
            // TODO: implement support for compound glyphs
        }

        return glyphData;
    }
    

    // Rasterizer related functions

    struct Point
    {
        F32 x;
        F32 y;

        Point() {}
        Point(F32 x, F32 y) : x(x), y(y) {}
    };


    struct Edge
    {
        Point lowermostPoint;
        Point uppermostPoint;
        // Needs to be +1/-1 but gets padded to 4 bytes anyway in order to preserve alignment 
        F32 direction;
    };

    inline auto operator<(const Edge& e0, const Edge& e1) -> bool 
    {
        return e0.uppermostPoint.y < e1.uppermostPoint.y;
    }

    inline auto operator==(const Edge& e0, const Edge& e1) -> bool
    {
        return e0.uppermostPoint.y == e1.uppermostPoint.y;
    }


    auto AddEdge(Array<Edge>& edges, Point startPoint, Point endPoint) -> V
    {
        if (startPoint.y < endPoint.y)
        {
            edges.emplace_back(startPoint, endPoint, 1.0);
        }
        else if (startPoint.y > endPoint.y)
        {
            edges.emplace_back(endPoint, startPoint, -1.0);
        }
        else
        {
            // In case this happens we can safely drop those
        }
    }


    // Use special case of De Casteljau's algorithm to turn the curve into polyline.
    auto Linearize
    (
        Array<Edge>& edges,
        F32 flatnessThreshold,
        Point point0,
        Point point1,
        Point point2
    ) -> V
    {
        // We can safely give hard limit to the depth of the "recursive" subdivision. This is because
        // depth of 15 giving us 2^15 == 32768 divisions for a single curve is insane even at very large
        // resolutions. For example even if the curve is bounded by a box of width 4000 pixels then for
        // the length of the curve is
        //  \[
        // 2\int\limits_0^1 \left| (p_2 - p_1)t + (p_1 - p_0) (1-t)\right|dt 
        //  =
        // 2\int\limits_0^1 \left| (p_2 - 2p_1 +p_0)t + (p_1 - p_0)\right|dt 
        // \leq
        // 2 \int\limits_0^1 8000t dt + 2 \int\limits_0^1 4000 dt 
        // = 
        // 8000 + 8000 
        // =
        // 16000
        // 	\]
        // This means that on average the length of each individual piece of this subdivided curve is 
        // going to be less than half a pixel!

        // Number of points times the division depth
        static constexpr U32 STACK_SIZE = 3 * 16;

        StaticArray<Point, STACK_SIZE> stack;
        stack[0] = point0;
        stack[1] = point1;
        stack[2] = point2;
        U32 stackSize = 3;

        while (0 < stackSize)
        {
            auto beta02 = stack[stackSize - 1];
            auto beta01 = stack[stackSize - 2];
            auto beta00 = stack[stackSize - 3];
            stackSize -= 3;

            auto approxLength =
                Sqrt(Square(beta00.x - beta01.x) + Square(beta00.y - beta01.y)) +
                Sqrt(Square(beta01.x - beta02.x) + Square(beta01.y - beta02.y));

            if (approxLength > flatnessThreshold && stackSize + 6 < STACK_SIZE)
            {
                auto beta10 = Point((beta01.x + beta00.x) / 2.0, (beta01.y + beta00.y) / 2.0);
                auto beta11 = Point((beta02.x + beta01.x) / 2.0, (beta02.y + beta01.y) / 2.0);
                auto beta20 = Point((beta11.x + beta10.x) / 2.0, (beta11.y + beta10.y) / 2.0);
                // We still havent reached the desired flatness which means we have to subdivide
                stack[stackSize] = beta00;
                stack[stackSize + 1] = beta10;
                stack[stackSize + 2] = beta20;

                stack[stackSize + 3] = beta20;
                stack[stackSize + 4] = beta11;
                stack[stackSize + 5] = beta02;

                stackSize += 6;
            }
            else 
            {
                AddEdge(edges, beta00, beta01);
                AddEdge(edges, beta01, beta02);
            }
        }
    }


    static constexpr F32 FLATNESS_CONSTANT_IN_PIXELS = 1;

    auto Linearize(const GlyphData& glyphData, F32 scale) -> Array<Edge>
    {
        Array<Edge> edges;

        // Invert the scalar transform in order to get the flatness threshold in the glyph coordinate
        // system.
        auto flatnessThreshold = FLATNESS_CONSTANT_IN_PIXELS / scale;
        // We do this to avoid using square root during linearization of Bezier Curves.
        flatnessThreshold = flatnessThreshold * flatnessThreshold;

        for (auto& curve : glyphData.components)
        {
            if (HoldsAlternative<QuadraticBezierCurve>(curve))
            {
                auto& bezier = Get<QuadraticBezierCurve>(curve);
                Linearize
                (
                    edges,
                    flatnessThreshold,
                    Point(bezier.startPoint.x, bezier.startPoint.y),
                    Point(bezier.controlPoint.x, bezier.controlPoint.y),
                    Point(bezier.endPoint.x, bezier.endPoint.y)
                );
            }
            else if (HoldsAlternative<Line>(curve))
            {
                auto& line = Get<Line>(curve);
                AddEdge
                (
                    edges,
                    Point(line.startPoint.x, line.startPoint.y),
                    Point(line.endPoint.x, line.endPoint.y)
                );
            }
        }

        return edges;
    }


    auto TransformEdgesToSurfaceSpace(Array<Edge>& edges, F32 scale, Point translation) -> V
    {
        for (auto& edge : edges)
        {
            // We flip the second coordinate in order to match the coordinates of the pixel grid.
            // This doesn't change the direction of the edges but now the uppermost edge is with lower
            // numbers as vertical coordinates which is what we want.
            edge.lowermostPoint.x *= scale;
            edge.lowermostPoint.y *= -scale;
            edge.uppermostPoint.x *= scale;
            edge.uppermostPoint.y *= -scale;
            edge.lowermostPoint.x += translation.x;
            edge.lowermostPoint.y += translation.y;
            edge.uppermostPoint.x += translation.x;
            edge.uppermostPoint.y += translation.y;
        }
    }


    // Here it's more convenient to store the edge in slope-intercept form.
    struct ActiveEdge
    {
        F32 lowermostPoint1;
        F32 uppermostPoint1;
        // The zeroth component of the intersection of the line passing through the edge's vertices 
        // with the current scanline. By having this point, the slope and the first components of the 
        // edge's vertices we can restore their original positions as well as easily compute the zeroth
        // component of the intersection of the edge with a certain scanline.
        F32 scanlineTopIntersection0;
        // The derivative with respect to the zeroth direction.
        F32 dxdy;
        F32 direction;

        ActiveEdge(F32 l1, F32 u1, F32 sti0, F32 dxdy, F32 dir) :
            lowermostPoint1(l1),
            uppermostPoint1(u1),
            scanlineTopIntersection0(sti0),
            dxdy(dxdy),
            direction(dir)
        {

        }
    };


    auto Activate(Array<ActiveEdge>& activeEdges, const Edge& edge, F32 scanlineTop) -> V
    {
        static constexpr F32 HORIZONTALITY_TOLERANCE  = (1.0f / F32(1 << 15)) * 4.0f;
        auto dx = edge.lowermostPoint.x - edge.uppermostPoint.x;
        auto dy = edge.lowermostPoint.y - edge.uppermostPoint.y;

        if (dy <= HORIZONTALITY_TOLERANCE)
        {
            // We dont want horizontal edges just drop them
        }
        else 
        {
            auto dxdy = dx / dy;

            auto scanlineTopIntersection0 = 
                edge.uppermostPoint.x + dxdy * (scanlineTop - edge.uppermostPoint.y);

            activeEdges.emplace_back
            (
                edge.lowermostPoint.y,
                edge.uppermostPoint.y,
                scanlineTopIntersection0,
                dxdy,
                edge.direction
            );
        }
    }

    auto AddActiveEdges
    (
        Array<ActiveEdge>& activeEdges,
        const Array<Edge>& edges,
        U32& edgesIdx,
        F32 scanlineBot,
        F32 scanlineTop
    )
    {
        // Add all the edges that have their uppermost point before the end of this scanline
        while (edgesIdx < edges.size() && edges[edgesIdx].uppermostPoint.y < scanlineBot)
        {
            Activate(activeEdges, edges[edgesIdx], scanlineTop);
            edgesIdx++;
        }
    }

    auto ProcessActiveEdge(const ActiveEdge& edge, Array<Point>& scanline, F32 scanlineBot, F32 scanlineTop)
    {
        // Now we need to find the highest point of the edge that is below the top of the scanline and 
        // the lowest point of the edge that is above the scanline bottom. In case that the edges go 
        // beyond the sanline the points we search for are intersections.
        Point highPoint;
        Point lowPoint;

        if (scanlineTop < edge.uppermostPoint1)
        {
            highPoint.x = 
                edge.scanlineTopIntersection0 + edge.dxdy * (edge.uppermostPoint1 - scanlineTop);
            highPoint.y = edge.uppermostPoint1;
        }
        else
        {
            // Here we know that since the actual edge is above the scanline, the 0th direction 
            // intersection has to be inside the surface since all edges are inside the surface
            highPoint.x = edge.scanlineTopIntersection0;
            highPoint.y = scanlineTop;
        }

        if (scanlineBot < edge.lowermostPoint1)
        {
            lowPoint.x = edge.scanlineTopIntersection0 + edge.dxdy;
            lowPoint.y = scanlineBot;
        }
        else 
        {
            lowPoint.x = 
                edge.scanlineTopIntersection0 + edge.dxdy * (edge.lowermostPoint1 - scanlineTop);
            lowPoint.y = edge.lowermostPoint1;
        }

        // Once we have clipped the edge to the boundaries of the scanline there are two relevant 
        // possibilities to look for namely wether or not high_point.0 < low_point.0. In the two cases
        // the computation of the area will look differently because we process the pixels of the 
        // scanline from left to right (increasing in the 0th direction) hence we will care about the 
        // 0th coordinate of the edge. 
        //
        // However we can avoid that by simply recognizing that flipping the clipped edge around it's 
        // vertical center won't change the unsigned area.

        auto dxdy = edge.dxdy;
        auto sign = edge.direction;

        if (highPoint.x > lowPoint.x)
        {
            Swap(lowPoint.x, highPoint.x);
            // The tangent also flips.
            dxdy = -dxdy;
        }

        auto startPixel = Max(0.0f, Floor(highPoint.x));
        auto startPixelIdx = U32(startPixel);
        auto endPixel = Ceil(lowPoint.x);
        auto height = lowPoint.y - highPoint.y;

        // Spans a single pixel and is trapezoid
        if (endPixel - startPixel <= 1.0f)
        {
            // We use startPixel + 1.0 instead of endPixel to handle vertical edges properly
            auto area = height * ((startPixel + 1.0 - lowPoint.x) + (startPixel + 1.0 - highPoint.x)) / 2.0;

            scanline[startPixelIdx].x += sign * area;

            startPixelIdx++;
            // It induces rectangles in all the other pixels on the right hence we put the height
            // times one (the width of the rectangles) in the next entry of the commulative sum 
            // array (second component of the scanline).
            if (startPixelIdx < scanline.size())
            {
                scanline[startPixelIdx].y += sign * height;
            }
        }
        else
        {
            // We need to find where the edge intersects the vertical pixel barier of the first
            // pixel containing it.

            auto width = startPixel + 1.0 - highPoint.x;
            auto dydx = 1.0 / dxdy;

            auto intersection1 = highPoint.y + width * dydx;
            auto height = intersection1 - highPoint.y;

            auto area = width * height / 2.0;

            scanline[startPixelIdx].x += sign * area;
            startPixelIdx++;

            auto endPixelIdx = U32(Round(endPixel - 1.0));

            while (startPixelIdx < endPixelIdx)
            {

                auto area = (height + height + dydx) / 2.0;

                scanline[startPixelIdx].x += sign * area;

                height += dydx;
                startPixelIdx++;
            }

            // This is a combination of a trapezoid and rectangle
            auto endWidthRect = endPixel - lowPoint.x;
            auto endWidthTrap = 1.0 - endWidthRect;
            auto endHeight = height + endWidthTrap * dydx;
            auto endPixelArea = (height + endHeight) / 2.0 * endWidthTrap + endHeight * endWidthRect;

            scanline[endPixelIdx].x += sign * endPixelArea;

            endPixelIdx++;

            if (endPixelIdx < scanline.size())
            {
                // All the remaining pixels are ocluded by rectangles with width 1 and height
                // end_height.
                scanline[endPixelIdx].y += sign * endHeight;
            }
        }
    }


    auto ProcessActiveEdges(const Array<ActiveEdge>& edges, Array<Point>& scanline, F32 scanlineBot, F32 scanlineTop)
    {
        for (auto& edge : edges)
        {
            ProcessActiveEdge(edge, scanline, scanlineBot, scanlineTop);
        }
    }

    auto PrepareActiveEdgesForNextScanline(Array<ActiveEdge>& edges)
    {
        for (auto& edge : edges)
        {
            // Find the value of x 1 unit further by adding the slope.
            edge.scanlineTopIntersection0 += edge.dxdy;
        }
    }


    auto PruneActiveEdges(Array<ActiveEdge>& activeEdges, F32 scanlineTop)
    {
        auto idx = 0;

        while (idx < activeEdges.size())
        {
            // We check if the edge is above the scanline in order to remove it
            if (activeEdges[idx].lowermostPoint1 <= scanlineTop)
            {
                Swap(activeEdges[idx], activeEdges.back());
                activeEdges.pop_back();
                idx--;
            }

            idx++;
        }
    }


    auto ClearScanline(Array<Point>& scanline) 
    {
        for (auto& pix : scanline)
        {
            pix.x = 0.f;
            pix.y = 0.f;
        }
    }


    auto DrawScanline(GrayScaleSurface& surface, const Array<Point>& scanline, U32 scanlineIdx)
    {
        F32 commulativeSum = 0.f;
        for (auto i = 0; i < surface.width; ++i)
        {
            commulativeSum += scanline[i].y;
            auto value = Max(Min((commulativeSum + scanline[i].x) * 255.f, 255.f), 0.f);
            surface.data[surface.width * scanlineIdx + i] = 255u - Byte(value);
        }
    }

    // This function implements the high level functionality of the rasterizing algorithm
    auto RasterizeEdges(Array<Edge>& edges, GrayScaleSurface& surface) 
    {
        surface.data.resize(surface.height * surface.width);

        // We store first component which represents the signed area of a pixel shadowed by an outline,
        // and then a second component that works as a commulative sum which will indicate that the 
        // area will be added to all the rest of the pixels on the right.
        Array<Point> scanline;
        scanline.resize(surface.width , Point(0.f, 0.f));

        // We keep a set of edges that are relevant for the current scanline each iteration
        Array<ActiveEdge> activeEdges;
        activeEdges.reserve(surface.width);

        auto edgesIdx = 0u;

        for (auto i = 0u; i < surface.height;++i) 
        {
            auto scanlineTop = F32(i);
            auto scanlineBot = F32(i + 1.f);

            // Remove edges that are not relevant for this scanline, hence no longer relevant in 
            // general.
            PruneActiveEdges(activeEdges, scanlineTop);
            // Add all the new edges that have become relevant for the scanline.
            AddActiveEdges(activeEdges, edges, edgesIdx, scanlineBot, scanlineTop);
            // Fill the scanline array according to the edges intersecting the scanline
            ProcessActiveEdges(activeEdges, scanline, scanlineBot, scanlineTop);
            // Fill the surface scanline according to the scanline array
            DrawScanline(surface, scanline, i);
            ClearScanline(scanline);
            // Transform the representation of the active edges so that they are convenient for
            // pruning and processing next scanline.
            PrepareActiveEdgesForNextScanline(activeEdges);
        }
    }

    auto Rasterize(const GlyphData& glyph_data, F32 scale) -> GrayScaleSurface
    {
        GrayScaleSurface surface;

        F32 minX = glyph_data.boundingBoxDiagonal.startPoint.x;
        F32 minY = glyph_data.boundingBoxDiagonal.startPoint.y;
        F32 maxX = glyph_data.boundingBoxDiagonal.endPoint.x;
        F32 maxY = glyph_data.boundingBoxDiagonal.endPoint.y;

        surface.width = Ceil((maxX - minX+ 1.0) * scale);
        surface.height = Ceil((maxY - minY + 1.0) * scale);

        // The next code can be confusing beause some operations are omitted due to mental algebra.
        // If S is the scaling matrix R is the reflection matrix tO is translation of the lowermost 
        // bounding box point to the origin, then tC is the translation of the resulting bounding box's
        // center to the origin and tSc is the translation of the scaled bounding box's lowermost point
        // to the origin then the final transformation T on point p is Tp = RS(p + tO + tC) + t_sc.
        // Now when we writeout the explicit form of the final translation vector of this affine 
        // transformation namely RStO + RStC + tsC some of the terms in the components will cencel 
        // out.
        // 
        // This operation is done only once per rasterization so it's irrelevant how efficient it is. 
        // I would've used more clear form but I've not implemented traits for 2d matrix algebra so I 
        // have to resort to this confusing componentwise form.

        auto tO = Point(-minX, -minY);
        auto tC1 = minY - maxY;

        auto translationVector = Point(scale * tO.x, -scale * (tO.y + tC1));

        auto edges = Linearize(glyph_data, scale);

        TransformEdgesToSurfaceSpace(edges, scale, translationVector);

        // Sort by the uppermost edges. Edges with uppermost points the higher up will be first.
        Sort(edges);

        RasterizeEdges(edges, surface);

        return surface;
    }

    auto RasterizeGlyph(const FontData& fontData, I32 codepoint, I32 height) -> GrayScaleSurface
    {
        auto expectedMaxHeight = F32(fontData.ascent) - fontData.descent;
        auto fontScale = height / expectedMaxHeight;

        auto glyph = fontData.FetchGlyphDataForCodepoint(codepoint);
        return Rasterize(glyph, fontScale);
    }
}

#endif