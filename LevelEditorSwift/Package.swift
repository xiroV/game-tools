// swift-tools-version:5.5
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "LevelEditorSwift",
    dependencies: [
        // Dependencies declare other packages that this package depends on.
        // .package(url: /* package url */, from: "1.0.0"),
        .package(url: "https://github.com/STREGAsGate/Raylib.git", branch: "master"),
        .package(url: "git@github.com:xiroV/SwiftRaylibGui.git", branch: "main")
    ],
    targets: [
        // Targets are the basic building blocks of a package. A target can define a module or a test suite.
        // Targets can depend on other targets in this package, and on products in packages this package depends on.
        .executableTarget(
            name: "LevelEditorSwift",
            dependencies: ["Raylib", "SwiftRaylibGui"]),
        .testTarget(
            name: "LevelEditorSwiftTests",
            dependencies: ["LevelEditorSwift"]),
    ]
)
