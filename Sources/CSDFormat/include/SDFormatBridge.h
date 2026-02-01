/**
 * @file SDFormatBridge.h
 * @brief C interface for the NDS Flashcart SD Card Formatter.
 *
 * This header provides a C-compatible interface to the C++23 formatting
 * library, enabling Swift and other languages to link against it.
 */

#ifndef SDFORMAT_BRIDGE_H
#define SDFORMAT_BRIDGE_H

/**
 * @brief Result codes for SD Card Formatter operations (C-compatible).
 */
typedef enum {
  SDFormat_Success = 0,
  SDFormat_AccessDenied,
  SDFormat_DeviceBusy,
  SDFormat_InvalidDevice,
  SDFormat_IOError,
  SDFormat_TooSmall,
  SDFormat_UnknownError
} SDFormatResultC;

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Context-Based Granular API (Phase 2)
// -----------------------------------------------------------------------------

#include <stdint.h>

/**
 * @brief Opaque handle to a format operation context.
 *
 * The context encapsulates the device, layout, and volume label. It does NOT
 * own the file descriptor; the caller is responsible for opening/closing it.
 */
typedef struct SDFormatContext* SDFormatContextRef;

/**
 * @brief Creates a format context for a given file descriptor.
 *
 * The context holds the pre-calculated layout and references the device. The
 * caller retains ownership of the FD and must keep it open for the lifetime
 * of the context.
 *
 * @param fd An open file descriptor with write permissions.
 * @param sectorCount Total sectors on the device (from GetDeviceCapacity).
 * @param volumeLabel Volume label (max 11 characters, will be uppercased).
 * @return A new context, or NULL on failure (e.g., device too small).
 */
SDFormatContextRef SDFormat_CreateContext(int fd, uint64_t sectorCount,
                                          const char* volumeLabel);

/**
 * @brief Destroys a format context and frees associated memory.
 *
 * Does NOT close the file descriptor.
 *
 * @param ctx The context to destroy. No-op if NULL.
 */
void SDFormat_DestroyContext(SDFormatContextRef ctx);

/**
 * @brief Writes the Master Boot Record sector.
 */
SDFormatResultC SDFormat_WriteMBR(SDFormatContextRef ctx);

/**
 * @brief Writes the FAT32 Boot Sector (and backup at sector 6).
 */
SDFormatResultC SDFormat_WriteBootSector(SDFormatContextRef ctx);

/**
 * @brief Writes the FSInfo sector (and backup at sector 7).
 */
SDFormatResultC SDFormat_WriteFSInfo(SDFormatContextRef ctx);

/**
 * @brief Writes and zeroes both FAT tables.
 */
SDFormatResultC SDFormat_WriteFAT(SDFormatContextRef ctx);

/**
 * @brief Writes and zeroes the root directory cluster.
 */
SDFormatResultC SDFormat_WriteRootDirectory(SDFormatContextRef ctx);

#ifdef __cplusplus
}
#endif

#endif  // SDFORMAT_BRIDGE_H
