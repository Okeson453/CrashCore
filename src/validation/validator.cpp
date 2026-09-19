#include "validation/live_validator.hpp"

namespace crashcore {

const char* kValidatorModule = "crashcore.validation.validator";

// validateKindToString lives in live_validator.cpp; this TU anchors validator symbols.
const char* validatorModuleName() {
  return kValidatorModule;
}

} // namespace crashcore
