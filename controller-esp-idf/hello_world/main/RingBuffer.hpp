// A simple ring buffer that drops data exceeding its size
#pragma once

// #include "config.hpp"
// #include "esp_log.h"
#include <cstddef>
#include <cstring>
#include <optional>
#include <span>

template <size_t bufferSize> class RingBuffer {
  char *start;
  char *end;
  char buffer[bufferSize];

public:
  RingBuffer() {
    start = buffer;
    end = buffer;
  }
  RingBuffer(const RingBuffer &other) {
    start = buffer + (other.start - other.buffer);
    end = buffer + (other.start - other.buffer);
  };
  RingBuffer &operator=(const RingBuffer &other) {
    start = buffer + (other.start - other.buffer);
    end = buffer + (other.start - other.buffer);
    return *this;
  };
  // Does advance end
  size_t write(const std::span<const char> &data);
  // Does not advance start
  // You should call consumeBytes afterwards
  std::optional<std::pair<std::span<const char>, std::span<const char>>>
  read(size_t maxBytes);
  // Advance start by x bytes
  // You can bring the buffer into invalid states if you mark more bytes than
  // are available
  void markConsumed(size_t maxBytes);

  size_t size() const;
};

template <size_t bufferSize>
size_t RingBuffer<bufferSize>::write(const std::span<const char> &data) {
  auto maxBytes = data.size();

  size_t availableBytes = bufferSize - size() - 1;
  size_t writeBytes = availableBytes > maxBytes ? maxBytes : availableBytes;

  size_t bytesBeforeWrap = buffer + bufferSize - end;
  size_t firstSpanLength =
      bytesBeforeWrap >= writeBytes ? writeBytes : bytesBeforeWrap;
  size_t secondSpanLength = writeBytes - firstSpanLength;

  // ESP_LOGI(RINGBUFFER_LOG_TAG, "Inserting into the ringbuffer");
  // ESP_LOGI(RINGBUFFER_LOG_TAG, "maxBytes:         %u", maxBytes);
  // ESP_LOGI(RINGBUFFER_LOG_TAG, "availableBytes:   %u", availableBytes);
  // ESP_LOGI(RINGBUFFER_LOG_TAG, "writeBytes:       %u", writeBytes);
  // ESP_LOGI(RINGBUFFER_LOG_TAG, "bytesBeforeWrap:  %u", bytesBeforeWrap);
  // ESP_LOGI(RINGBUFFER_LOG_TAG, "firstSpanLength:  %u", firstSpanLength);
  // ESP_LOGI(RINGBUFFER_LOG_TAG, "secondSpanLength: %u", secondSpanLength);
  // ESP_LOGI(RINGBUFFER_LOG_TAG, "buffer:           %u", (unsigned int)buffer);
  // ESP_LOGI(RINGBUFFER_LOG_TAG, "start:            %u", (unsigned int)start);
  // ESP_LOGI(RINGBUFFER_LOG_TAG, "end:              %u", (unsigned int)end);

  std::memcpy(end, data.data(), firstSpanLength);
  end += firstSpanLength;

  if (end == buffer + bufferSize) {
    end = buffer;
  }
  if (secondSpanLength != 0) {
    std::memcpy(buffer, data.data() + firstSpanLength, secondSpanLength);
    end += secondSpanLength;
  }

  return firstSpanLength + secondSpanLength;
}

template <size_t bufferSize>
std::optional<std::pair<std::span<const char>, std::span<const char>>>
RingBuffer<bufferSize>::read(size_t maxBytes) {
  if (start == end) {
    return std::nullopt;
  }
  size_t availableBytes = size();
  size_t readBytes = availableBytes > maxBytes ? maxBytes : availableBytes;

  size_t bytesBeforeWrap = buffer + bufferSize - start;
  size_t firstSpanLength =
      bytesBeforeWrap >= readBytes ? readBytes : bytesBeforeWrap;
  size_t secondSpanLength = readBytes - firstSpanLength;

  return std::pair(std::span(start, firstSpanLength),
                   std::span(buffer, secondSpanLength));
}

template <size_t bufferSize>
void RingBuffer<bufferSize>::markConsumed(size_t consumedBytes) {
  size_t totalOffset = start + consumedBytes - buffer;
  size_t newOffset = totalOffset % bufferSize;
  start = buffer + newOffset;
}

template <size_t bufferSize> size_t RingBuffer<bufferSize>::size() const {
  if (start <= end) {
    return end - start;
  }
  return end + bufferSize - start;
}