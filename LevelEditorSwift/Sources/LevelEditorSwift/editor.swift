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
    let width: Int32
    let height: Int32     
    let title: String
}

class Editor {
    var version: Int
    var state: EditorState = .editing 
    var objects: Array<Object> = []
    var selectedObject: Int = -1
    var editKeyValueIndex: Int = -1
    var editBlockTypeIndex: Int = -1
    var keyOrValue: KeyOrValue = .key
    var objectTypeParameter: ObjectTypeParameter = .name
    var outputDelimiter: Character
    var levelNameError: Bool = false
    var levelName: String = ""
    var messages: Array<EditorMessage> = []
    var objectTypes: Array<ObjectType> = [
        ObjectType("Block", .red),
        ObjectType("Spawn", .blue)
    ]  
    var selectedExporter: Int = 0
    var window: Window
    var fontSize: Int32 = 10
    var cameraTarget: Vector2
    var closeEditor: Bool = false
    var showGrid: Bool = true
    var showHelp: Bool = true
    var showFPS: Bool = false

    init (
        version: Int,
        outputDelimiter: Character,
        window: Window,
        fontSize: Int32
    ) {
        self.version = version
        self.outputDelimiter = outputDelimiter
        self.window = window
        self.fontSize = fontSize
        self.cameraTarget = Vector2(x: Float(window.width/2), y: Float(window.height/2))
    }
}
