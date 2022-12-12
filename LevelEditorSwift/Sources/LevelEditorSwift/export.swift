struct ExportConfig {
    let id: String
    let name: String
    let ext: String 
    let generate: (Editor) -> String
}

class Export {
    let exporters: Array<ExportConfig>

    init() {
        self.exporters = [
            swiftExporter(),
            zigExporter()
        ]
    }
}
