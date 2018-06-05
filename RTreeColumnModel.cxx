#include "RTreeColumnModel.hxx"

namespace Toy {
}

std::ostream& operator<<(std::ostream& out, const Toy::RTreeColumnType value) {
    switch (value) {
      case Toy::RTreeColumnType::kUnknown: return out << "UNKNWON";
      case Toy::RTreeColumnType::kFloat: return out << "FLOAT";
      case Toy::RTreeColumnType::kInt: return out << "INT";
      case Toy::RTreeColumnType::kBit: return out << "BIT";
      case Toy::RTreeColumnType::kByte: return out << "BYTE";
      case Toy::RTreeColumnType::kOffset: return out << "OFFSET";
      default: return out << "ERROR";
    }
}