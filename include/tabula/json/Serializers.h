// Lightweight JSON serializer declarations for Tables and
// RectangularTextContainer
#pragma once
#include <string>
namespace tabula {
class Table;
template <typename T> class RectangularTextContainer;
} // namespace tabula

namespace tabula {
namespace json {
std::string serializeTable(const tabula::Table &table);
// helper overload for Cell/TextChunk containers used by table serialization
std::string serializeRectangular(
    const tabula::RectangularTextContainer<tabula::TextChunk> &rtc);
} // namespace json
} // namespace tabula
