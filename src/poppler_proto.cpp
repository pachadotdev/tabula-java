#ifdef HAVE_POPPLER
#include <iostream>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: poppler-proto <pdf-file>\n";
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
  std::cout << "Pages: " << np << std::endl;
  std::unique_ptr<poppler::page> p(doc->create_page(0));
  if (!p)
    return 1;
  auto text = p->text();
  // poppler::ustring -> utf8
  poppler::byte_array ba = text.to_utf8();
  std::string s(ba.begin(), ba.end());
  std::cout << s << std::endl;
  return 0;
}
#else
#include <iostream>
int main() {
  std::cerr << "poppler-proto built without Poppler support" << std::endl;
  return 2;
}
#endif
