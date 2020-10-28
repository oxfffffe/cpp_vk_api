#include "../cmd_handler/cmd_handler.hpp"

using namespace bot;

cmds_t bot::cmds =
{
  { "+помощь",     { "показать помощь",                   &Cmd_handler::help_cmd,      USER  } },
  { "+стат",       { "показать статистику бота",          &Cmd_handler::stat_cmd,      USER  } },
  { "+crc32",      { "сгенерить CRC-32 хеш-сумму строки", &Cmd_handler::crc32_cmd,     USER  } },
  { "+пикча",      { "найти картинку среди просторов ВК", &Cmd_handler::picture_cmd,   USER  } },
  { "+фото",       { "найти картинку среди просторов ВК", &Cmd_handler::picture_cmd,   USER  } },
  { "+документ",   { "поиск документов",                  &Cmd_handler::document_cmd,  USER  } },
  { "+доки",       { "поиск документов",                  &Cmd_handler::document_cmd,  USER  } },
  { "+видео",      { "поиск видеозаписей",                &Cmd_handler::video_cmd,     USER  } },
  { "+видос",      { "поиск видеозаписей",                &Cmd_handler::video_cmd,     USER  } },
  { "+погода",     { "показать погоду",                   &Cmd_handler::weather_cmd,   USER  } },
  { "+никнейм",    { "установить/удалить никнейм",        &Cmd_handler::nickname_cmd,  USER  } },
  { "+пинг",       { "проверить время ответа",            &Cmd_handler::ping_cmd,      USER  } },
  { "+вики",       { "поиск статьи в Википедии",          &Cmd_handler::wiki_cmd,      USER  } },
  { "+переводчик", { "перевести текст(сломано)",          &Cmd_handler::translate_cmd, USER  } },
  { "+os",         { "(админ)выполнить команду bash",     &Cmd_handler::os_cmd,        ADMIN } },
  { "+!",          { "(админ)повтор текста",              &Cmd_handler::repeat_cmd,    ADMIN } }
};

void Cmd_handler::init_cmds(
  const string& message,
  const long&   peer_id,
  const long&   from_id)
{
  _message          = message;
  _peer_id          = peer_id;
  _from_id          = from_id;
  _splitted_message = split(_message);
  _nickname         = _database.return_nickname(_from_id);

  if (_message.at(0) == '+') {
    _logger.write_log(_message);
  }

  for (auto cmd : cmds) {
    if (std::get<2>(cmd.second) == ADMIN and _from_id != admin_id) {
      continue;
    }
    if (cmd.first == split(_message)[0]) {
      (this->*std::get<void(Cmd_handler::*)()>(cmd.second))();
    }
  }
}
