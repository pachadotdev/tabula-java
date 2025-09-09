#ifdef HAVE_POPPLER
#include <iostream>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <sstream>
#include <string>

static std::string jsonEscape(const std::string &s) {
  std::ostringstream o;
  for (char c : s) {
    switch (c) {
    case '"':
      o << "\\\"";
      break;
    case '\\':
      o << "\\\\";
      break;
    case '\b':
      o << "\\b";
      break;
    case '\f':
      o << "\\f";
      break;
    case '\n':
      o << "\\n";
      break;
    case '\r':
      o << "\\r";
      break;
    case '\t':
      o << "\\t";
      break;
    default:
      if ((unsigned char)c < 0x20) {
        o << "\\u" << std::hex << (int)c;
      } else
        o << c;
    }
  }
  return o.str();
}

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: poppler-example <pdf-file>" << std::endl;
    return 2;
  }
  std::string path = argv[1];
  std::unique_ptr<poppler::document> doc(
      poppler::document::load_from_file(path));
  if (!doc) {
    std::cerr << "Failed to open PDF" << std::endl;
    return 1;
  }
  int np = doc->pages();
  std::cout << "[";
  for (int i = 0; i < np; ++i) {
    std::unique_ptr<poppler::page> p(doc->create_page(i));
    if (!p)
      continue;
    auto text = p->text();
    poppler::byte_array ba = text.to_utf8();
    std::string s(ba.begin(), ba.end());
    if (i)
      std::cout << ",";
    std::cout << "{\"page\":" << (i + 1) << ",\"text\":\"" << jsonEscape(s)
              << "\"}";
  }
  std::cout << "]" << std::endl;
  return 0;
}

#else
#include <iostream>
int main() {
  std::cerr << "poppler-example built without Poppler support" << std::endl;
  return 2;
}
#endif
