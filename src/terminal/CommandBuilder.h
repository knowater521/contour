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
#pragma once

#include <terminal/Logger.h>
#include <terminal/Parser.h>
#include <terminal/Functions.h>
#include <terminal/Commands.h>

#include <string>
#include <string_view>
#include <vector>

namespace terminal {

enum class ApplyResult {
    Ok,
    Invalid,
    Unsupported,
};

/// Applies a FunctionDefinition to a given context, emitting the respective command.
///
/// A FunctionSelector must have been transformed into a FunctionDefinition already.
/// So the idea is:
///     VT sequence -> FunctionSelector -> FunctionDefinition -> Command.
ApplyResult apply(FunctionDefinition const& _function, Sequence const& _context, CommandList& _output);

/// Takes parser events as input, assembles them into Sequence objects and then produces
/// a sequence of Command objects.
///
/// This is basically applying semantic validation on the input sequences and constructing the
/// appropriate commands.
class CommandBuilder {
  public:
    using ActionClass = parser::ActionClass;
    using Action = parser::Action;

    /// Constructs the sequencer stage.
    ///
    /// @param _logger the logging object to be used when logging is needed.
    /// @param _commandBuilder the next stage in the pipeline to feed the Sequence objects to.
    explicit CommandBuilder(Logger _logger);

    CommandList const& commands() const noexcept { return commands_; }
    CommandList& commands() noexcept { return commands_; }

    void clear() { commands_.clear(); }

    void operator()(ActionClass _actionClass, Action _action, char32_t _finalChar)
    {
        return handleAction(_actionClass, _action, _finalChar);
    }

    // helper methods
    //
    std::optional<RGBColor> static parseColor(std::string_view const& _value);

  private:
    void handleAction(ActionClass _actionClass, Action _action, char32_t _finalChar);
    void executeControlFunction(char _c0);
    void dispatchESC(char _finalChar);
    void dispatchCSI(char _finalChar);
    void dispatchOSC();
    void emitSequence();

    template <typename Event, typename... Args>
    void log(Args&&... args) const
    {
        if (logger_)
            logger_(Event{ std::forward<Args>(args)... });
    }

    template <typename T, typename... Args>
    ApplyResult emitCommand(Args&&... args)
    {
        commands_.emplace_back(T{std::forward<Args>(args)...});
        // TODO: telemetry_.increment(...);
        return ApplyResult::Ok;
    }

  private:
    Sequence sequence_{};
    CommandList commands_{};
    Logger const logger_;
};

}  // namespace terminal

