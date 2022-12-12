import Raylib
import SwiftRaylibGui

func loadLevel(_ editor: Editor) {
    // TODO Not yet implemented
}

func control() {
//func control(_ editor: Editor, windows: Window, exporters: Exporters) {
    // TODO Not yet implemented
}

func drawMessages() {

}

func main() {
    var exit = false

    let window = Window(
        width: 1600,
        height: 1000,
        title: "Level Editor"
    )
    
    var camera = Camera2D()
    camera.target = Vector2(x: Float(window.width)/2, y: Float(window.height)/2);
    camera.offset = Vector2(x: Float(window.width)/2, y: Float(window.height)/2);
    camera.rotation = 0.0;
    camera.zoom = 1.0;

    let editor = Editor(
        version: 1,
        outputDelimiter: ";",
        window: window,
        fontSize: 20,
        camera: camera
    )
    
    let qc = QuakeConsole()


    Raylib.initWindow(editor.window.width, editor.window.height, editor.window.title)
    Raylib.setTargetFPS(60)
    let gui = SwiftRaylibGui(230, 432)
    
    Raylib.setExitKey(.null)

    // TODO(Brian) Figure font out later
    _ = Raylib.loadFont("assets/fonts/OverpassMono/OverpassMono-Regular.ttf")
    
    // Raylib.SetTextureFilter(fontDefault.texture, .TEXTURE_FILTER_BILINEAR);

    if (CommandLine.arguments.count > 1) {
        editor.levelName = CommandLine.arguments.first!
        loadLevel(editor)
    } else {
        editor.levelName = "TestLevel1.lvl"
        loadLevel(editor)
    }
    
    while !exit {
        qc.input()
        if !qc.isOpen {
            if Raylib.isKeyPressed(.escape) {
                exit = true
            }
        }
        
        qc.update()
        
        Raylib.beginDrawing()
            Raylib.clearBackground(.darkGray)
            editor.drawGrid()
            Raylib.beginMode2D(editor.camera)
                editor.drawObjects()
            Raylib.endMode2D()
            editor.drawHelp(gui: gui)
            //drawWindows(&editor, &windows, exporters)
            editor.drawMessages();
            qc.render(window: window)
        
            if (editor.showFPS) { Raylib.drawFPS(20, 20) }

        Raylib.endDrawing()
        

        editor.camera.zoom = 1
        editor.update()
        
        if qc.isOpen {
            continue
        }
        
        editor.control(window: editor.window)
        editor.camera.zoom = clamp(camera.zoom + Raylib.getMouseWheelMove(), 0.5, 2.0)
        editor.camera.target = editor.cameraTarget
    }

    Raylib.closeWindow()
}

func colorFromType(type: MessageType) -> Color {
    switch (type) {
        case .success:
            return .green
        case .error:
            return .red
        case .info:
            return .white
    }
}


main()

