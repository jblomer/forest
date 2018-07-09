#include "RColumnModel.hxx"

namespace Toy {
}

std::ostream& operator<<(std::ostream& out, const Toy::RColumnType value) {
    switch (value) {
      case Toy::RColumnType::kUnknown: return out << "UNKNWON";
      case Toy::RColumnType::kFloat: return out << "FLOAT";
      case Toy::RColumnType::kInt: return out << "INT";
      case Toy::RColumnType::kBit: return out << "BIT";
      case Toy::RColumnType::kByte: return out << "BYTE";
      case Toy::RColumnType::kOffset: return out << "OFFSET";
      default: return out << "ERROR";
    }
}