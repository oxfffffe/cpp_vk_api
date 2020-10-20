#include "../cmd_holder.hpp"

string cmd_holder::append_vkurl(const string& method) {
  return api_url + method + "?";
}

void cmd_holder::append_vkparams(map<string, string>& map) {
  map["access_token"] = access_token;
  map["v"]            = api_version;
}
