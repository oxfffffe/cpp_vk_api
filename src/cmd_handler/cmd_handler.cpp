#include "./cmd_handler.hpp"

using namespace bot;

uint8_t const user      = 0x00;
uint8_t const moderator = 0x01;
uint8_t const creator   = 0x10;

vector<string> bot::words_from_file(const string& filename) {
  std::ifstream file(filename);
  vector<string> words;
  string word;
  while (file >> word) {
    words.push_back(word);
  }
  return words;
}

string bot::utf8_to_lower(const string& text) {
  wstring wide_string = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().from_bytes(text);
  std::transform(
    wide_string.begin(),
    wide_string.end(),
    wide_string.begin(),
    std::bind2nd(std::ptr_fun(&std::tolower<wchar_t>), std::locale(""))
  );
  return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(wide_string);
}

void Vk_cmd_handler::init_roles() {
  moderators = database.get_by_role(_handler->_peer_id, "модератор");
  blacklist = database.get_by_role(_handler->_peer_id, "мут");
}

void Vk_cmd_handler::init_conservations() {
  conservations = database.get_peer_ids();
}

void Vk_cmd_handler::init_words_blacklist() {
  words_blacklist = words_from_file(word_blacklist);
}

cmds_t const bot::vk_cmds =
{
  { "+помощь",  { "показать помощь",                         &Cmds::help_cmd,        user } },
  { "+стат",    { "показать статистику бота",                &Cmds::stat_cmd,        user } },
  { "+crc32",   { "сгенерить CRC-32 хеш-сумму строки",       &Cmds::crc32_cmd,       user } },
  { "+пикча",   { "найти картинку среди просторов ВК",       &Cmds::picture_cmd,     user } },
  { "+доки",    { "поиск документов",                        &Cmds::document_cmd,    user } },
  { "+видео",   { "поиск видеозаписей",                      &Cmds::video_cmd,       user } },
  { "+погода",  { "показать погоду",                         &Cmds::weather_cmd,     user } },
  { "+пинг",    { "проверить время ответа",                  &Cmds::ping_cmd,        user } },
  { "+вики",    { "поиск статьи в Википедии",                &Cmds::wiki_cmd,        user } },
  { "+смех",    { "смех, параметр -s - количество символов", &Cmds::laugh_cmd,       user } },
  { "+курс",    { "показать курс валют",                     &Cmds::currency_cmd,    user } },
  { "+оботе",   { "показать информацию о боте",              &Cmds::about_cmd,       user } },
  { "+роли",    { "посмотреть роли участников",              &Cmds::get_roles_cmd,   user } },
  { "+дополни", { "закончить текст",                         &Cmds::complete_cmd,    user } },
  { "+гитхаб",  { "различная инфа о юзерах с GitHub",        &Cmds::github_info_cmd, user } },
  { "+трек",    { "получить ссылки на треки из Genius",      &Cmds::genius_cmd,      user } },
  { "+гугл",    { "редирект сложнейшено вопроса в гугл",     &Cmds::google_cmd,      user } },
  { "+реверс",  { "перевернуть строку(модератор)",           &Cmds::reverse_cmd,     moderator } },
  { "+онлайн",  { "показать юзеров онлайн(модератор)",       &Cmds::online_cmd,      moderator } },
  { "+кик",     { "кикнуть юзера(модератор)",                &Cmds::kick_cmd,        moderator } },
  { "+мут",     { "ограничить доступ к командам бота",       &Cmds::blacklist_cmd,   moderator } },
  { "+запрети", { "добавить слово в список запрещённых",     &Cmds::forbid_word_cmd, moderator } },
  { "+роль",    { "установить роль участника(модератор,...)",&Cmds::role_cmd,        creator } }, ///< костыль
  { "+!",       { "(админ)повтор текста",                    &Cmds::repeat_cmd,      creator } },
  { "+os",      { "(админ)выполнить команду bash",           &Cmds::os_cmd,          creator } },
};

template <class T>
bool bot::any_of(vector<T>& vec, T id) {
  return std::find(vec.begin(), vec.end(), id) != vec.end();
}

bool bot::exists(const json& object, const string& key) {
    return object.find(key) != object.end();
}

void Cmds::init_cmds(const json& update)
{
  if (update["type"] == "wall_post_new") {
    new_post_event(
      update["object"]["from_id"].get<long>(),
      update["object"]["id"].get<long>()
    );
    return;
  }

  if (update["object"]["message"]["text"] == "") {
    return;
  }

  if (exists(update["object"]["message"], "reply_message")) {
    _reply = update["object"]["message"]["reply_message"];
  }

  if (update["type"] == "message_new") {
    json event = update["object"]["message"];
    _message = event["text"];
    _peer_id = event["peer_id"];
    _from_id = event["from_id"];

    if (_message.at(0) == '+') {
      _vk_handler.logger.write_log(_message);
      ++_msg_counter;
    }
  }
  _args    = split(_message);
  _vk_handler.init_roles();


  for (auto word : _args) {
    if (any_of<string>(_vk_handler.words_blacklist, word)) {
      return;
    }
  }
  for (auto cmd : vk_cmds) {
    if (std::get<uint8_t>(cmd.second) == creator and _from_id != creator_id) {
      continue;
    }
    if (std::get<uint8_t>(cmd.second) == moderator and not any_of(_vk_handler.moderators, _from_id)) {
      continue;
    }
    if (any_of(_vk_handler.blacklist, _from_id)) {
      continue;
    }
    if (cmd.first == split(_message)[0]) {
      _vk_handler.message_send((this->*std::get<string(Cmds::*)()>(cmd.second))());
    }
  }
}

void Cmds::stress_test(const string& peer_id) {
  string vkurl = append_vkurl("messages.send");
  params body;

  vector<string> cmd = {
    "+crc32",
    "+пикча",
    "+видео",
    "+доки",
    "+погода",
    "+дополни",
    "+реверс",
    "+вики",
  };
  vector<string> words = {
    "тест",
    "строка",
    "123",
    "картинка",
    "еда",
    "рисунок",
    "акробат",
    "хештег",
    "бот",
    "машина",
    "000",
    "инста",
  };

  body["message"] = cmd[rand() % cmd.size()] + ' ' + words[rand() % words.size()];
  body["peer_id"] = peer_id;
  body["disable_mentions"] = "1";
  body["access_token"] = stress_test_token;
  body["random_id"] = "0";
  body["v"] = api_version;
  request(vkurl, body);
}
