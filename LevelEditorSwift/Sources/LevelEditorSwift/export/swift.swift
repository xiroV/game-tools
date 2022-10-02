// TODO(Brian): Example/test exporter for now
func swiftExporter() -> ExportConfig {
    return ExportConfig(
        id: "swift",
        name: "Swift",
        ext: "swift",
        generate: { editor in
            print("This generates swift output") 
            return "Swift output"
        }
    )
}
