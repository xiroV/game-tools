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
    var x, y, width, height, rotation: Int32
    var type: Int
    var data: Array<ObjectData>
    
    init(x: Int32, y: Int32, width: Int32, height: Int32) {
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.rotation = 0
        self.type = 0
        self.data = []
    }
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
    
    func isElementSelected() -> Bool {
        return self.selectedObject != -1
    }
    
    func drawObjects() -> Void {
        if (self.isElementSelected()) {
            let selectedObject = self.objects[self.selectedObject];

            if (selectedObject.width == 0 && selectedObject.height == 0) {
                Raylib.drawCircleLines(
                    selectedObject.x,
                    selectedObject.y,
                    5.0,
                    .green
                );
            } else {
                drawRect(
                    Float32(selectedObject.x - 1),
                    Float32(selectedObject.y - 1),
                    Float32(selectedObject.width + 2),
                    Float32(selectedObject.height + 2),
                    Float32(selectedObject.rotation),
                    .green
                );
            }
        }
        
        for object in self.objects {
            if (object.width == 0 && object.height == 0) {
                Raylib.drawCircle(object.x, object.y, 5, self.objectTypes[object.type].color);
            } else {
                drawRect(
                    Float32(object.x),
                    Float32(object.y),
                    Float32(object.width == 0 ? 1 : object.width),
                    Float32(object.height == 0 ? 1 : object.height),
                    Float32(object.rotation),
                    self.objectTypes[object.type].color
                );
            }
        }
    }
    
    func control(window: Window) -> Void {
        if (Raylib.isKeyPressed(.letterN)) {
            let obj = Object(x: self.window.width / 2 - 50, y: self.window.height / 2 - 50, width: 100, height: 100)
            self.objects.append(obj)
            self.selectedObject = self.objects.count - 1
        }
    }
}
