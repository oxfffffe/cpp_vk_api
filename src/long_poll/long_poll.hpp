#pragma once

#include "../cmd_handler/cmd_handler.hpp"

using handler::Cmd_handler;

class Lp {
private:
  void        loop         ();
  void        get_lp_server();
  void        errors_handle(long error_code);
  std::string server;
  std::string key;
  std::string ts;
  Logger      _logger{logfile, errfile};
  Cmd_handler handler;

public:
  void init_bot();
};
