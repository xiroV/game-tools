protocol Exporter {
    static var id: String { get }
    static var name: String { get }
    static var ext: String { get }
    static func generate(editor: Editor) -> String
}

class Export {
    let exporters: Array<Exporter>

    init() {
        self.exporters = [
        //    swiftExporter() 
        ]
    }
}
