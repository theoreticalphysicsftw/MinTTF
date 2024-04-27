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
        auto bytes = (Byte*) &x;

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
        *((U16*)&result) = *uXPtr << 8 | (*uXPtr) >> 8;
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

        auto Load(StrView path) -> Error;
        auto Load(Span<const U8>) -> Error;
        auto GetCharIndex(I32 codepoint) const -> U32;
        auto FetchGlyphDataForCodepoint(I32 codepoint) -> GlyphData;
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

namespace MTTF
{
    auto FontData::LoadContour(GlyphData& data, Span<const TTFPoint> vertices, Span<const U8> flags, U64 sidx, U64 eidx) -> U64
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


    auto FontData::Load(StrView path) -> Error
    {
        return Error();
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


    auto FontData::FetchGlyphDataForCodepoint(I32 codepoint) -> GlyphData
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
        return U32();
    }


    auto FontData::GetCharIndexFmt6(U32 codepoint) const -> U32
    {
        return U32();
    }


    auto FontData::GetCharIndexFmt12(U32 codepoint) const -> U32
    {
        return U32();
    }


    auto FontData::GetGlyphOffset(U32 glyphIndex) const -> U32
    {
        return U32();
    }


    auto FontData::FetchGlyphData(U32 glyphIndex) -> GlyphData
    {
        return GlyphData();
    }
}

#endif