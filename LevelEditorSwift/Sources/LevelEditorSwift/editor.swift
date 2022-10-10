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

protocol EditorMode {
    func control(window: Window) -> Void
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

struct Constants {
    let MOVE_INTERVAL: Int32 = 10
    let MOVE_DELAY = 5.0
    let MOVE_HOLD_DELAY: Double = 40
    let ROTATION_INTERVAL: Int32 = 15
    let RESIZE_INTERVAL: Int32 = 50
    let CAMERA_MOVE_SPEED: Float32 = 5.0
    let GRID_DISTANCE = 50
}

var timeSinceLastMovePress: Float = 0.0
var timeSinceLastMove: Double = 0.0

let CONSTANTS = Constants()

class Editor: EditorMode  {
    var version: Int
    var state: EditorState = .editing 
    var objects: Array<Object> = []
    var camera: Camera2D
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
        fontSize: Int32,
        camera: Camera2D
    ) {
        self.version = version
        self.outputDelimiter = outputDelimiter
        self.window = window
        self.fontSize = fontSize
        self.cameraTarget = Vector2(x: Float(window.width/2), y: Float(window.height/2))
        self.camera = camera
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
        if Raylib.isKeyDown(.leftAlt) && Raylib.isKeyDown(.function4) {
            if self.isElementSelected() {
                self.selectedObject = -1
            } else {
                self.state = EditorState.closing
            }
        }
        
        if Raylib.windowShouldClose {
            self.state = EditorState.closing
        }
        
        if Raylib.isKeyPressed(.function10) {
            self.showFPS = !self.showFPS;
        }
        
        switch (self.state) {
            case EditorState.closing:
                // self.window.exitWindow.control();
                break;
            case EditorState.export:
                // self.window.exportWindow.control();
                break;
            case EditorState.keyValueEditor:
                // self.window.keyValueEditorWindow.control();
                break;
            case EditorState.blockTypeEditor:
                // self.window.blockTypeEditorWindow.control();
                break;
            case EditorState.editing:
                if Raylib.isKeyPressed(.escape) {
                    if (self.isElementSelected()) {
                        self.selectedObject = -1
                    } else {
                        self.state = EditorState.closing
                    }
                }

                if Raylib.isKeyPressed(.letterM) {
                    self.state = EditorState.export;
                }

                if Raylib.isKeyPressed(.pageUp) {
                    self.camera.zoom += 0.5;
                }

                if Raylib.isKeyPressed(.pageDown) {
                    self.camera.zoom -= 0.5;
                }

                if Raylib.isKeyPressed(.letterY) {
                    self.state = EditorState.blockTypeEditor;
                }

                if Raylib.isKeyPressed(.letterG) {
                    self.showGrid = !self.showGrid;
                }

                // NEW OBJECT
                if Raylib.isKeyPressed(.letterN) {
                    let obj = Object(x: self.window.width / 2 - 50, y: self.window.height / 2 - 50, width: 100, height: 100)
                    self.objects.append(obj)
                    self.selectedObject = self.objects.count - 1
                }

                // SWITCH SELECTED
                if Raylib.isKeyPressed(.tab) {
                    if self.selectedObject + 1 < self.objects.count {
                        self.selectedObject += 1;
                    } else {
                        if self.objects.count > 0 {
                            self.selectedObject = 0;
                        } else {
                            self.selectedObject = -1;
                        }
                                
                    }
                }

                if Raylib.isKeyPressed(.letterC) {
                    self.copyBlock();
                }

                if (self.isElementSelected()) {
                    let upPressed = Raylib.isKeyPressed(.up);
                    let downPressed = Raylib.isKeyPressed(.down);
                    let leftPressed = Raylib.isKeyPressed(.left);
                    let rightPressed = Raylib.isKeyPressed(.right);
                    let isMoveDown = Raylib.isKeyDown(.up) || Raylib.isKeyDown(.down) ||
                    Raylib.isKeyDown(.left) || Raylib.isKeyDown(.right);

                    // MOVEMENT
                    if upPressed {
                        self.objects[self.selectedObject].y -= CONSTANTS.MOVE_INTERVAL;
                        timeSinceLastMovePress = 0;
                    }

                    if downPressed {
                        self.objects[self.selectedObject].y += CONSTANTS.MOVE_INTERVAL;
                        timeSinceLastMovePress = 0;
                    }

                    if leftPressed {
                        self.objects[self.selectedObject].x -= CONSTANTS.MOVE_INTERVAL;
                        timeSinceLastMovePress = 0;
                    }

                    if rightPressed {
                        self.objects[self.selectedObject].x += CONSTANTS.MOVE_INTERVAL;
                        timeSinceLastMovePress = 0;
                    }

                    if timeSinceLastMovePress > Float(CONSTANTS.MOVE_HOLD_DELAY) {
                        if (Raylib.isKeyDown(.up) && !upPressed) {
                            if timeSinceLastMove > CONSTANTS.MOVE_DELAY {
                                self.objects[self.selectedObject].y -= CONSTANTS.MOVE_INTERVAL;
                            }
                        }

                        if Raylib.isKeyDown(.down) && !downPressed {
                            if (timeSinceLastMove > CONSTANTS.MOVE_DELAY) {
                                self.objects[self.selectedObject].y += CONSTANTS.MOVE_INTERVAL;
                            }
                        }

                        if Raylib.isKeyDown(.left) && !leftPressed {
                            if (timeSinceLastMove > CONSTANTS.MOVE_DELAY) {
                                self.objects[self.selectedObject].x -= CONSTANTS.MOVE_INTERVAL;
                            }
                        }

                        if Raylib.isKeyDown(.right) && !rightPressed {
                            if (timeSinceLastMove > CONSTANTS.MOVE_DELAY) {
                                self.objects[self.selectedObject].x += CONSTANTS.MOVE_INTERVAL;
                            }
                        }
                    }

                    if isMoveDown {
                        timeSinceLastMovePress += Raylib.getFrameTime() * 100.0;
                        if (timeSinceLastMove > CONSTANTS.MOVE_DELAY) {
                            timeSinceLastMove = 0;
                        } else {
                            timeSinceLastMove += Double(Raylib.getFrameTime() * 100.0);
                        }
                    } else {
                        timeSinceLastMove = 0;
                    }
                    

                    // RESIZING
                    if Raylib.isKeyPressed(.letterS) {
                        self.objects[self.selectedObject].height += CONSTANTS.RESIZE_INTERVAL;
                    }

                    if Raylib.isKeyPressed(.letterW) {
                        if self.objects[self.selectedObject].height >= CONSTANTS.RESIZE_INTERVAL {
                            self.objects[self.selectedObject].height -= CONSTANTS.RESIZE_INTERVAL;
                        }
                    }

                    if Raylib.isKeyPressed(.letterA) {
                        if (self.objects[self.selectedObject].width >= CONSTANTS.RESIZE_INTERVAL) {
                            self.objects[self.selectedObject].width -= CONSTANTS.RESIZE_INTERVAL;
                        }
                    }

                    if Raylib.isKeyPressed(.letterD) {
                        self.objects[self.selectedObject].width += CONSTANTS.RESIZE_INTERVAL;
                    }
                    
                    // ROTATE
                    if Raylib.isKeyPressed(.letterQ) {
                        self.objects[self.selectedObject].rotation = (self.objects[self.selectedObject].rotation - CONSTANTS.ROTATION_INTERVAL) % 360;

                    }

                    if Raylib.isKeyPressed(.letterE) {
                        self.objects[self.selectedObject].rotation = (self.objects[self.selectedObject].rotation + CONSTANTS.ROTATION_INTERVAL) % 360;
                    }

                    // DELETE
                    if Raylib.isKeyPressed(.delete) {
                        if self.isElementSelected() {
                            self.objects.remove(at: self.selectedObject)
                            if (self.objects.count > 0) {
                                self.selectedObject = 0;
                            } else {
                                self.selectedObject = -1;
                            }
                        }
                    }

                    // CHANGE TYPE
                    if Raylib.isKeyPressed(.letterT) {
                        if self.objects[self.selectedObject].type + 1 >= self.objectTypes.count {
                            self.objects[self.selectedObject].type = 0;
                        } else {
                            self.objects[self.selectedObject].type += 1;
                        }
                    }

                    // OPEN KEY VALUE WINDOW
                    if Raylib.isKeyPressed(.letterV) {
                        self.state = EditorState.keyValueEditor;
                    }

                 } else {
                    // MOVEMENT
                     if Raylib.isKeyDown(.up) {
                         self.camera.target.y -= CONSTANTS.CAMERA_MOVE_SPEED;
                     } else if Raylib.isKeyDown(.down) {
                         self.camera.target.y += CONSTANTS.CAMERA_MOVE_SPEED;
                    }

                     if Raylib.isKeyDown(.left) {
                         self.camera.target.x -= CONSTANTS.CAMERA_MOVE_SPEED;
                    }

                     if (Raylib.isKeyDown(.right)) {
                         self.camera.target.x += CONSTANTS.CAMERA_MOVE_SPEED;
                    }

                }
        
                break;
        }
    }
    
    func copyBlock() -> Void {
        if !self.isElementSelected() {
            return
        }
        
        let objectToCopy = self.objects[self.selectedObject]
        
        var newObject = Object(x: objectToCopy.x, y: objectToCopy.y, width: objectToCopy.width, height: objectToCopy.height)
        newObject.rotation = objectToCopy.rotation
        newObject.type = objectToCopy.type
        for data in objectToCopy.data {
            newObject.data.append(data)
        }
        
        self.objects.append(newObject)
        
        // TODO(Jonas): Should we set the new object as the active here?
    }
}
