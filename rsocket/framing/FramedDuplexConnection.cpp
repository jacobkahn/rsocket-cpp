// Copyright 2004-present Facebook. All Rights Reserved.

#include "rsocket/framing/FramedDuplexConnection.h"
#include "rsocket/framing/FrameSerializer.h"
#include "rsocket/framing/FramedReader.h"
#include "rsocket/framing/FramedWriter.h"

namespace rsocket {

using namespace yarpl::flowable;

FramedDuplexConnection::~FramedDuplexConnection() {}

FramedDuplexConnection::FramedDuplexConnection(
    std::unique_ptr<DuplexConnection> connection,
    ProtocolVersion protocolVersion)
    : inner_(std::move(connection)),
      protocolVersion_(std::make_shared<ProtocolVersion>(protocolVersion)) {}

yarpl::Reference<Subscriber<std::unique_ptr<folly::IOBuf>>>
FramedDuplexConnection::getOutput() noexcept {
  return yarpl::make_ref<FramedWriter>(
      inner_->getOutput(), protocolVersion_);
}

void FramedDuplexConnection::setInput(
    yarpl::Reference<Subscriber<std::unique_ptr<folly::IOBuf>>> framesSink) {
  if(!inputReader_) {
    inputReader_ = yarpl::make_ref<FramedReader>(protocolVersion_);
    inner_->setInput(inputReader_);
  }
  inputReader_->setInput(std::move(framesSink));
}

} // reactivesocket
