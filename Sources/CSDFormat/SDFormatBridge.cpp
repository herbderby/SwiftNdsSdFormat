/**
 * @file SDFormatBridge.cpp
 * @brief C bridge implementation wrapping the C++23 formatter API.
 */

#include "SDFormatBridge.h"

#include "SDFormatResult.h"
#include "SectorWriter.h"

using sdFormat::SectorWriter;

namespace {
/**
 * @brief Converts C++ SDFormatResult to C SDFormatResultC.
 */
SDFormatResultC toC(SDFormatResult result) {
  switch (result) {
    case SDFormatResult::Success:
      return SDFormat_Success;
    case SDFormatResult::AccessDenied:
      return SDFormat_AccessDenied;
    case SDFormatResult::DeviceBusy:
      return SDFormat_DeviceBusy;
    case SDFormatResult::InvalidDevice:
      return SDFormat_InvalidDevice;
    case SDFormatResult::IOError:
      return SDFormat_IOError;
    case SDFormatResult::TooSmall:
      return SDFormat_TooSmall;
    case SDFormatResult::UnknownError:
    default:
      return SDFormat_UnknownError;
  }
}
}  // namespace

// -----------------------------------------------------------------------------
// Context-Based Granular API
// -----------------------------------------------------------------------------

/**
 * @brief Opaque context structure holding a SectorWriter instance.
 *
 * Non-owning of the file descriptor. The caller is responsible for managing
 * the FD lifecycle.
 */
struct SDFormatContext {
  SectorWriter writer;

  SDFormatContext(int fd, uint64_t sectorCount, std::string_view volumeLabel)
      : writer(SectorWriter::make(fd, sectorCount, volumeLabel)) {}
};

extern "C" SDFormatContextRef SDFormat_CreateContext(int fd,
                                                     uint64_t sectorCount,
                                                     const char* volumeLabel) {
  static constexpr size_t kMinSectorsForMBR = 18432;  // ~9MB

  if (fd < 0 || volumeLabel == nullptr) {
    return nullptr;
  }

  if (sectorCount < kMinSectorsForMBR) {
    return nullptr;
  }

  return new SDFormatContext(fd, sectorCount, volumeLabel);
}

extern "C" void SDFormat_DestroyContext(SDFormatContextRef ctx) { delete ctx; }

extern "C" SDFormatResultC SDFormat_WriteMBR(SDFormatContextRef ctx) {
  if (ctx == nullptr) {
    return SDFormat_InvalidDevice;
  }
  return toC(ctx->writer.writeMBR());
}

extern "C" SDFormatResultC SDFormat_WriteBootSector(SDFormatContextRef ctx) {
  if (ctx == nullptr) {
    return SDFormat_InvalidDevice;
  }
  return toC(ctx->writer.writeFat32BootSector());
}

extern "C" SDFormatResultC SDFormat_WriteFSInfo(SDFormatContextRef ctx) {
  if (ctx == nullptr) {
    return SDFormat_InvalidDevice;
  }
  return toC(ctx->writer.writeFSInfo());
}

extern "C" SDFormatResultC SDFormat_WriteFAT(SDFormatContextRef ctx) {
  if (ctx == nullptr) {
    return SDFormat_InvalidDevice;
  }
  return toC(ctx->writer.writeFat32Tables());
}

extern "C" SDFormatResultC SDFormat_WriteRootDirectory(SDFormatContextRef ctx) {
  if (ctx == nullptr) {
    return SDFormat_InvalidDevice;
  }
  return toC(ctx->writer.writeRootDirectory());
}
