#pragma once

#include "ICommand.hpp"

namespace bot
{
namespace command
{
class Picture final : public ICommand
{
public:
  std::string execute(const CommandParams&, const Dependencies&) override;
  std::string description() const override;
 ~Picture() = default;
};
} //namespace command
} //namespace bot
