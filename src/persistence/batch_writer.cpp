#include "persistence/batch_writer.hpp"

namespace crashcore {

const char* kBatchWriterModule = "crashcore.persistence.batch_writer";

std::size_t batchWriterFlush(BatchWriter& w) {
  return w.flush();
}

std::uint64_t batchWriterWritten(const BatchWriter& w) {
  return w.written();
}

std::uint64_t batchWriterDropped(const BatchWriter& w) {
  return w.dropped();
}

} // namespace crashcore
