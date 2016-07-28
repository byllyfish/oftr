#include "ofp/yaml/getjson.h"

static void scanDoubleQuotes(std::istream &input, std::string &json,
                             int &newlineCount) {
  char ch;

  while (input.get(ch)) {
    json.push_back(ch);

    if (ch == '"') {
      return;
    }

    if (ch == '\\' && input.get(ch)) {
      json.push_back(ch);
    } else if (ch == '\n') {
      ++newlineCount;
    }
  }
}

static void scanObject(std::istream &input, std::string &json,
                       int &newlineCount) {
  int brace = 1;

  char ch;
  while (input.get(ch)) {
    json.push_back(ch);

    switch (ch) {
      case '{':
        ++brace;
        break;
      case '}':
        if (--brace == 0) {
          return;
        }
        break;
      case '"':
        scanDoubleQuotes(input, json, newlineCount);
        break;
      case '\n':
        ++newlineCount;
        break;
      default:
        break;
    }
  }
}

bool ofp::yaml::getjson(std::istream &input, std::string &json, int &lineNum,
                        int &newlineCount) {
  json.clear();
  lineNum = -1;

  char ch;
  while (input.get(ch)) {
    if (ch == '{') {
      json.push_back(ch);
      lineNum = newlineCount + 1;
      scanObject(input, json, newlineCount);
      return true;

    } else if (ch == '\n') {
      ++newlineCount;
    }
  }

  return false;
}

static bool isEmptyOrWhitespaceOnly(std::string &s) {
  return std::find_if(s.begin(), s.end(),
                      [](char ch) { return !isspace(ch); }) == s.end();
}

static void chomp(std::string &s) {
  if (!s.empty() && s.back() == '\r') {
    s.erase(s.size() - 1);
  }
}

bool ofp::yaml::getyaml(std::istream &input, std::string &yaml, int &lineNum,
                        int &newlineCount) {
  int msgSize = 0;
  std::string lineBuf;

  yaml.clear();

  while (std::getline(input, lineBuf)) {
    chomp(lineBuf);
    ++newlineCount;

    if (lineBuf == "---" || lineBuf == "...") {
      if (isEmptyOrWhitespaceOnly(yaml)) {
        // Don't return empty messages.
        msgSize = 0;
        yaml.clear();
        continue;
      }
      lineNum = newlineCount - msgSize;
      return true;
    }
    yaml.append(lineBuf);
    yaml.push_back('\n');
    ++msgSize;
  }

  if (isEmptyOrWhitespaceOnly(yaml)) {
    lineNum = -1;
    return false;
  }

  lineNum = newlineCount - msgSize + 1;

  return true;
}

bool ofp::yaml::getline(std::istream &input, std::string &line, int &lineNum,
                        int &newlineCount) {
  std::string lineBuf;

  while (std::getline(input, lineBuf)) {
    chomp(lineBuf);
    ++newlineCount;

    if (!isEmptyOrWhitespaceOnly(lineBuf)) {
      line = lineBuf;
      lineNum = newlineCount;
      return true;
    }
  }

  lineNum = -1;
  line.clear();

  return false;
}
