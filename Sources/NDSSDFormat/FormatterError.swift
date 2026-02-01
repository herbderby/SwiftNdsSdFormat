import CSDFormat

/// Errors thrown by the SectorWriter.
public enum FormatterError: Error, Equatable, Sendable {
    case accessDenied
    case deviceBusy
    case invalidDevice
    case ioError
    case tooSmall
    case unknown(UInt32)

    public init(result: SDFormatResultC) {
        switch result {
        case SDFormat_AccessDenied: self = .accessDenied
        case SDFormat_DeviceBusy: self = .deviceBusy
        case SDFormat_InvalidDevice: self = .invalidDevice
        case SDFormat_IOError: self = .ioError
        case SDFormat_TooSmall: self = .tooSmall
        default: self = .unknown(result.rawValue)
        }
    }

    /// Human-readable description of the error.
    public var localizedDescription: String {
        switch self {
        case .accessDenied: return "Access denied"
        case .deviceBusy: return "Device busy"
        case .invalidDevice: return "Invalid device"
        case .ioError: return "I/O error"
        case .tooSmall: return "Device too small"
        case .unknown(let code): return "Unknown error (code: \(code))"
        }
    }
}
