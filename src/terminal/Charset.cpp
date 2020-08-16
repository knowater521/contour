/**
 * This file is part of the "libterminal" project
 *   Copyright (c) 2019-2020 Christian Parpart <christian@parpart.family>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <terminal/Charset.h>

namespace terminal {

constexpr CharsetMap usasciiCharset()
{
    CharsetMap result{};

    for (char ch = 0; ch < 127; ++ch)
        result[static_cast<size_t>(ch)] = static_cast<char32_t>(ch);

    return result;
}

/// British:
///     ESC (A
///     Reference: http://vt100.net/docs/vt220-rm/table2-5.html
constexpr CharsetMap createBritishCharset()
{
    auto result = usasciiCharset();
    result['#'] = 0x00A3; // U'£';
    return result;
}

/// German:
///     ESC ( K
constexpr CharsetMap createGermanCharset()
{
    auto result = usasciiCharset();

    result[static_cast<size_t>('@')] = 0x00A7; // U'§';
    result[static_cast<size_t>('[')] = 0x00C4; // U'Ä';
    result[static_cast<size_t>('\\')] = 0x00D6; // U'Ö';
    result[static_cast<size_t>(']')] = 0x00DC; // U'Ü';
    result[static_cast<size_t>('{')] = 0x00E4; // U'ä';
    result[static_cast<size_t>('|')] = 0x00F6; // U'ö';
    result[static_cast<size_t>('}')] = 0x00FC; // U'ü';
    result[static_cast<size_t>('~')] = 0x00DF; //U'ß';

    return result;
}

/// DEC Special Character and Line Drawing Set.
///
/// Reference: http://vt100.net/docs/vt102-ug/table5-13.html
constexpr CharsetMap createSpecialCharset()
{
    auto result = usasciiCharset();

    result[static_cast<size_t>('`')] = 0x25c6; // '◆'
    result[static_cast<size_t>('a')] = 0x2592; // '▒'
    result[static_cast<size_t>('b')] = 0x2409; // '␉'
    result[static_cast<size_t>('c')] = 0x240c; // '␌'
    result[static_cast<size_t>('d')] = 0x240d; // '␍'
    result[static_cast<size_t>('e')] = 0x240a; // '␊'
    result[static_cast<size_t>('f')] = 0x00b0; // '°'
    result[static_cast<size_t>('g')] = 0x00b1; // '±'
    result[static_cast<size_t>('h')] = 0x2424; // '␤'
    result[static_cast<size_t>('i')] = 0x240b; // '␋'
    result[static_cast<size_t>('j')] = 0x2518; // '┘'
    result[static_cast<size_t>('k')] = 0x2510; // '┐'
    result[static_cast<size_t>('l')] = 0x250c; // '┌'
    result[static_cast<size_t>('m')] = 0x2514; // '└'
    result[static_cast<size_t>('n')] = 0x253c; // '┼'
    result[static_cast<size_t>('o')] = 0x23ba; // '⎺'
    result[static_cast<size_t>('p')] = 0x23bb; // '⎻'
    result[static_cast<size_t>('q')] = 0x2500; // '─'
    result[static_cast<size_t>('r')] = 0x23bc; // '⎼'
    result[static_cast<size_t>('s')] = 0x23bd; // '⎽'
    result[static_cast<size_t>('t')] = 0x251c; // '├'
    result[static_cast<size_t>('u')] = 0x2524; // '┤'
    result[static_cast<size_t>('v')] = 0x2534; // '┴'
    result[static_cast<size_t>('w')] = 0x252c; // '┬'
    result[static_cast<size_t>('x')] = 0x2502; // '│'
    result[static_cast<size_t>('y')] = 0x2264; // '≤'
    result[static_cast<size_t>('z')] = 0x2265; // '≥'
    result[static_cast<size_t>('{')] = 0x03c0; // 'π'
    result[static_cast<size_t>('|')] = 0x2260; // '≠'
    result[static_cast<size_t>('}')] = 0x00a3; // '£'
    result[static_cast<size_t>('~')] = 0x00b7; // '·'

    return result;
}

/// Dutch:
///     ESC ( 4
///
constexpr CharsetMap createDutchCharset()
{
    auto result = usasciiCharset();

    result[static_cast<size_t>('#')] = 0x00A3; // U'£';
    result[static_cast<size_t>('@')] = 0x00BE; // U'¾';
    //TODO: result[static_cast<size_t>('[')] = U'ij';
    result[static_cast<size_t>('\\')] = 0x00BD; // U'½';
    result[static_cast<size_t>(']')] = 0x007C; // U'|';
    result[static_cast<size_t>('{')] = 0x00A8; // U'¨';
    result[static_cast<size_t>('|')] = 0x0066; // U'f';
    result[static_cast<size_t>('}')] = 0x00BC; // U'¼';
    result[static_cast<size_t>('~')] = 0x00B4; // U'´';

    return result;
}

/// Finish:
///     ESC ( C
///     ESC ( 5
constexpr CharsetMap createFinishCharset()
{
    auto result = usasciiCharset();

    result[static_cast<size_t>('[')] = 0x00C4; // U'Ä';
    result[static_cast<size_t>('\\')] = 0x00D6; // U'Ö';
    result[static_cast<size_t>(']')] = 0x00C5; // U'Å';
    result[static_cast<size_t>('^')] = 0x00DC; // U'Ü';
    result[static_cast<size_t>('`')] = 0x00E9; // U'é';
    result[static_cast<size_t>('{')] = 0x00E4; // U'ä';
    result[static_cast<size_t>('|')] = 0x00F6; // U'ö';
    result[static_cast<size_t>('}')] = 0x00E5; // U'å';
    result[static_cast<size_t>('~')] = 0x00FC; // U'ü';

    return result;
}

/// French:
///     ESC ( R
constexpr CharsetMap createFrenchCharset()
{
    auto result = usasciiCharset();

    result[static_cast<size_t>('#')] = 0x00A3; // U'£';
    result[static_cast<size_t>('@')] = 0x00E0; // U'à';
    result[static_cast<size_t>('[')] = 0x00B0; // U'°';
    result[static_cast<size_t>('\\')] = 0x00E7; // U'ç';
    result[static_cast<size_t>(']')] = 0x00A7; // U'§';
    result[static_cast<size_t>('{')] = 0x00E9; // U'é';
    result[static_cast<size_t>('|')] = 0x00F9; // U'ù';
    result[static_cast<size_t>('}')] = 0x00E8; // U'è';
    result[static_cast<size_t>('~')] = 0x00A8; // U'¨';

    return result;
}

/// French Canadian:
///     ESC ( Q
constexpr CharsetMap createFrenchCanadianCharset()
{
    auto result = usasciiCharset();

    result[static_cast<size_t>('@')] = 0x00E0; // U'à';
    result[static_cast<size_t>('[')] = 0x00E2; // U'â';
    result[static_cast<size_t>('\\')] = 0x00E7; // U'ç';
    result[static_cast<size_t>(']')] = 0x00EA; // U'ê';
    result[static_cast<size_t>('^')] = 0x00EE; // U'î';
    result[static_cast<size_t>('`')] = 0x00F4; // U'ô';
    result[static_cast<size_t>('{')] = 0x00E9; // U'é';
    result[static_cast<size_t>('|')] = 0x00F9; // U'ù';
    result[static_cast<size_t>('}')] = 0x00E8; // U'è';
    result[static_cast<size_t>('~')] = 0x00FB; // U'û';

    return result;
}

/// Norwegian/Danich:
///     ESC ( E
///     ESC ( 6
constexpr CharsetMap createNorwegianDanishCharset()
{
    auto result = usasciiCharset();

    result[static_cast<size_t>('@')] = 0x00C4; // U'Ä';
    result[static_cast<size_t>('[')] = 0x00C6; // U'Æ';
    result[static_cast<size_t>('\\')] = 0x00D8; // U'Ø';
    result[static_cast<size_t>(']')] = 0x00C5; // U'Å';
    result[static_cast<size_t>('^')] = 0x00DC; // U'Ü';
    result[static_cast<size_t>('`')] = 0x00E4; // U'ä';
    result[static_cast<size_t>('{')] = 0x00E6; // U'æ';
    result[static_cast<size_t>('|')] = 0x00F8; // U'ø';
    result[static_cast<size_t>('}')] = 0x00E5; // U'å';
    result[static_cast<size_t>('~')] = 0x00FC; // U'ü';

    return result;
}

/// Spanish:
///     ESC ( Z
constexpr CharsetMap createSpanishCharset()
{
    auto result = usasciiCharset();

    result[static_cast<size_t>('#')] = 0x00A3; // U'£';
    result[static_cast<size_t>('@')] = 0x00A7; // U'§';
    result[static_cast<size_t>('[')] = 0x00A1; // U'¡';
    result[static_cast<size_t>('\\')] = 0x00D1; // U'Ñ';
    result[static_cast<size_t>(']')] = 0x00BF; // U'¿';
    result[static_cast<size_t>('{')] = 0x00B0; // U'°';
    result[static_cast<size_t>('|')] = 0x00F1; // U'ñ';
    result[static_cast<size_t>('}')] = 0x00E7; // U'ç';

    return result;
}

/// Swedish:
///     ESC ( H
///     ESC ( 7
constexpr CharsetMap createSwedishCharset()
{
    auto result = usasciiCharset();

    result[static_cast<size_t>('@')] = 0x00C9; // U'É';
    result[static_cast<size_t>('[')] = 0x00C4; // U'Ä';
    result[static_cast<size_t>('\\')] = 0x00D6; // U'Ö';
    result[static_cast<size_t>(']')] = 0x00C5; // U'Å';
    result[static_cast<size_t>('^')] = 0x00DC; // U'Ü';
    result[static_cast<size_t>('`')] = 0x00E9; // U'é';
    result[static_cast<size_t>('{')] = 0x00E4; // U'ä';
    result[static_cast<size_t>('|')] = 0x00F6; // U'ö';
    result[static_cast<size_t>('}')] = 0x00E5; // U'å';
    result[static_cast<size_t>('~')] = 0x00FC; // U'ü';

    return result;
}

/// Swiss:
///     ESC ( =
constexpr CharsetMap createSwissCharset()
{
    auto result = usasciiCharset();

    result[static_cast<size_t>('#')] = 0x00F9; // U'ù';
    result[static_cast<size_t>('@')] = 0x00E0; // U'à';
    result[static_cast<size_t>('[')] = 0x00E9; // U'é';
    result[static_cast<size_t>('\\')] = 0x00E7; // U'ç';
    result[static_cast<size_t>(']')] = 0x00EA; // U'ê';
    result[static_cast<size_t>('^')] = 0x00EE; // U'î';
    result[static_cast<size_t>('_')] = 0x00E8; // U'è';
    result[static_cast<size_t>('`')] = 0x00F4; // U'ô';
    result[static_cast<size_t>('{')] = 0x00E4; // U'ä';
    result[static_cast<size_t>('|')] = 0x00F6; // U'ö';
    result[static_cast<size_t>('}')] = 0x00FC; // U'ü';
    result[static_cast<size_t>('~')] = 0x00FB; // U'û';

    return result;
}

CharsetMap const* charsetMap(CharsetId _id) noexcept
{
    static auto const british = createBritishCharset();
    static auto const dutch = createDutchCharset();
    static auto const finish = createFinishCharset();
    static auto const french = createFrenchCharset();
    static auto const frenchCanadian = createFrenchCanadianCharset();
    static auto const german = createGermanCharset();
    static auto const norweigianDanish = createNorwegianDanishCharset();
    static auto const spanish = createSpanishCharset();
    static auto const special = createSpecialCharset();
    static auto const swedish = createSwedishCharset();
    static auto const swiss = createSwissCharset();
    static auto const usascii = usasciiCharset();

    switch (_id)
    {
        case CharsetId::British: return &british;
        case CharsetId::Dutch: return &dutch;
        case CharsetId::Finish: return &finish;
        case CharsetId::French: return &french;
        case CharsetId::FrenchCanadian: return &frenchCanadian;
        case CharsetId::German: return &german;
        case CharsetId::NorwegianDanish: return &norweigianDanish;
        case CharsetId::Spanish: return &spanish;
        case CharsetId::Special: return &special;
        case CharsetId::Swedish: return &swedish;
        case CharsetId::Swiss: return &swiss;
        case CharsetId::USASCII: return &usascii;
    }

    return nullptr;
}

} // end terminal
