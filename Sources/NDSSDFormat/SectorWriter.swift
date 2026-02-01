import CSDFormat

/// Encapsulates a formatting context and provides throwing wrappers for atomic
/// write operations.
///
/// This struct uses `~Copyable` for RAII semantics - the context is
/// automatically destroyed when the struct goes out of scope.
public struct SectorWriter: ~Copyable {
    private let context: SDFormatContextRef

    // MARK: - Lifecycle

    /// Creates a formatting context for the given file descriptor.
    ///
    /// - Parameters:
    ///   - fd: An open file descriptor with write permissions.
    ///   - sectorCount: Total sectors on the device.
    ///   - volumeLabel: Volume label (max 11 characters, will be uppercased).
    /// - Throws: `FormatterError.invalidDevice` if context creation fails.
    public init(fd: Int32, sectorCount: UInt64, volumeLabel: String) throws {
        guard
            let ctx = volumeLabel.withCString({ label in
                SDFormat_CreateContext(fd, sectorCount, label)
            })
        else {
            throw FormatterError.invalidDevice
        }
        self.context = ctx
    }

    deinit {
        SDFormat_DestroyContext(context)
    }

    // MARK: - Atomic Write Operations

    /// Writes the Master Boot Record sector.
    public func writeMasterBootRecord() throws {
        try check(SDFormat_WriteMBR(context))
    }

    /// Writes the FAT32 Boot Sector (and backup at sector 6).
    public func writeBootSector() throws {
        try check(SDFormat_WriteBootSector(context))
    }

    /// Writes the FSInfo sector (and backup at sector 7).
    public func writeFileSystemInfo() throws {
        try check(SDFormat_WriteFSInfo(context))
    }

    /// Writes and zeroes both FAT tables.
    public func writeFAT32() throws {
        try check(SDFormat_WriteFAT(context))
    }

    /// Writes and zeroes the root directory cluster.
    public func writeRootDirectory() throws {
        try check(SDFormat_WriteRootDirectory(context))
    }

    // MARK: - Private Helpers

    private func check(_ result: SDFormatResultC) throws {
        guard result == SDFormat_Success else {
            throw FormatterError(result: result)
        }
    }
}
