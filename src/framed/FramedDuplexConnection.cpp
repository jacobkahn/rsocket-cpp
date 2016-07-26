// Copyright 2004-present Facebook.  All rights reserved.
#include "FramedDuplexConnection.h"

#include <glog/logging.h>
#include <folly/Memory.h>
#include "FramedReader.h"
#include "FramedWriter.h"
#include <memory>

namespace reactivesocket {

FramedDuplexConnection::FramedDuplexConnection(
    std::unique_ptr<DuplexConnection> connection, Stats& stats)
    : connection_(std::move(connection)), stats_(stats) {}

FramedDuplexConnection::~FramedDuplexConnection() {
  // to make sure we close the parties when the connection dies
  if (outputWriter_) {
    outputWriter_->cancel();
  }
  if (inputReader_) {
    inputReader_->onComplete();
  }
}

Subscriber<Payload>& FramedDuplexConnection::getOutput() noexcept {
  if (!outputWriter_) {
    outputWriter_ = folly::make_unique<FramedWriter>(connection_->getOutput());
  }
  return *outputWriter_;
}

void FramedDuplexConnection::setInput(Subscriber<Payload>& framesSink) {
  CHECK(!inputReader_);
  inputReader_ = folly::make_unique<FramedReader>(framesSink);
  connection_->setInput(*inputReader_);
}

} // reactive socket
