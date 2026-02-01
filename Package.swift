// swift-tools-version: 6.2
import PackageDescription

let package = Package(
    name: "SwiftNdsSdFormat",
    platforms: [.macOS(.v26)],
    products: [
        .library(name: "NDSSDFormat",
                 targets: ["NDSSDFormat"])
    ],
    dependencies: [
        .package(url: "https://github.com/herbderby/NdsSDFormat.git",
                 from: "1.0.0")
    ],
    targets: [
        .target(
            name: "CSDFormat",
            dependencies: [
                .product(name: "NDSSDFormatCore",
                         package: "NdsSDFormat")
            ],
            path: "Sources/CSDFormat",
            sources: ["SDFormatBridge.cpp"],
            publicHeadersPath: "include",
            cxxSettings: [
                .unsafeFlags(["-std=c++23"])
            ]
        ),
        .target(
            name: "NDSSDFormat",
            dependencies: ["CSDFormat"],
            path: "Sources/NDSSDFormat",
            swiftSettings: [
                .swiftLanguageMode(.v6),
                .enableExperimentalFeature("StrictConcurrency"),
            ]
        ),
    ]
)
