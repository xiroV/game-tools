// TODO(Brian): Example/test exporter for now
class SwiftExporter: Exporter {
    static var id: String = "swift"
    static var name: String = "Swift"
    static var ext: String = "swift"
    static func generate(editor: Editor) -> String {
           print("This generates swift output") 
            return "Swift output"

    }
}
