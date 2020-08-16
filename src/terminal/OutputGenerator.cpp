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
#include <terminal/Color.h>
#include <terminal/OutputGenerator.h>

#include <crispy/base64.h>
#include <crispy/overloaded.h>
#include <crispy/times.h>

#include <fmt/format.h>

#include <array>
#include <fstream>
#include <numeric>
#include <string>

using namespace std;

using crispy::times;

namespace terminal {

OutputGenerator::~OutputGenerator()
{
    flush();
}

void OutputGenerator::operator()(std::vector<Command> const& commands)
{
    for (Command const& command : commands)
        (*this)(command);
}

constexpr optional<char> gnumber(CharsetTable table, CharsetId charset)
{
    array<char, 4> const std = {'(', ')', '*', '+'};

    switch (charset)
    {
        case CharsetId::Special:
        case CharsetId::British:
        case CharsetId::USASCII:
        case CharsetId::German:
            return {std.at(static_cast<size_t>(table))};
        // TODO: other cases
    }
    return nullopt;
}

optional<char> finalChar(CharsetId charset)
{
    switch (charset)
    {
        case CharsetId::Special:
            return {'0'};
        case CharsetId::British:
            return {'A'};
        case CharsetId::USASCII:
            return {'B'};
        case CharsetId::German:
            return {'K'};
        // TODO: other cases
    }
    return nullopt;
}

string OutputGenerator::flush(vector<unsigned> const& _sgr)
{
    if (_sgr.empty())
        return "";

    auto const params =
        _sgr.size() != 1 || _sgr[0] != 0
            ? accumulate(begin(_sgr), end(_sgr), string{},
                         [](auto a, auto b) {
                             return a.empty() ? fmt::format("{}", b) : fmt::format("{};{}", a, b);
                         })
            : string();

    return fmt::format("\033[{}m", params);
}

void OutputGenerator::flush()
{
    if (!sgr_.empty())
    {
        auto const f = flush(sgr_);
        sgr_.clear();
        writer_(f.data(), f.size());
    }
}

void OutputGenerator::sgr_add(unsigned n)
{
    if (n == 0)
    {
        sgr_.clear();
        sgr_.push_back(n);
    }
    else
    {
        if (sgr_.empty() || sgr_.back() != n)
            sgr_.push_back(n);

        if (sgr_.size() == 16)
        {
            write(flush(sgr_));
            sgr_.clear();
        }
    }
}

void OutputGenerator::operator()(Command const& command)
{
    auto const pairOrNone = [](size_t _default, size_t _a, size_t _b) -> string {
        if (_a == _default && _b == _default)
            return "";
        else if (_a == _default)
            return fmt::format(";{}", _b);
        else if (_b == _default)
            return fmt::format("{};", _a);
        else
            return fmt::format("{};{}", _a, _b);
    };

    visit(overloaded{
        [&](Bell) { write("\x07"); },
        [&](Linefeed) { write("\n"); },
        [&](Backspace) { write("\x08"); },
        [&](FullReset) { write("\033c"); },
        [&](DeviceStatusReport) { write("\033[5n"); },
        [&](ReportCursorPosition) { write("\033[6n"); },
        [&](ReportExtendedCursorPosition) { write("\033[?6n"); },
        [&](SelectConformanceLevel const& v) {
            auto level = 61;
            switch (v.level) {
                case VTType::VT525:
                case VTType::VT520:
                case VTType::VT510: level = 65; break;
                case VTType::VT420: level = 64; break;
                case VTType::VT340:
                case VTType::VT330:
                case VTType::VT320: level = 63; break;
                case VTType::VT240:
                case VTType::VT220: level = 62; break;
                case VTType::VT100: level = 61; break;
                break;
            }
            auto const c1t = v.c1t == ControlTransmissionMode::S8C1T ? 0 : 1;
            write("\033[{};{}\"p", level, c1t);
        },
        [&](SendDeviceAttributes) { write("\033[c"); }, // Primary DA
        [&](SendTerminalId) { write("\033[>c"); }, // FIXME: this is Secondary DA!
        [&](ClearToEndOfScreen) { write("\033[0J"); },
        [&](ClearToBeginOfScreen) { write("\033[1J"); },
        [&](ClearScreen) { write("\033[2J"); },
        [&](ClearScrollbackBuffer) { write("\033[3J"); },
        [&](EraseCharacters const& v) { write("\033[{}X", v.n); },
        [&](ScrollUp const& up) { write("\033[{}S", up.n); },
        [&](ScrollDown const& down) { write("\033[{}T", down.n); },
        [&](CopyToClipboard const& v) { write("\033]42;{}\033\\", crispy::base64::encode(v.data)); },
        [&](ClearToEndOfLine) { write("\033[K"); },
        [&](ClearToBeginOfLine) { write("\033[1K"); },
        [&](ClearLine) { write("\033[2K"); },
        [&](CursorNextLine const& v) { write("\033[{}E", v.n); },
        [&](CursorPreviousLine const& v) { write("\033[{}F", v.n); },
        [&](InsertCharacters const& v) { write("\033[{}@", v.n); },
        [&](InsertColumns const& v) { write("\033[{}'}", v.n); },
        [&](InsertLines const& lines) { write("\033[{}L", lines.n); },
        [&](DeleteLines const& lines) { write("\033[{}M", lines.n); },
        [&](DeleteCharacters const& chars) { write("\033[{}P", chars.n); },
        [&](DeleteColumns const& cols) { write("\033[{}'~", cols.n); },
        [&](HorizontalPositionAbsolute const& v) { write("\033[{}`", v.n); },
        [&](HorizontalPositionRelative const& v) { write("\033[{}a", v.n); },
        [&](HorizontalTabClear const& v) {
            switch (v.which)
            {
                case HorizontalTabClear::UnderCursor:
                    write("\033[g");
                    break;
                case HorizontalTabClear::AllTabs:
                    write("\033[3g");
                    break;
            }
        },
        [&](HorizontalTabSet) { write("\033H"); },
        [&](Hyperlink const& v) {
            if (v.id.empty())
                write("\033]8;;{}\033\\", v.uri);
            else
                write("\033]8;id={};{}\033\\", v.id, v.uri);
        },
        [&](MoveCursorUp const& up) {
            if (normalCursorKeys())
                write("\033[{}A", up.n);
            else
                times(up.n) | [&]() { write("\033OA"); };
        },
        [&](MoveCursorDown const& down) {
            if (normalCursorKeys())
                write("\033[{}B", down.n);
            else
                times(down.n) | [&]() { write("\033OB"); };
        },
        [&](MoveCursorForward const& fwd) { write("\033[{}C", fwd.n); },
        [&](MoveCursorBackward const& back) { write("\033[{}D", back.n); },
        [&](MoveCursorToColumn const& to) { write("\033[{}G", to.column); },
        [&](MoveCursorToBeginOfLine) { write("\r"); },
        [&](MoveCursorTo const& to) { write("\033[{}H", pairOrNone(1, to.row, to.column)); },
        [&](MoveCursorToLine const& to) { write("\033[{}d", to.row); },
        [&](MoveCursorToNextTab) { write("\t"); },
        [&](Notify const& v) { write("\033]777;notify;{};{}\033\\", v.title, v.content); },
        [&](CursorBackwardTab const& v) { write("\033[{}Z", v.count); },
        [&](SaveCursor) { write("\0337"); },
        [&](RestoreCursor) { write("\0338"); },
        [&](RequestDynamicColor const& v) { write("\033];?\x07", setDynamicColorCommand(v.name)); },
        [&](RequestTabStops const&) { write("\033[2$w"); },
        [&](SetDynamicColor const& v) { write("\033]{};{}\x07", setDynamicColorCommand(v.name), setDynamicColorValue(v.color)); },
        [&](DumpState const&) { write("\033]{}\x07", "888"); },
        [&](ResetDynamicColor const& v) { write("\033]{}\x07", resetDynamicColorCommand(v.name)); },
        [&](SetForegroundColor const& v) {
            if (true) // v.color != currentForegroundColor_)
            {
                currentForegroundColor_ = v.color;
                if (holds_alternative<IndexedColor>(v.color))
                {
                    auto const colorValue = get<IndexedColor>(v.color);
                    if (static_cast<unsigned>(colorValue) < 8)
                        sgr_add(30 + static_cast<unsigned>(colorValue));
                    else
                    {
                        sgr_add(38);
                        sgr_add(5);
                        sgr_add(static_cast<unsigned>(colorValue));
                    }
                }
                else if (holds_alternative<DefaultColor>(v.color))
                    sgr_add(39);
                else if (holds_alternative<BrightColor>(v.color))
                    sgr_add(90 + static_cast<unsigned>(get<BrightColor>(v.color)));
                else if (holds_alternative<RGBColor>(v.color))
                {
                    auto const& rgb = get<RGBColor>(v.color);
                    sgr_add(38);
                    sgr_add(2);
                    sgr_add(static_cast<unsigned>(rgb.red));
                    sgr_add(static_cast<unsigned>(rgb.green));
                    sgr_add(static_cast<unsigned>(rgb.blue));
                }
            }
        },
        [&](SetBackgroundColor const& v) {
            if (true)//v.color != currentBackgroundColor_)
            {
                currentBackgroundColor_ = v.color;
                if (holds_alternative<IndexedColor>(v.color))
                {
                    auto const colorValue = get<IndexedColor>(v.color);
                    if (static_cast<unsigned>(colorValue) < 8)
                        sgr_add(40 + static_cast<unsigned>(colorValue));
                    else
                    {
                        sgr_add(48);
                        sgr_add(5);
                        sgr_add(static_cast<unsigned>(colorValue));
                    }
                }
                else if (holds_alternative<DefaultColor>(v.color))
                    sgr_add(49);
                else if (holds_alternative<BrightColor>(v.color))
                    sgr_add(100 + static_cast<unsigned>(get<BrightColor>(v.color)));
                else if (holds_alternative<RGBColor>(v.color))
                {
                    auto const& rgb = get<RGBColor>(v.color);
                    sgr_add(48);
                    sgr_add(2);
                    sgr_add(static_cast<unsigned>(rgb.red));
                    sgr_add(static_cast<unsigned>(rgb.green));
                    sgr_add(static_cast<unsigned>(rgb.blue));
                }
            }
        },
        [&](SetUnderlineColor const& v) {
            if (v.color != currentUnderlineColor_)
            {
                currentUnderlineColor_ = v.color;
                if (holds_alternative<IndexedColor>(v.color))
                {
                    auto const colorValue = get<IndexedColor>(v.color);
                    sgr_add(58);
                    sgr_add(5);
                    sgr_add(static_cast<unsigned>(colorValue));
                }
                else if (holds_alternative<terminal::RGBColor>(v.color))
                {
                    auto const color = get<RGBColor>(v.color);
                    sgr_add(58);
                    sgr_add(2);
                    sgr_add(color.red);
                    sgr_add(color.green);
                    sgr_add(color.blue);
                }
                // TODO?
                // else if (holds_alternative<DefaultColor>(v.color))
                //     sgr_add(58);
                // else if (holds_alternative<BrightColor>(v.color))
                //     sgr_add(90 + static_cast<unsigned>(get<BrightColor>(v.color)));
            }
        },
		[&](SetCursorStyle cursorStyle) {
			switch (cursorStyle.display) {
				case CursorDisplay::Blink:
					switch (cursorStyle.shape) {
                        case CursorShape::Rectangle:
                        case CursorShape::Bar:
                            // not supported as CSI, defaulting to BLOCK
                            [[fallthrough]];
						case CursorShape::Block:
							write("\033[2 q");
							break;
						case CursorShape::Underscore:
							write("\033[4 q");
							break;
					}
					break;
				case CursorDisplay::Steady:
					switch (cursorStyle.shape) {
                        case CursorShape::Rectangle:
                        case CursorShape::Bar:
                            // not supported as CSI, defaulting to BLOCK
                            [[fallthrough]];
						case CursorShape::Block:
							write("\033[1 q");
							break;
						case CursorShape::Underscore:
							write("\033[3 q");
							break;
					}
					break;
			}
		},
        [&](SetMark) { write("\033[>M"); },
        [&](SetMode mode) { write("\033[{}{}", to_code(mode.mode), mode.enable ? 'h' : 'l'); },
        [&](RequestMode v) {
            if (isAnsiMode(v.mode))
                write("\033[{}$p", to_code(v.mode));
            else
                write("\033[?{}$p", to_code(v.mode));
        },
        [&](SetTopBottomMargin margin) {
			if (!margin.top.has_value() && !margin.bottom.has_value())
				write("\033[r");
			else if (!margin.bottom.has_value())
				write("\033[{}r", margin.top.value());
			else if (!margin.top.has_value())
				write("\033[;{}r", margin.bottom.value());
			else
				write("\033[{};{}r", margin.top.value(), margin.bottom.value());
		},
        [&](SetLeftRightMargin margin) {
			if (!margin.left && !margin.right)
				write("\033[s");
			else if (!margin.right)
				write("\033[{}s", *margin.left);
			else if (!margin.left)
				write("\033[;{}s", *margin.right);
			else
				write("\033[{};{}s", *margin.left, *margin.right);
		},
        [&](ScreenAlignmentPattern) { write("\033#8"); },
        [&](SendMouseEvents v) { write("\033[?{}{}", to_code(v.protocol), v.enable ? 'h' : 'l'); },
        [&](ApplicationKeypadMode v) { write("\033{}", v.enable ? '=' : '>'); },
        [&](Index) { write("\033D"); },
        [&](ReverseIndex) { write("\033M"); },
        [&](ForwardIndex) { write("\0339"); },
        [&](BackIndex) { write("\0336"); },
        [&](SetGraphicsRendition const& v) {
            // TODO: add context-aware caching to avoid double-setting
            sgr_add(static_cast<int>(v.rendition));
            if (v.rendition == GraphicsRendition::Reset)
            {
                currentForegroundColor_ = DefaultColor{};
                currentBackgroundColor_ = DefaultColor{};
            }
        },
        [&](DesignateCharset v) {
            if (auto g = gnumber(v.table, v.charset); g.has_value())
                if (auto f = finalChar(v.charset); f.has_value())
                    write("\033{}{}", *g, *f);
        },
        [&](SingleShiftSelect v) {
            switch (v.table)
            {
                case CharsetTable::G2:
                    write("\033N");
                    break;
                case CharsetTable::G3:
                    write("\033O");
                    break;
                default:
                    break;
            }
        },
        [&](AppendChar const& v) { write(v.ch); },
        [&](ChangeIconTitle const& v) { write("\033]1;{}\033\\", v.title); },
        [&](ChangeWindowTitle const& v) { write("\033]2;{}\033\\", v.title); },
        [&](SoftTerminalReset) { write("\033[!p"); },
        [&](ResizeWindow const& v) {
            write("\033[{};{};{}t",
                v.unit == ResizeWindow::Unit::Pixels ? 4 : 8,
                v.height,
                v.width
            );
        },
        [&](SaveWindowTitle const&) { write("\033[22;0;0t"); },
        [&](RestoreWindowTitle const&) { write("\033[23;0;0t"); },
    }, command);
}

}  // namespace terminal
