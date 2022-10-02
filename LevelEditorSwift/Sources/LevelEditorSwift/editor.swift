import Raylib

enum EditorState {
    case editing
    case closing
    case keyValueEditor
    case blockTypeEditor
    case export
}

enum ObjectTypeParameter {
    case name
    case color
}

enum KeyOrValue {
    case key
    case value
}

struct ObjectData {
    var key: String
    var value: String
}

struct ObjectType {
    let name: String
    let color: Color

    init(_ name: String, _ color: Color) {
        self.name = name
        self.color = color
    }
}

enum MessageType {
    case error
    case info
    case success
}

struct Object {
    var x, y, width, height, rotation: Int
    var type: Int
    var data: Array<ObjectData>
}

struct EditorMessage {
    let message: String
    let expiration: Float 
    let type: MessageType
}

struct Window {
    let height: Int32     
    let width: Int32
}

struct Editor {
    var state: EditorState = .editing 
    var objects: Array<Object>
    var selectedObject: Int
    var keyOrValue: KeyOrValue 
    var objectTypeParameter: ObjectTypeParameter
    var editBlockTypeIndex: Int
    var outputDelimiter: Character
    var version: Int
    var levelNameError: Bool
    var levelName: String
    var messages: Array<EditorMessage>
    var objectTypes: Array<ObjectType> = [
        ObjectType("Block", .red),
        ObjectType("Spawn", .blue)
    ]  
    var selectedExporter: Int 
    var window: Window
    var fontSize: Int
    var cameraTarget: Bool // TODO(Brian)
}
