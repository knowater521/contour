/**
 * This file is part of the "libterminal" project
 *   Copyright (c) 2019 Christian Parpart <christian@parpart.family>
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
#include <terminal/Screen.h>
#include <terminal/Selector.h>
#include <catch2/catch.hpp>

using namespace std;
using namespace std::placeholders;
using namespace terminal;

TEST_CASE("Selector.Linear")
{
    auto screen = Screen{{5, 5}, [&](auto const& msg) { INFO(fmt::format("{}", msg)); }};
    screen.write("12 45\r\n678 0\r\nA CDE\r\nFGHIJ\r\nKLMNO");

	SECTION("forward single-line") {
		auto selector = Selector{
			Selector::Mode::Linear,
			bind(&Screen::absoluteAt, screen, _1),
			U" ,",
			screen.size().rows + static_cast<cursor_pos_t>(screen.historyLineCount()),
			screen.size(),
			Coordinate{2, 2}
		};
		selector.extend(Coordinate{2, 4});
		// TODO
	}
}

TEST_CASE("Selector.LinearWordWise")
{
	// TODO
}

TEST_CASE("Selector.FullLine")
{
	// TODO
}

TEST_CASE("Selector.Rectangular")
{
	// TODO
}